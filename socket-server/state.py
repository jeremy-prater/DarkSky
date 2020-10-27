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
            'motors.ra.state': 'unknown',
            'motors.ra.position': 0,
            'motors.ra.delta': 0,
            'motors.dec.state': 'unknown',
            'motors.dec.position': 0,
            'motors.dec.delta': 0,
            'serial.port': '',
            'serial.connected': False,
            'time.jde': 0,
            'time.sidereal.local': 0,
            'time.sidereal.gmt': 0,
        }

        self.requestedStateCondition = threading.Condition()
        self.requestedState = deque()
        self.currentRequestedState = None

        self.requestProcessingThread = threading.Thread(
            target=self.processRequestedState, args=(), daemon=True)
        self.requestProcessingThread.start()

    def update(self, key, value):
        if key in self.state and self.state[key] == value:
            return

        self.logger.info("State set {} -> {}".format(key, value))
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
        while True:
            with self.requestedStateCondition:
                while len(self.requestedState) and (self.currentRequestedState == None):
                    currentStateRequest = self.requestedState.pop()
                    state = currentStateRequest[0]
                    value = currentStateRequest[1]
                    self.logger.info(
                        "Processing state request {} -> {}".format(state, value))
                    self.processStateUpdate(state, value)

                self.logger.info("processRequestedState --> Sleep...")
                self.requestedStateCondition.wait()
                self.logger.info("processRequestedState --> Wake!!")

    # Compare requested state to actual state and issue commands
    def processStateUpdate(self, state: str, value):
        if self.currentRequestedState != None:
            self.logger.error(
                "processStateUpdate was called and currentRequestedState is not None!")
            self.logger.error("currentRequestedState.magic    : {}".format(
                self.currentRequestedState.magic))
            self.logger.error("currentRequestedState.command  : {}".format(
                self.currentRequestedState.command))
            self.logger.error("currentRequestedState.arg1     : {}".format(
                self.currentRequestedState.arg1))
            self.logger.error("currentRequestedState.arg2     : {}".format(
                self.currentRequestedState.arg2))
            self.logger.error("currentRequestedState.arg3     : {}".format(
                self.currentRequestedState.arg3))
            self.logger.error("currentRequestedState.checksum : {}".format(
                self.currentRequestedState.checksum))

        from mcp_serial import MotorPowerController
        mpc = MotorPowerController()
        # Stop all motor state

        self.logger.info("Processing state update {} -> {}".format(state, value))

        # Calibration
        if (state == "calibrating"):
            # We don't set the currentRequestedState here because this is an internal state
            self.state[state] = value

        # DEC motor state
        elif (state == "motors.dec.state"):
            self.currentRequestedState = Packet.CreateFromStruct(
                PacketCommand.MOTOR_DEC_STATE, Packet.MotorStateToBinary(value), 0, 0)

        # packet.command = MOTOR_DEC_POSITION;
        # packet.arg1 = darkSkyContext.motor1.position;

        # packet.command = MOTOR_DEC_DELTA_POS;
        # packet.arg1 = darkSkyContext.motor1.deltaPosition;

        # RA motor state
        # packet.command = MOTOR_RA_STATE;
        # packet.arg1 = darkSkyContext.motor2.state;

        # packet.command = MOTOR_RA_POSITION;
        # packet.arg1 = darkSkyContext.motor2.position;

        # packet.command = MOTOR_RA_DELTA_POS;
        # packet.arg1 = darkSkyContext.motor2.deltaPosition;

        # LNB State
        # packet.command = LNB_STATE;
        # packet.arg1 = darkSkyContext.lnb.power;
        # packet.arg2 = darkSkyContext.lnb.carrier;
        else:
            self.logger.error("Unknown state flag! {} -> {}".format(state, value))

    # State update methods

    # Dec motor updates
    def updateDecState(self, packet: Packet):
        self.updateMotorState('dec', packet)

    def updateDecPosition(self, packet: Packet):
        self.updateMotorPosition('dec', packet)

    def updateDecDelta(self, packet: Packet):
        self.updateMotorDelta('dec', packet)

    # RA motor updates
    def updateRaState(self, packet: Packet):
        self.updateMotorState('ra', packet)

    def updateRaPosition(self, packet: Packet):
        self.updateMotorPosition('ra', packet)

    def updateRaDelta(self, packet: Packet):
        self.updateMotorDelta('ra', packet)

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
