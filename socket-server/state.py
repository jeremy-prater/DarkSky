import logging
from backend_socketio import SocketIOBackend
import time
import threading
from packet import Packet, PacketCommand


class State:
    instance = None

    MOTOR_STATES = [
        'stopped',
        'forward',
        'reverse',
        'stall'
    ]

    @staticmethod
    def getInstance():
        if State.instance == None:
            State.instance = State()
        return State.instance

    def __init__(self):
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
        socket = SocketIOBackend.getInstance()
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

    def updateDecState(self, packet: Packet):
        self.updateMotorState('dec', packet)

    def updateRaState(self, packet: Packet):
        self.updateMotorState('ra', packet)

    def requestDecState(self, packet: Packet):
        self.requestMotorState('dec', packet)

    def requestRaState(self, packet: Packet):
        self.requestMotorState('ra', packet)

    def updateMotorState(self, motor: str, packet: Packet)
    {
        payload = packet.GetPayload()
        newState = payload['arg1']

        self.state.state['motors'][motor]['state'] = State.MOTOR_STATES[newState]
    }

    def requestMotorState(self, motor: str, state: str)
    {
        self.state.requestedState['motors'][motor]['state'] = state
        self.processStateUpdate()
    }

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
        if self.requestedState['motor']['ra']['state'] != self.state['motor']['ra']['state']:
            pass

        # Dec motor state
        if self.requestedState['motor']['dec']['state'] != self.state['motor']['dec']['state']:
            pass
        if self.requestedState['motor']['dec']['position'] != self.state['motor']['dec']['position']:
            pass
        if self.requestedState['motor']['dec']['state'] != self.state['motor']['dec']['state']:
            pass
