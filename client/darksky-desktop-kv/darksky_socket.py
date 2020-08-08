import socketio
import logging, coloredlogs

class SocketClient():
    def __init__(self, **kwargs):
        super(SocketClient, self).__init__(**kwargs)

        self.logger = logging.getLogger(__name__)
        coloredlogs.install(level='DEBUG', logger=self.logger)
        self.logger.info("Init")

        self.sio = socketio.Client()
        self.connected = False

        self.sio.on('connect', self.connect)
        self.sio.on('disconnect', self.disconnect)
        self.sio.on('updateState', self.updateState)


    def isConnected(self):
        return self.connected

    def connect(self):
        self.logger.info('connection established')
        self.connected = True

    def disconnect(self):
        self.logger.info('Connection closed')
        self.connected = False

    def updateState(self, data):
        self.logger.info('message received with {}'.format(data))
        # sio.emit('my response', {'response': 'my response'})


    def doConnect(self):     
        self.logger.info("Connecting")
        self.sio.connect("http://localhost:22502")


    def doDisconnect(self):
        self.logger.info("Disconnecting")
        self.sio.disconnect()
