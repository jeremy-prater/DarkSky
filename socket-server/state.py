import logging
from backend_socketio import SocketIOBackend
import time
import threading


class State:
    instance = None

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
                'ra': {
                    'position': 0,
                    'state': 'stopped',
                    'stopAt': 0
                },
                'dec': {
                    'position': 0,
                    'state': 'stopped',
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
                'ra': {
                    'position': 0,
                    'state': 'stopped',
                    'stopAt': 0
                },
                'dec': {
                    'position': 0,
                    'state': 'stopped',
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
