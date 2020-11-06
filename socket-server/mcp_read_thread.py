import serial
import serial.tools.list_ports
from packet import PacketCommand, Packet
import logging
import json
# import backend_socketio
import threading
import struct
from state import State


class MotorPowerControllerReader:
    def __init__(self, mpc):
        self.logger = logging.getLogger(__name__)
        self.logger.info(
            "Creating DarkSky Motor Power Controller Serial Reader")
        self.mpc = mpc
        self.serial = self.mpc.serial
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
        # mcpSocketIO = backend_socketio.SocketIOBackend()

        context.incomingData = []

        while (context.reading):
            try:
                data = context.serial.read()
            except serial.serialutil.SerialException as err:
                context.logger.error("Serial Error : {}".format(err))
                break

            context.incomingData.append(data)
            while len(context.incomingData) >= 4:
                header = struct.unpack('I', b''.join(context.incomingData[:4]))
                if header[0] != Packet.Magic:
                    context.incomingData.pop(0)
                else:
                    if len(context.incomingData) >= 16:
                        packet = Packet.CreateFromRawData(
                            context.incomingData[:16])
                        if packet != None:
                            del context.incomingData[:16]
                            # context.logger.info("MPC-Reader : <<- packet in : {}".format(packet.command))
                            context.mpc.processReceivedPacket(packet)
                        else:
                            context.incomingData.pop(0)
                    else:
                        break

        context.logger.info("MPC-Reader : Exited...")
