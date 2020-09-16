import serial
import serial.tools.list_ports
from packet import PacketCommand, Packet
import logging
import json
import backend_socketio
import threading
import struct
from state import State


class MotorPowerControllerReader:
    def __init__(self, serial):
        self.logger = logging.getLogger(__name__)
        self.logger.info(
            "Creating DarkSky Motor Power Controller Serial Reader")
        self.serial = serial
        self.state = State()
        self.reading = False
        self.readerThread = threading.Thread(
            target=self.ReaderThread, args=(self,), daemon=True)
        self.readerThread.start()

    @staticmethod
    def ReaderThread(context):
        context.logger.info(
            "MPC-Reader : Starting read loop on {}".format(context.serial.port))
        context.reading = True
        mcpSocketIO = backend_socketio.SocketIOBackend()

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

                            # We have a packet!
                            if packet.command == PacketCommand.BOOT:
                                mcpSocketIO.SendPacket('signal.boot', True)

                            elif packet.command == PacketCommand.MOTOR_DEC_POSITION:
                                context.state.updateMotorPosition('dec', packet)
                            elif packet.command == PacketCommand.MOTOR_DEC_DELTA_POS:
                                context.state.updateMotorDelta('dec', packet)
                            elif packet.command == PacketCommand.MOTOR_DEC_STATE:
                                context.state.updateMotorState('dec', packet)

                            elif packet.command == PacketCommand.MOTOR_RA_POSITION:
                                context.state.updateMotorPosition('ra', packet)
                            elif packet.command == PacketCommand.MOTOR_RA_DELTA_POS:
                                context.state.updateMotorDelta('ra', packet)
                            elif packet.command == PacketCommand.MOTOR_RA_STATE:
                                context.state.updateMotorState('ra', packet)

                            elif packet.command == PacketCommand.LNB_STATE:
                                context.state.updateLNBVoltage(packet)
                                context.state.updateLNBCarrier(packet)

                        else:
                            context.incomingData.pop(0)
                    else:
                        break

        context.logger.info("MPC-Reader : Exited...")
