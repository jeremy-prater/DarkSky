import serial
import serial.tools.list_ports
from packet import PacketCommand, Packet
import logging
import json
import backend_socketio
import threading


class MotorPowerControllerReader:
    def __init__(self, serial):
        self.logger = logging.getLogger(__name__)
        self.logger.info("Creating DarkSky Motor Power Controller Serial Reader")
        self.serial = serial;
        self.reading = False
        self.readerThread = threading.Thread(target=self.ReaderThread, args=(self,), daemon=True)
        self.readerThread.start()

    @staticmethod
    def ReaderThread(context):
        context.logger.info("MPC-Reader : Starting read loop on {}".format(context.serial.port))
        context.reading = True
        while (context.reading):
            data = context.serial.read()
            print(data)
        context.logger.info("MPC-Reader : Exited...")
        