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
                    'stopAt': 0
                },
                'dec': {
                    'state': 'unknown',
                    'position': 0,
                    'stopAt': 0
                }
            },
            'serial': {
                'port': '',
                'connected': False
            }
        }

        self.requestedState = {
            'lnb': {
                'voltage': 0,
                'carrier': False
            },
            'motors': {
                'stopAll': False,
                'ra': {
                    'state': 'stopped',
                    'position': 0,
                    'stopAt': 0
                },
                'dec': {
                    'state': 'stopped',
                    'position': 0,
                    'stopAt': 0
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

    # packet.command = MOTOR_DEC_STOP_POS;
    # packet.arg1 = darkSkyContext.motor1.stopPosition;

    # packet.command = MOTOR_RA_STATE;
    # packet.arg1 = darkSkyContext.motor2.state;

    # packet.command = MOTOR_RA_POSITION;
    # packet.arg1 = darkSkyContext.motor2.position;

    # packet.command = MOTOR_RA_STOP_POS;
    # packet.arg1 = darkSkyContext.motor2.stopPosition;

    # packet.command = LNB_STATE;
    # packet.arg1 = darkSkyContext.lnb.power;
    # packet.arg2 = darkSkyContext.lnb.carrier;

    # State update methods

    # Dec motor updates
    def updateDecState(self, packet: Packet):
        self.updateMotorState('dec', packet)

    def updateDecPosition(self, packet: Packet):
        self.updateMotorPosition('dec', packet)

    def updateDecStopAt(self, packet: Packet):
        self.updateMotorStopAt('dec', packet)

    # RA motor updates
    def updateRaState(self, packet: Packet):
        self.updateMotorState('ra', packet)

    def updateRaPosition(self, packet: Packet):
        self.updateMotorPosition('ra', packet)

    def updateRaStopAt(self, packet: Packet):
        self.updateMotorStopAt('ra', packet)

    # Motor request methods

    # Dec motor requests
    def requestDecState(self, packet: Packet):
        self.requestMotorState('dec', packet)

    def requestDecPosition(self, packet: Packet):
        self.requestMotorPosition('dec', packet)

    def requestDecStopAt(self, packet: Packet):
        self.requestMotorStopAt('dec', packet)

    # RA motor requests
    def requestRaState(self, packet: Packet):
        self.requestMotorState('ra', packet)

    def requestRaPosition(self, packet: Packet):
        self.requestMotorPosition('ra', packet)

    def requestRaStopAt(self, packet: Packet):
        self.requestMotorStopAt('ra', packet)

    # Generic motor update functions
    def updateMotorState(self, motor: str, packet: Packet):
        self.state['motors'][motor]['state'] = State.MOTOR_STATES[packet.GetPayload()[
            'arg1']]

    def updateMotorPosition(self, motor: str, packet: Packet):
        self.state['motors'][motor]['position'] = packet.GetPayload()['arg1']

    def updateMotorStopAt(self, motor: str, packet: Packet):
        self.state['motors'][motor]['stopAt'] = packet.GetPayload()['arg1']

    # Generic motor request functions
    def requestMotorState(self, motor: str, state: str):
        self.requestedState['motors'][motor]['state'] = state
        self.processStateUpdate()

    def requestMotorPosition(self, motor: str, state: int):
        self.requestedState['motors'][motor]['position'] = state
        self.processStateUpdate()

    def requestMotorStopAt(self, motor: str, state: int):
        self.requestedState['motors'][motor]['stopAt'] = state
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
        if self.requestedState['motor']['ra']['stopAt'] != self.state['motor']['ra']['stopAt']:
            pass

        # Dec motor state
        if self.requestedState['motor']['dec']['state'] != self.state['motor']['dec']['state']:
            pass
        if self.requestedState['motor']['dec']['position'] != self.state['motor']['dec']['position']:
            pass
        if self.requestedState['motor']['dec']['stopAt'] != self.state['motor']['dec']['stopAt']:
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

        azStep = 10
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

            if bumpAlt and (context.state["dish"]["alt"] == 90 or context.state["dish"]["alt"] == 0):
                altStep = -altStep

            context.logger.info("Simulation AZ : {}, ALT : {}, LNB : {}".format(
                context.state["dish"]["az"], context.state["dish"]["alt"], context.state["lnb"]["strength"]))

            context.UpdateDishPositionHistory()

            time.sleep(30.05)
