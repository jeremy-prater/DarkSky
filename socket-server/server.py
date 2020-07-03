import logging
import coloredlogs

from backend_socketio import SocketIOBackend
from mcp_serial import MotorPowerController
from gps import GPS
from state import State

import eventlet
eventlet.monkey_patch()

coloredlogs.install(level='DEBUG', milliseconds=True)

mpc = MotorPowerController.getInstance()
server = SocketIOBackend.getInstance()
gps = GPS.getInstance()
state = State.getInstance()

server.Listen()