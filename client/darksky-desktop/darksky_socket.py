from PySide2.QtCore import QObject, Slot, Signal, Property
import socketio

class SocketClient(QObject):
    sio = socketio.Client()

    connected = False
    connectedChanged = Signal(bool)

    @Property(bool, notify=connectedChanged)
    def isConnected(self):
        return self.connected

    @sio.event
    def connect(self):
        print('SocketIO => connection established')
        self.connected = True
        self.connectedChanged.emit(SocketClient.connected)

    @sio.event
    def disconnect(self):
        print('SocketIO => Connection closed')
        self.connected = False
        self.connectedChanged.emit(SocketClient.connected)

    @sio.event
    def my_message(self, data):
        print('message received with ', data)
        sio.emit('my response', {'response': 'my response'})


    @Slot()
    def doConnect(self):
        print("Connecting to DarkSky")
        self.sio.on('connect', self.connect)
        self.sio.on('disconnect', self.disconnect)
        
        print("SocketIO => Connecting...")
        self.sio.connect("http://localhost:22502")


    @Slot()
    def doDisconnect(self):
        print("SocketIO => Disconnecting")
        self.sio.disconnect()
