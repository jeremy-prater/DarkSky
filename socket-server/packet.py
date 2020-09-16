import struct
from enum import IntEnum
import hashlib


class PacketCommand(IntEnum):
    BOOT = 0x0000,
    MOTOR_DEC_STATE = 0x0101,
    MOTOR_DEC_POSITION = 0x0102,
    MOTOR_DEC_DELTA_POS = 0x0103,
    MOTOR_RA_STATE = 0x0201,
    MOTOR_RA_POSITION = 0x0202,
    MOTOR_RA_DELTA_POS = 0x0203,
    LNB_STATE = 0x0301,
    STOP_ALL_MOTORS = 0x1001,
    ERROR = 0x2000,
    REBOOT = 0xFFFF


class Packet:
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
    def binaryToLNBVoltage(value):
        if value == 0:
            return "off"
        elif value == 1:
            return "13vdc"
        elif value == 2:
            return "18vdc"
        else:
            return "unknown"

    @staticmethod
    def binaryToLNBCarrier(value):
        if value == 0:
            return "off"
        elif value == 1:
            return "on"
        else:
            return "unknown"

    def GetRawBuffer(self):
        return struct.pack('IHHHHI', self.magic, self.command, self.arg1, self.arg2, self.arg3, self.checksum)

    def IsValid(self):
        return self.checksum == self.GenerateChecksum()

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
