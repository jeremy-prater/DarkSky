import serial
import serial.tools.list_ports
from packet import PacketCommand, Packet
import logging


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
        for port in serial.tools.list_ports.comports():
            print(port)

    #def SendPacket(packet: Packet):
        
