import logging
import datetime
import threading
import math
import time
import random
from packet import Packet, PacketCommand
from singleton import Singleton


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
            'calibrating' : False,
            'gps': {
                'mode': 0
            },
            'lnb': {
                'voltage': 0,
                'carrier': False,
                'strength': 0
            },
            'dish': {
                'alt': 0,
                'az': 0,
                'historyPath': [],
                'historyStrength': []
            },
            'motors': {
                'stopAll': False,
                'ra': {
                    'state': 'unknown',
                    'position': 0,
                    'delta': 0
                },
                'dec': {
                    'state': 'unknown',
                    'position': 0,
                    'delta': 0
                }
            },
            'serial': {
                'port': '',
                'connected': False
            }
        }

        self.requestedState = {
            'calibrating' : False,
            'lnb': {
                'voltage': 0,
                'carrier': False
            },
            'motors': {
                'stopAll': False,
                'ra': {
                    'state': 'stopped',
                    'position': 0,
                    'delta': 0
                },
                'dec': {
                    'state': 'stopped',
                    'position': 0,
                    'delta': 0
                }
            }
        }

    def updateGPS(self, gpsFix):
        # self.logger.info(gpsFix)
        # self.logger.info('GPS Update {}.{} @ {}'.format(gpsFix['lat'], gpsFix['lon'], gpsFix['time']))
        self.state['gps'] = gpsFix

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
        self.logger.info("{} : Request Dec Position : {}".format(client, value))
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
        self.requestedState['lnb']['voltage'] = value
        self.processStateUpdate()

    def requestLnbCarrier(self, client: str, value: int):
        self.logger.info("{} : Request LNB Carrier : {}".format(client, value))
        self.requestedState['lnb']['carrier'] = value
        self.processStateUpdate()

    def requestCalibration(self, client: str, value: bool):
        self.requestedState['calibrating'] = value
        self.state['calibrating'] = value
        self.logger.info("{} : Request Calibrating : {}".format(client, value))
        # self.processStateUpdate()

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

    # Generic motor update functions
    def updateMotorState(self, motor: str, packet: Packet):
        self.state['motors'][motor]['state'] = Packet.binaryToMotorState(packet.arg1)

    def updateMotorPosition(self, motor: str, packet: Packet):
        self.state['motors'][motor]['position'] = packet.arg1

    def updateMotorDelta(self, motor: str, packet: Packet):
        self.state['motors'][motor]['delta'] = packet.arg1

    def updateLNBVoltage(self, packet: Packet):
        self.state['lnb']['voltage'] = Packet.binaryToLNBVoltage(packet.arg1)

    def updateLNBCarrier(self, packet: Packet):
        self.state['lnb']['carrier'] = Packet.binaryToLNBCarrier(packet.arg2)


    # Generic motor request functions
    def requestMotorState(self, motor: str, state: str):
        self.requestedState['motors'][motor]['state'] = state
        self.processStateUpdate()

    def requestMotorPosition(self, motor: str, state: int):
        self.requestedState['motors'][motor]['position'] = state
        self.processStateUpdate()

    def requestMotorDelta(self, motor: str, state: int):
        self.requestedState['motors'][motor]['delta'] = state
        self.processStateUpdate()

    # Compare requested state to actual state and issue commands
    def processStateUpdate(self):
        # LNB State
        if self.requestedState['lnb']['voltage'] != self.state['lnb']['voltage']:
            pass
        if self.requestedState['lnb']['carrier'] != self.state['lnb']['carrier']:
            pass

        # Stop all motor state
        if self.requestedState['motor']['stopAll'] != self.state['motor']['stopAll']:
            pass

        # RA motor state
        if self.requestedState['motor']['ra']['state'] != self.state['motor']['ra']['state']:
            pass
        if self.requestedState['motor']['ra']['position'] != self.state['motor']['ra']['position']:
            pass
        if self.requestedState['motor']['ra']['delta'] != self.state['motor']['ra']['delta']:
            pass

        # Dec motor state
        if self.requestedState['motor']['dec']['state'] != self.state['motor']['dec']['state']:
            pass
        if self.requestedState['motor']['dec']['position'] != self.state['motor']['dec']['position']:
            pass
        if self.requestedState['motor']['dec']['delta'] != self.state['motor']['dec']['delta']:
            pass

    def UpdateDishPositionHistory(self):
        if len(self.state["dish"]["historyPath"]) == 0:
            self.state["dish"]["historyPath"].append(
                [self.state["dish"]["az"], self.state["dish"]["alt"]])
            self.state["dish"]["historyStrength"].append(
                self.state["lnb"]["strength"])
            return

        daz = abs(self.state["dish"]["az"] -
                  self.state["dish"]["historyPath"][0][0])
        dalt = abs(self.state["dish"]["alt"] -
                   self.state["dish"]["historyPath"][0][1])

        if daz >= 1 or dalt >= 1:
            self.state["dish"]["historyPath"].insert(0,
                                                     [self.state["dish"]["az"], self.state["dish"]["alt"]])
            self.state["dish"]["historyStrength"].insert(0,
                                                         self.state["lnb"]["strength"])

        if len(self.state["dish"]["historyPath"]) > 2500:
            self.state["dish"]["historyPath"].pop()
            self.state["dish"]["historyStrength"].pop()

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

        context.state["dish"]["az"] = 0
        context.state["dish"]["alt"] = 0

        while (context.simulating):
            context.state["lnb"]["strength"] = math.fabs(math.cos(
                (context.state["dish"]["az"] / 180) * math.pi)) * math.fabs(math.cos((context.state["dish"]["alt"] / 90) * math.pi)) * lnbRange

            bumpAlt = False
            context.state["dish"]["az"] += azStep
            if context.state["dish"]["az"] >= 360:
                context.state["dish"]["az"] -= 360
                context.state["dish"]["alt"] += altStep
                bumpAlt = True
            elif context.state["dish"]["az"] < 0:
                context.state["dish"]["az"] += -360
                context.state["dish"]["alt"] += altStep
                bumpAlt = True

            if bumpAlt and context.state["dish"]["alt"] >= 90:
                altStep = -altStep
                context.state["dish"]["alt"] += altStep
            
            if bumpAlt and context.state["dish"]["alt"] <= 0:
                altStep = -altStep
                context.state["dish"]["alt"] += altStep

            context.logger.info("Simulation AZ : {}, ALT : {}, LNB : {}".format(
                context.state["dish"]["az"], context.state["dish"]["alt"], context.state["lnb"]["strength"]))

            context.UpdateDishPositionHistory()

            time.sleep(0.05)
