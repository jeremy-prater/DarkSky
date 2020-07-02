import logging
import coloredlogs
import socketio
import eventlet
from packet import Packet, PacketCommand
from mcp_serial import MotorPowerController
import threading

class SocketIOBackend:
    instance = None

    @staticmethod
    def getInstance():
        if SocketIOBackend.instance == None:
            SocketIOBackend.instance = SocketIOBackend()
        return SocketIOBackend.instance

    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.logger.info("Creating DarkSky Socket.IO server")

        # for super debug socket io logging... comment these lines out...
        logging.getLogger('socketio').setLevel(logging.WARNING)
        logging.getLogger('engineio').setLevel(logging.WARNING)

        self.sioLock = threading.Lock()

        self.mcp = MotorPowerController.getInstance()
        self.sio = socketio.Server(
            cors_allowed_origins='*', engineio_logger=True)
        self.app = socketio.WSGIApp(self.sio)
        self.sio.on('connect', self.connect)
        self.sio.on('disconnect', self.disconnect)
        self.sio.on('comport.connect', self.comportConnect)
        #self.sio.on('request.dec.position', self.decState)
        self.sio.on('request.dec.state', self.decState)
        #self.sio.on('request.ra.position', self.raState)
        self.sio.on('request.ra.state', self.raState)
        self.sio.on('request.lnb.state', self.lnbState)

    def Listen(self):
        eventlet.wsgi.server(eventlet.listen(('', 8100)), self.app)

    def connect(self, sid, environ):
        self.logger.info("Client connected : {}".format(sid))
        self.SendPacket('comport.update', self.mcp.GetPorts())
        MotorPowerController.getInstance().SendStatus()

    def disconnect(self, sid):
        self.logger.info("Client disconnected : {}".format(sid))

    def SendPacket(self, event, payload):
        # self.logger.info("Sending Socket IO {} => {}".format(event, payload))
        self.sioLock.acquire()
        self.sio.emit(event, payload)
        self.sioLock.release()

    def comportConnect(self,  sid, comport):
        MotorPowerController.getInstance().Connect(comport)

    def setMotorState(self, sid, state, motor):
        arg = 0
        if state == "forward":
            arg = 1
        elif state == "reverse":
            arg = 2
        MotorPowerController.getInstance().SendPacket(Packet(motor, arg, 0, 0))

    def decState(self, sid, state):
        self.setMotorState(
            sid, state, PacketCommand.COMMAND_MOTOR_DEC_SET_STATE)

    def raState(self, sid, state):
        self.setMotorState(
            sid, state, PacketCommand.COMMAND_MOTOR_RA_SET_STATE)

    def lnbState(self, sid, state):
        self.logger.warn("LNB state not implemented yet {}".format(state))
