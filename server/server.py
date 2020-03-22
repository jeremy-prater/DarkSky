import logging
import coloredlogs

from backend_socketio import SocketIOBackend
from mcp_serial import MotorPowerController

import eventlet
eventlet.monkey_patch()

coloredlogs.install(level='DEBUG', milliseconds=True)

mpc = MotorPowerController.getInstance()
server = SocketIOBackend.getInstance()

server.Listen()