import logging
import coloredlogs
import socketio
import eventlet
from packet import Packet, PacketCommand
import threading
from singleton import Singleton
from state import State
import time

class SocketIOBackend(Singleton):
    def init(self):
        self.logger = logging.getLogger(__name__)
        self.logger.info("Creating DarkSky Socket.IO server")

        # for super debug socket io logging... comment these lines out...
        logging.getLogger('socketio').setLevel(logging.WARNING)
        logging.getLogger('engineio').setLevel(logging.WARNING)

        self.state = State()

        self.sioLock = threading.Lock()

        self.sio = socketio.Server(
            cors_allowed_origins='*', engineio_logger=True)
        self.app = socketio.WSGIApp(self.sio)
        self.sio.on('connect', self.connect)
        self.sio.on('disconnect', self.disconnect)
        # self.sio.on('comport.connect', self.comportConnect)
        # self.sio.on('updateRequestedState', self.state.updateRequestedState)
        self.sio.on('request.dec.state', self.state.requestDecState)
        self.sio.on('request.dec.position', self.state.requestDecPosition)
        self.sio.on('request.dec.delta', self.state.requestDecDelta)
        self.sio.on('request.ra.state', self.state.requestRaState)
        self.sio.on('request.ra.position', self.state.requestRaPosition)
        self.sio.on('request.ra.delta', self.state.requestRaDelta)
        self.sio.on('request.lnb.voltage', self.state.requestLnbVoltage)
        self.sio.on('request.lnb.carrier', self.state.requestLnbCarrier)
        self.sio.on('request.calibration', self.state.requestCalibration)

        self.stateThread = threading.Thread(
            target=self.StateUpdate, args=(self,), daemon=True)

        self.stateThread.start()

    @staticmethod
    def StateUpdate(context):
        context.logger.info(
            'Starting State Update Thread')
        context.sending = True
        while (context.sending):
            # context.logger.info('Emitting state over socket')
            context.updateState()
            time.sleep(1)

    def Listen(self):
        eventlet.wsgi.server(eventlet.listen(('', 22502)), self.app)

    def connect(self, sid, environ):
        self.logger.info("Client connected : {}".format(sid))
        self.updateState()
    
    def updateState(self):
        self.SendMessage('updateState', self.state.state)

    def disconnect(self, sid):
        self.logger.info("Client disconnected : {}".format(sid))

    def SendMessage(self, event, payload):
        # self.logger.info("Sending Socket IO {} => {}".format(event, payload))
        self.sioLock.acquire()
        self.sio.emit(event, payload)
        self.sioLock.release()
