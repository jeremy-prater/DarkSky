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
        self.simulating = False

    def GetPorts(self):
        self.ports = []
        for port in serial.tools.list_ports.comports():
            self.ports.append(port.__dict__)
        return self.ports

    def ConnectSimulator(self):
        self.port = "simulator"
        self.simulating = True
        self.logger.info("MPC is simulating!")

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
            self.readerThread = MotorPowerControllerReader(self);


    def SendStatus(self):
        self.state.update('serial.port', self.port["device"])
        self.state.update('serial.connected', self.serial.is_open)

    def SendPacket(self, packet: Packet):
        if self.simulating:
            self.processReceivedPacket(packet)
            return

        if self.serial.is_open:
            outData = packet.GetRawBuffer()
            self.logger.info("Sending to MCP : {}".format(outData))
            self.serial.write(outData)

    def processReceivedPacket(self, packet):
        # We have a packet!
        if packet.command == PacketCommand.BOOT:
            # mcpSocketIO.SendMessage('signal.boot', True)
            pass
        elif packet.command == PacketCommand.STOP_ALL_MOTORS:
            self.state.updateStopAll(packet)
        elif packet.command == PacketCommand.MOTOR_ALT_POSITION:
            self.state.updateAltPosition(packet)
        elif packet.command == PacketCommand.MOTOR_ALT_DELTA_POS:
            self.state.updateAltDelta(packet)
        elif packet.command == PacketCommand.MOTOR_ALT_STATE:
            self.state.updateAltState(packet)
        elif packet.command == PacketCommand.MOTOR_AZ_POSITION:
            self.state.updateAzPosition(packet)
        elif packet.command == PacketCommand.MOTOR_AZ_DELTA_POS:
            self.state.updateAzDelta(packet)
        elif packet.command == PacketCommand.MOTOR_AZ_STATE:
            self.state.updateAzState(packet)
        elif packet.command == PacketCommand.LNB_STATE:
            self.state.updateLNBVoltage(packet)
            self.state.updateLNBCarrier(packet)