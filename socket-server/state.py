import logging
import datetime
import threading
import math
import time
import random
from packet import Packet, PacketCommand
from singleton import Singleton
from astropy.time import Time
from astropy.coordinates import EarthLocation
from collections import deque


class State(Singleton):
    MOTOR_STATES = [
        'stopped',
        'forward',
        'reverse',
        'stall'
    ]

    HighPriorityStates = ['motors.stopAll']

    StateSpamList = [
        'gps.time',
        'time.jde',
        'time.sidereal.local',
        'time.sidereal.gmt'
    ]

    def init(self):
        self.logger = logging.getLogger(__name__)
        self.logger.info('Init')
        self.state = {
            'calibrating': False,
            'gps.mode': 0,
            'lnb.voltage': 0,
            'lnb.carrier': False,
            'lnb.strength': 0,
            'dish.alt': 0,
            'dish.az': 0,
            'dish.historyPath': [],
            'dish.historyStrength': [],
            'motors.stopAll': False,
            'motors.az.state': 'unknown',
            'motors.az.position': 0,
            'motors.az.delta': 0,
            'motors.az.pwm': 0,
            'motors.az.autodrive': False,
            'motors.alt.state': 'unknown',
            'motors.alt.position': 0,
            'motors.alt.delta': 0,
            'motors.alt.pwm': 0,
            'serial.port': '',
            'serial.connected': False,
            'time.jde': 0,
            'time.sidereal.local': 0,
            'time.sidereal.gmt': 0,
        }

        self.requestedStateCondition = threading.Condition()
        self.requestedStateUpdatedCondition = threading.Condition()
        self.requestedState = deque()
        requestedState = None

        self.requestProcessingThread = threading.Thread(
            target=self.processRequestedState, args=(), daemon=True)
        self.requestProcessingThread.start()

    def update(self, key, value):
        if key in self.state and self.state[key] == value:
            return

        if key not in State.StateSpamList:
            self.logger.info("State set {} -> {}".format(key, value))

        if len(self.requestedState) > 0:
            currentStateRequest = self.requestedState[-1]
            curKey = currentStateRequest[0]
            curValue = currentStateRequest[1]
            if curKey == key and curValue == value:
                # Pop the thing from the deqeue
                self.requestedState.pop()
                # Wake up the processing thread
                with self.requestedStateUpdatedCondition:
                    self.requestedStateUpdatedCondition.notify()


        self.state[key] = value
        # Trigger update diff function

    def get(self, key):
        value = None
        if key in self.state:
            value = self.state[key]
        self.logger.info("State get {} <- {}".format(key, value))
        return value

    def updateGPS(self, gpsFix):
        # self.logger.info(gpsFix)
        # self.logger.info('GPS Update {}.{} @ {}'.format(gpsFix['lat'], gpsFix['lon'], gpsFix['time']))
        for key in gpsFix:
            if key != 'time':
                self.update('gps.' + key, gpsFix[key])
            else:
                if (gpsFix['mode'] >= 2):
                    eLocation = EarthLocation.from_geodetic(lon=gpsFix['lon'],
                                                            lat=gpsFix['lat'],
                                                            height=gpsFix['alt'])
                    time = Time(gpsFix[key], format='fits',
                                scale='utc', location=eLocation)
                    self.update('gps.' + key, time.value)
                    self.update('time.jde', time.jd)
                    self.update('time.sidereal.local',
                                time.sidereal_time('mean').hour * 3600)
                    self.update('time.sidereal.gmt', time.sidereal_time(
                        'mean', 'greenwich').hour * 3600)

    def requestState(self, client: str, states: dict):
        with self.requestedStateCondition:
            for state in states.items():
                if state[0] in State.HighPriorityStates:
                    self.logger.info(
                        "Adding Hi Pri Request State : {} : {} -> {}".format(client, state[0], state[1]))
                    self.requestedState.append(state)
                else:
                    self.logger.info(
                        "Adding Lo Pri Request State : {} : {} -> {}".format(client, state[0], state[1]))
                    self.requestedState.appendleft(state)
            self.requestedStateCondition.notify()

    def processRequestedState(self):
        self.logger.info("Staring State Processing Thread")
        count = 0
        lastState = ""
        while True:
            with self.requestedStateUpdatedCondition:
                with self.requestedStateCondition:
                    while len(self.requestedState):
                        currentStateRequest = self.requestedState[-1]
                        state = currentStateRequest[0]
                        if lastState != state:
                            count = 0
                            lastState = state
                        value = currentStateRequest[1]
                        self.logger.info(
                            "Processing state request : Ref {} : {} -> {}".format(count, state, value))
                        count = count + 1
                        if self.processStateUpdate(state, value):
                            count = 0
                            self.requestedState.pop()
                        else:
                            self.requestedStateUpdatedCondition.wait(0.5)

                    # self.logger.info("processRequestedState --> Sleep...")
                    self.requestedStateCondition.wait(0.1)
                    # self.logger.info("processRequestedState --> Wake!!")
                    self.safetyTick()

    def safetyTick(self):
        pass

    # Compare requested state to actual state and issue commands
    def processStateUpdate(self, state: str, value):
        # self.logger.info("Processing state update {} -> {}".format(state, value))
        requestedState = None

        # Catch all
        if self.state[state] == value:
            return True

        # Calibration
        if (state == "calibrating"):
            # We don't set the currentRequestedState here because this is an internal state
            self.requestState("local" , {"motors.stopAll": False})
            self.requestState("local" , {"motors.stopAll": True})
            self.state[state] = value
            return True

        # Stop all motors
        elif (state == "motors.stopAll"):
            requestedState = Packet.CreateFromStruct(
                PacketCommand.STOP_ALL_MOTORS, value, 0, 0)

        # DEC motor state
        elif (state == "motors.alt.state"):
            requestedState = Packet.CreateFromStruct(
                PacketCommand.MOTOR_ALT_STATE, Packet.MotorStateToBinary(value), 0, 0)

        # DEC motor state
        elif (state == "motors.alt.position"):
            requestedState = Packet.CreateFromStruct(
                PacketCommand.MOTOR_ALT_POSITION, value, 0, 0)

        # DEC motor delta position
        elif (state == "motors.alt.delta"):
            requestedState = Packet.CreateFromStruct(
                PacketCommand.MOTOR_ALT_DELTA_POS, value, 0, 0)

        # RA motor state
        elif (state == "motors.az.state"):
            requestedState = Packet.CreateFromStruct(
                PacketCommand.MOTOR_AZ_STATE, Packet.MotorStateToBinary(value), 0, 0)

        # RA motor state
        elif (state == "motors.az.position"):
            requestedState = Packet.CreateFromStruct(
                PacketCommand.MOTOR_AZ_POSITION, value, 0, 0)

        # RA motor delta position
        elif (state == "motors.az.delta"):
            requestedState = Packet.CreateFromStruct(
                PacketCommand.MOTOR_AZ_DELTA_POS, value, 0, 0)

        # RA motor auto drive
        elif (state == "motors.az.autodrive"):
            requestedState = Packet.CreateFromStruct(
                PacketCommand.MOTOR_AZ_AUTODRIVE, value, 0, 0)

        # LNB State
        elif (state == "lnb.voltage"):
            requestedState = Packet.CreateFromStruct(
                PacketCommand.LNB_STATE, value, self.state['lnb.carrier'], 0)
        elif (state == "lnb.carrier"):
            requestedState = Packet.CreateFromStruct(
                PacketCommand.LNB_STATE, self.state['lnb.power'], value, 0)

        # Default case
        else:
            self.logger.error(
                "Unknown state flag! {} -> {}".format(state, value))
            return True

        if requestedState != None:
            from mcp_serial import MotorPowerController
            MotorPowerController().SendPacket(requestedState)
            return False

    # State update methods

    # Dec motor updates
    def updateAltState(self, packet: Packet):
        self.updateMotorState('alt', packet)

    def updateAltPosition(self, packet: Packet):
        self.updateMotorPosition('alt', packet)

    def updateAltDelta(self, packet: Packet):
        self.updateMotorDelta('alt', packet)

    def updateAltPWM(self, packet: Packet):
        self.updateMotorPWM('alt', packet)

    # RA motor updates
    def updateAzState(self, packet: Packet):
        self.updateMotorState('az', packet)

    def updateAzPosition(self, packet: Packet):
        self.updateMotorPosition('az', packet)

    def updateAzDelta(self, packet: Packet):
        self.updateMotorDelta('az', packet)

    def updateAzPWM(self, packet: Packet):
        self.updateMotorPWM('az', packet)

    def updateAzAutoDrive(self, packet: Packet):
        self.update('motors.az.autodrive', packet.arg1 == 1)

    def updateStopAll(self, packet: Packet):
        self.update('motors.stopAll', packet.arg1)

    # Generic motor update functions
    def updateMotorState(self, motor: str, packet: Packet):
        self.update('motors.' + motor + '.state',
                    Packet.BinaryToMotorState(packet.arg1))

    def updateMotorPosition(self, motor: str, packet: Packet):
        self.update('motors.' + motor + '.position', packet.arg1)

    def updateMotorDelta(self, motor: str, packet: Packet):
        self.update('motors.' + motor + '.delta', packet.arg1)

    def updateMotorPWM(self, motor: str, packet: Packet):
        self.update('motors.' + motor + '.pwm', packet.arg1)

    def updateLNBVoltage(self, packet: Packet):
        self.update(
            'lnb.voltage', Packet.BinaryToLNBVoltage(packet.arg1))

    def updateLNBCarrier(self, packet: Packet):
        self.update(
            'lnb.carrier', Packet.BinaryToLNBCarrier(packet.arg2))

    def UpdateDishPositionHistory(self):
        if len(self.state.get("dish.historyPath")) == 0:
            self.state.get("dish.historyPath").append(
                [self.state.get("dish.az"), self.state.get("dish.alt")])
            self.state.get("dish.historyStrength").append(
                self.state.get("lnb.strength"))
            return

        dazalt = self.state.get("dish.historyPath")[0]
        daz = abs(self.state.get("dish.az") - dazalt[0])
        dalt = abs(self.state.get("dish.alt") - dazalt[1])

        if daz >= 1 or dalt >= 1:
            self.state.get("dish.historyPath").insert(
                0, [self.state.get("dish.az"), self.state.get("dish.alt")])
            self.state.get("dish.historyStrength").insert(
                0, self.state.get("lnb.strength"))

        if len(self.state.get("dish.historyPath")) > 2500:
            self.state.get("dish.historyPath").pop()
            self.state.get("dish.historyStrength").pop()

    def StartSimulation(self):
        self.simulating = False
        self.simulationThread = threading.Thread(
            target=self.SimulationThread, args=(self,), daemon=True)
        self.simulationThread.start()

    @staticmethod
    def SimulationThread(context):
        context.logger.info("Starting Simulation Thread")
        context.simulating = True

        azStep = 5
        altStep = 2
        lnbRange = 100

        context.update("dish.az", 0)
        context.update("dish.alt", 0)

        while (context.simulating):
            curAz = context.state.get("dish.az")
            curAlt = context.state.get("dish.alt")
            curStrength = math.fabs(math.cos((curAz / 180) * math.pi)) * math.fabs(
                math.cos((curAlt / 90) * math.pi)) * lnbRange

            context.logger.info("Simulation AZ : {}, ALT : {}, LNB : {}".format(
                curAz, curAlt, curStrength))

            context.UpdateDishPositionHistory()

            time.sleep(0.1)

            bumpAlt = False
            curAz = curAz + azStep

            if curAz >= 360:
                curAz -= 360
                curAlt += altStep
                bumpAlt = True
            elif curAz < 0:
                curAz += 360
                curAlt += altStep
                bumpAlt = True

            if bumpAlt and (curAlt >= 90 or curAlt <= 0):
                altStep = -altStep
                curAlt += altStep

            context.update("dish.az", curAz)
            context.update("dish.alt", curAlt)
            context.update("lnb.strength", curStrength)
