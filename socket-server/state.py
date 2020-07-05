import logging
from backend_socketio import SocketIOBackend
import time
import threading
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
                'fix': 0
            },
            'lnb': {
                'voltage': 0,
                'carrier': False
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
            },
            'sdr': {}
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

        self.stateThread = threading.Thread(
            target=self.StateUpdate, args=(self,), daemon=True)

        self.stateThread.start()

    @staticmethod
    def StateUpdate(context):
        socket = SocketIOBackend()
        context.logger.info(
            'Starting State Update Thread')
        context.sending = True
        while (context.sending):
            context.logger.info('Emitting state over socket')
            context.logger.info(context.state)

            time.sleep(1)

    def updateGPS(self, gpsFix):
        # self.logger.info(gpsFix)
        # self.logger.info('GPS Update {}.{} @ {}'.format(gpsFix['lat'], gpsFix['lon'], gpsFix['time']))
        self.state['gps'] = gpsFix

        # Here we need to signal the astronomical coordinates to update
        # 1. time changed
        # 2. coordinates may have changed
        # 3. altiude correction
        # 4. precession
        #
        # ... do math

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
        self.state['motors'][motor]['stoAt'] = packet.GetPayload()['arg1']

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
