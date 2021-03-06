import struct
from enum import IntEnum
import hashlib


class PacketCommand(IntEnum):
    BOOT = 0x0000,
    MOTOR_ALT_STATE = 0x0101,
    MOTOR_ALT_POSITION = 0x0102,
    MOTOR_ALT_DELTA_POS = 0x0103,
    MOTOR_ALT_PWM = 0x0104,
    MOTOR_AZ_STATE = 0x0201,
    MOTOR_AZ_POSITION = 0x0202,
    MOTOR_AZ_DELTA_POS = 0x0203,
    MOTOR_AZ_PWM = 0x0204,
    MOTOR_AZ_AUTODRIVE = 0x0205,
    LNB_STATE = 0x0301,
    STOP_ALL_MOTORS = 0x1001,
    ERROR = 0x2000,
    REBOOT = 0xFFFF


class Packet:
    Magic = 0xDADAF00D

    def __init__(self, magic: int, command: PacketCommand, arg1: int, arg2: int, arg3: int, checksum: int):
        self.magic = magic
        self.command = command
        self.arg1 = arg1
        self.arg2 = arg2
        self.arg3 = arg3
        self.checksum = checksum

    @staticmethod
    def CreateFromRawData(data):
        if len(data) != 16:
            return None
        packetStruct = struct.unpack('IHHHHI', b''.join(data))
        return Packet(packetStruct[0],
                      packetStruct[1],
                      packetStruct[2],
                      packetStruct[3],
                      packetStruct[4],
                      packetStruct[5])

    @staticmethod
    def CreateFromStruct(command: PacketCommand, arg1: int, arg2: int, arg3: int):
        return Packet(Packet.Magic, command, arg1, arg2, arg3, 0)

    @staticmethod
    def BinaryToMotorState(value):
        if value == 0:
            return "stop"
        elif value == 1:
            return "forward"
        elif value == 2:
            return "reverse"
        elif value == 3:
            return "complete"
        elif value == 4:
            return "stall"
        else:
            return "unknown"

    @staticmethod
    def MotorStateToBinary(value):
        if value == "stop":
            return 0
        elif value == "forward":
            return 1
        elif value == "reverse":
            return 2
        elif value == "complete":
            return 3
        elif value == "stall":
            return 4
        else:
            return 5

    @staticmethod
    def BinaryToLNBVoltage(value):
        if value == 0:
            return "off"
        elif value == 1:
            return "13vdc"
        elif value == 2:
            return "18vdc"
        else:
            return "unknown"

    @staticmethod
    def LNBVoltageToBinary(value):
        if value == "off":
            return 0
        elif value == "13vdc":
            return 1
        elif value == "18vdc":
            return 2
        else:
            return 3

    @staticmethod
    def BinaryToLNBCarrier(value):
        if value == 0:
            return "off"
        elif value == 1:
            return "on"
        else:
            return "unknown"

    @staticmethod
    def LNBCarrierToBinary(value):
        if value == "off":
            return 0
        elif value == "on":
            return 1
        else:
            return 2

    def GetRawBuffer(self):
        return struct.pack('IHHHHI', self.magic, self.command, self.arg1, self.arg2, self.arg3, self.checksum)

    def IsValid(self):
        return self.checksum == self.GenerateChecksum()

    def IsEqual(self, packet):
        return (self.checksum == packet.checksum) and (self.magic == packet.magic) and (self.command == packet.command) and (self.arg1 == packet.arg1) and (self.arg2 == packet.arg2) and (self.arg3 == packet.arg3)

    def SetChecksum(self):
        self.checksum = self.GenerateChecksum()

    def GenerateChecksum(self):
        return hashlib.md5(struct.pack(
            'IHHHH', self.magic, self.command, self.arg1, self.arg2, self.arg3)).digest()

    def GetPayload(self):
        print(self.command)
        print(self.command.name)
        return {
            "command": self.command,
            "arg1": self.arg1,
            "arg2": self.arg2,
            "arg3": self.arg3,
            "checksum": self.checksum
        }
