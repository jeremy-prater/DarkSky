import serial
import serial.tools.list_ports
from packet import PacketCommand, Packet
import logging
import json
import backend_socketio
import threading
import struct


class MotorPowerControllerReader:
    def __init__(self, serial):
        self.logger = logging.getLogger(__name__)
        self.logger.info(
            "Creating DarkSky Motor Power Controller Serial Reader")
        self.serial = serial
        self.reading = False
        self.readerThread = threading.Thread(
            target=self.ReaderThread, args=(self,), daemon=True)
        self.readerThread.start()

    @staticmethod
    def binaryToMotorState(value):
        if value == 0:
            return "stop"
        elif value == 1:
            return "forward"
        elif value == 2:
            return "reverse"
        elif value == 3:
            return "stall"
        else:
            return "unknown"

    @staticmethod
    def binaryToLNBState(value):
        if value == 0:
            return ("off", "off")
        elif value == 1:
            return ("13vdc", "off")
        elif value == 2:
            return ("13vdc", "on")
        elif value == 3:
            return ("18vdc", "on")
        elif value == 4:
            return ("18vdc", "off")
        else:
            return ("unknown", "unknown")

    @staticmethod
    def ReaderThread(context):
        context.logger.info(
            "MPC-Reader : Starting read loop on {}".format(context.serial.port))
        context.reading = True
        mcpSocketIO = backend_socketio.SocketIOBackend.getInstance()            

        context.incomingData = []

        while (context.reading):
            data = context.serial.read()
            context.incomingData.append(data)
            while len(context.incomingData) >= 4:
                header = struct.unpack('I', b''.join(context.incomingData[:4]))
                if header[0] != 0xDADAF00D:
                    context.incomingData.pop(0)
                else:
                    if len(context.incomingData) >= 16:
                        packet = Packet.CreateFromRawData(
                            context.incomingData[:16])
                        if packet != None:
                            del context.incomingData[:16]

                            if packet.command == PacketCommand.BOOT:
                                mcpSocketIO.SendPacket('signal.boot', True)
                            elif packet.command == PacketCommand.MOTOR_DEC_POSITION:
                                mcpSocketIO.SendPacket(
                                    'signal.motor.dec.position', packet.arg1)
                            elif packet.command == PacketCommand.MOTOR_DEC_STATE:
                                mcpSocketIO.SendPacket(
                                    'signal.motor.dec.state', MotorPowerControllerReader.binaryToMotorState(packet.arg1))
                            elif packet.command == PacketCommand.MOTOR_RA_POSITION:
                                mcpSocketIO.SendPacket(
                                    'signal.motor.ra.position', packet.arg1)
                            elif packet.command == PacketCommand.MOTOR_RA_STATE:
                                mcpSocketIO.SendPacket(
                                    'signal.motor.ra.state', MotorPowerControllerReader.binaryToMotorState(packet.arg1))
                            elif packet.command == PacketCommand.LNB_STATE:
                                mcpSocketIO.SendPacket(
                                    'signal.lnb.power', MotorPowerControllerReader.binaryToLNBState(packet.arg1))
                        else:
                            context.incomingData.pop(0)
                    else:
                        break

        context.logger.info("MPC-Reader : Exited...")
