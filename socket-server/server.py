import logging
import coloredlogs

from backend_socketio import SocketIOBackend
from mcp_serial import MotorPowerController
from gps import GPS
from state import State

import eventlet

if __name__ == "__main__":
    eventlet.monkey_patch()

    coloredlogs.install(level='DEBUG', milliseconds=True)

    mpc = MotorPowerController()
    server = SocketIOBackend()
    gps = GPS()

    server.Listen()

    mpc.ConnectSimulator()
