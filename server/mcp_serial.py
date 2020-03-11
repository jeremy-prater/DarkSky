import serial
import serial.tools.list_ports
from packet import PacketCommand, Packet
from mcp_read_thread import MotorPowerControllerReader
import logging
import json
import backend_socketio


class MotorPowerController:
    instance = None

    @staticmethod
    def getInstance():
        if MotorPowerController.instance == None:
            MotorPowerController.instance = MotorPowerController()
        return MotorPowerController.instance

    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.logger.info("Creating DarkSky Motor Power Controller backend")
        self.serial = serial.Serial()
        self.serial.baudrate = 115200
        self.ports = []
        self.port = {}
        self.readerThread = None

    def GetPorts(self):
        self.ports = []
        for port in serial.tools.list_ports.comports():
            self.ports.append(port.__dict__)
        return self.ports

    def Connect(self, comport):
        if self.serial.is_open:
            if self.readerThread != None:
                self.logger.warn("Destroy Existing Thread reader...");
                # self.readerThread.shutdown()
            self.serial.close()

        self.port = comport
        self.logger.info("Connecting to comport {}".format(comport))
        self.serial.port = self.port["device"]
        self.serial.open()
        self.SendStatus()
        self.readerThread = MotorPowerControllerReader(self.serial);

    def SendStatus(self):
        backend_socketio.SocketIOBackend.getInstance().SendPacket('comport.status', self.serial.is_open)

    # def SendPacket(packet: Packet):
