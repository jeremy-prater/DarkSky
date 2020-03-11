import serial
import serial.tools.list_ports
from packet import PacketCommand, Packet
import logging
import json
import backend_socketio


class MotorPowerControllerReader:
    def __init__(self, serial):
        self.logger = logging.getLogger(__name__)
        self.logger.info("Creating DarkSky Motor Power Controller Serial Reader")
        self.serial = serial;
        