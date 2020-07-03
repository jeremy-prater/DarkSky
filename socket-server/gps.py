import logging
import gpsd
import threading
import time
from state import State
import datetime


class GPS:
    instance = None

    @staticmethod
    def getInstance():
        if GPS.instance == None:
            GPS.instance = GPS()
        return GPS.instance

    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.logger.info('Init')

        # Connect to the local gpsd
        try:
            gpsd.connect()
            self.gpsThread = threading.Thread(
                target=self.GPSThread, args=(self,), daemon=True)
        except ConnectionRefusedError as err:
            self.logger.warn(
                'Unable to connect to gpsd... Running in simulator mode {}'.format(err))
            self.gpsThread = threading.Thread(
                target=self.GPSSimulatorThread, args=(self,), daemon=True)

        self.gpsThread.start()

    @staticmethod
    def GPSSimulatorThread(context):
        state = State.getInstance()
        context.logger.info(
            'Starting GPS Simulator loop')
        context.reading = True
        while (context.reading):
            state.updateGPS({
                'mode': 3,
                'sats': 11,
                'lat': 45.963637,
                'lon': -122.416802,
                'time': datetime.datetime.utcnow().isoformat(),
                'error': "",
                'alt': 0,
                'climb': 0
            })
            time.sleep(1)

    @staticmethod
    def GPSThread(context):
        state = State.getInstance()
        context.logger.info(
            'Starting GPS Simulator loop')
        context.reading = True
        while (context.reading):
            # Get gps position
            packet = gpsd.get_current()

            # Update gps state
            state.updateGPS(packet)

            # See the inline docs for GpsResponse for the available data
            print(packet.position())
