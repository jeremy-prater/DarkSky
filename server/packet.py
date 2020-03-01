import struct
from enum import IntEnum

class PacketCommand(IntEnum):
    SIGNAL_BOOT = 0x0000
    SIGNAL_MOTOR_DEC_STATE = 0x0101
    SIGNAL_MOTOR_DEC_POSITION = 0x0102
    SIGNAL_MOTOR_RA_STATE = 0x0201
    SIGNAL_MOTOR_RA_POSITION = 0x0202
    SIGNAL_LNB_POWER_STATE = 0x0301
    COMMAND_REBOOT = 0x1000
    COMMAND_STOP_ALL_MOTORS = 0x1001
    COMMAND_MOTOR_DEC_SET_STATE = 0x1101
    COMMAND_MOTOR_DEC_SET_POSITION = 0x1102
    COMMAND_MOTOR_RA_SET_STATE = 0x1201
    COMMAND_MOTOR_RA_SET_POSITION = 0x1202
    COMMAND_LNB_SET_POWER_STATE = 0x1301
    ERROR = 0x2000

class Packet:
    def __init__(self, command: PacketCommand, arg1: int, arg2: int, arg3: int):
        self.command = command
        self.arg1 = arg1
        self.arg2 = arg2
        self.arg3 = arg3

    def GetRawBuffer(self):
        return struct.pack('IHHHHI', 0xDADA, self.command, self.arg1, self.arg2, self.arg3, 0x0000)

    def GetPayload(self):
        print (self.command)
        print (self.command.name)
        return {
            "command": self.command,
            "arg1": self.arg1,
            "arg2": self.arg2,
            "arg3": self.arg3
        }

