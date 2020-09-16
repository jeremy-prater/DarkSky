import logging
import coloredlogs
import eventlet
import argparse
import sys

from backend_socketio import SocketIOBackend
from mcp_serial import MotorPowerController
from gps import GPS
from state import State


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='DarkSky socket server')
    parser.add_argument('-s',
                 '--simulate',
                 action='store_true',
                 help='Simulate the dish position')
    args = parser.parse_args()

    eventlet.monkey_patch()

    coloredlogs.install(level='DEBUG', milliseconds=True)

    logger = logging.getLogger(__name__)

    mpc = MotorPowerController()
    server = SocketIOBackend()
    gps = GPS()

    if args.simulate:
        logger.info('Starting simulation threads')
        mpc.ConnectSimulator()
        State().StartSimulation()
    else:
        ports = mpc.GetPorts()
        mpc.Connect(ports[0])

    server.Listen()