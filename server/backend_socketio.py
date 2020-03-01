import logging
import coloredlogs
import socketio
import eventlet
from packet import Packet, PacketCommand

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
        self.sio = socketio.Server(cors_allowed_origins='*')
        self.app = socketio.WSGIApp(self.sio)
        self.sio.on('connect', self.connect)
        self.sio.on('disconnect', self.disconnect)

    def Listen(self):
        eventlet.wsgi.server(eventlet.listen(('', 8100)), self.app)

    def connect(self, sid, environ):
        self.logger.info("Client connected : {}".format(sid))
        self.SendPacket(Packet(PacketCommand.COMMAND_REBOOT, 0, 0, 0))

    def disconnect(self, sid):
        self.logger.info("Client disconnected : {}".format(sid))

    def SendPacket(self, packet: Packet):
        payload = packet.GetPayload()
        self.sio.emit(payload['command'].name, payload)
