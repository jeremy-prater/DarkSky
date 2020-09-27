import serial
import serial.tools.list_ports
from packet import PacketCommand, Packet
from mcp_read_thread import MotorPowerControllerReader
import logging
import json
from state import State
from singleton import Singleton

class MotorPowerController(Singleton):
    def init(self):
        self.logger = logging.getLogger(__name__)
        self.logger.info("Creating DarkSky Motor Power Controller backend")
        self.serial = serial.Serial()
        self.serial.baudrate = 115200
        self.ports = []
        self.port = {}
        self.readerThread = None
        self.state = State()

    def GetPorts(self):
        self.ports = []
        for port in serial.tools.list_ports.comports():
            self.ports.append(port.__dict__)
        return self.ports

    def ConnectSimulator(self):
        self.port = "simulator"

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
        if self.serial.is_open:
            self.readerThread = MotorPowerControllerReader(self.serial);


    def SendStatus(self):
        self.state.update('serial.port', self.port["device"])
        self.state.update('serial.connected', self.serial.is_open)

    def SendPacket(self, packet: Packet):
        if self.serial.is_open:
            outData = packet.GetRawBuffer()
            self.logger.info("Sending to MCP : {}".format(outData))
            self.serial.write(outData)
