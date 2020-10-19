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


class State(Singleton):
    MOTOR_STATES = [
        'stopped',
        'forward',
        'reverse',
        'stall'
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

        self.requestedState = []

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

    # Control board state updates

    # Tick rate 500ms

    # order of commands
    # packet.command = MOTOR_DEC_STATE;
    # packet.arg1 = darkSkyContext.motor1.state;

    # packet.command = MOTOR_DEC_POSITION;
    # packet.arg1 = darkSkyContext.motor1.position;

    # packet.command = MOTOR_DEC_DELTA_POS;
    # packet.arg1 = darkSkyContext.motor1.deltaPosition;

    # packet.command = MOTOR_RA_STATE;
    # packet.arg1 = darkSkyContext.motor2.state;

    # packet.command = MOTOR_RA_POSITION;
    # packet.arg1 = darkSkyContext.motor2.position;

    # packet.command = MOTOR_RA_DELTA_POS;
    # packet.arg1 = darkSkyContext.motor2.deltaPosition;

    # packet.command = LNB_STATE;
    # packet.arg1 = darkSkyContext.lnb.power;
    # packet.arg2 = darkSkyContext.lnb.carrier;

    # Motor request methods

    # Dec motor requests
    def requestDecState(self, client: str, value: str):
        self.logger.info("{} : Request Dec State : {}".format(client, value))
        self.requestMotorState('dec', value)

    def requestDecPosition(self, client: str, value: int):
        self.logger.info(
            "{} : Request Dec Position : {}".format(client, value))
        self.requestMotorPosition('dec', value)

    def requestDecDelta(self, client: str, value: int):
        self.logger.info("{} : Request Dec Delta : {}".format(client, value))
        self.requestMotorDelta('dec', value)

    # RA motor requests
    def requestRaState(self, client: str, value: str):
        self.logger.info("{} : Request Ra State : {}".format(client, value))
        self.requestMotorState('ra', value)

    def requestRaPosition(self, client: str, value: int):
        self.logger.info("{} : Request Ra Position : {}".format(client, value))
        self.requestMotorPosition('ra', value)

    def requestRaDelta(self, client: str, value: int):
        self.logger.info("{} : Request Ra Delta : {}".format(client, value))
        self.requestMotorDelta('ra', value)

    def requestLnbVoltage(self, client: str, value: int):
        self.logger.info("{} : Request LNB Voltage : {}".format(client, value))
        self.requestedState.append({'lnb.voltage': value})
        self.processStateUpdate()

    def requestLnbCarrier(self, client: str, value: int):
        self.logger.info("{} : Request LNB Carrier : {}".format(client, value))
        self.requestedState.append({'lnb.carrier': value})
        self.processStateUpdate()

    def requestCalibration(self, client: str, value: bool):
        self.logger.info("{} : Request Calibrating : {}".format(client, value))
        self.update('calibrating', value)
        # self.processStateUpdate()

    # Generic motor request functions
    def requestMotorState(self, motor: str, value: str):
        self.requestedState.append({'motors.' + motor + '.state': value})
        self.processStateUpdate()

    def requestMotorPosition(self, motor: str, value: int):
        self.requestedState.append({'motors.' + motor + '.position': value})
        self.processStateUpdate()

    def requestMotorDelta(self, motor: str, value: int):
        self.requestedState.append({'motors.' + motor + '.delta': value})
        self.processStateUpdate()

    def requestStopAll(self, client: str, value: bool):
        self.requestedState.append({'motors.stopAll': value})
        self.processStateUpdate()

    def requestState(self, client: str, states: dict):
        for state in states.items():
            self.logger.info("Request State : {} : {} -> {}".format(client, state[0], state[1]))

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
                    Packet.binaryToMotorState(packet.arg1))

    def updateMotorPosition(self, motor: str, packet: Packet):
        self.update('motors.' + motor + '.position', packet.arg1)

    def updateMotorDelta(self, motor: str, packet: Packet):
        self.update('motors.' + motor + '.delta', packet.arg1)

    def updateLNBVoltage(self, packet: Packet):
        self.update(
            'lnb.voltage', Packet.binaryToLNBVoltage(packet.arg1))

    def updateLNBCarrier(self, packet: Packet):
        self.update(
            'lnb.carrier', Packet.binaryToLNBCarrier(packet.arg2))

    # Compare requested state to actual state and issue commands
    def processStateUpdate(self):
        from mcp_serial import MotorPowerController
        mpc = MotorPowerController()

        # LNB State

        # Stop all motor state

        # RA motor state

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
