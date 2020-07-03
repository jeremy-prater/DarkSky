import logging

class State:
    instance = None

    @staticmethod
    def getInstance():
        if State.instance == None:
            State.instance = State()
        return State.instance

    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.logger.info('Init')

    def updateGPS(self, gpsFix):
        self.logger.info(gpsFix)
        # self.logger.info('GPS Update {}.{} @ {}'.format(gpsFix.lat, gpsFix.lon, gpsFix.time))