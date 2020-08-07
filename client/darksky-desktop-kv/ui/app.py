import coloredlogs, logging

from ui.main import DarkSky
from kivy.app import App

class DarkSkyApp(App):
    def build(self):
        self.logger = logging.getLogger(__name__)
        coloredlogs.install(level='DEBUG', logger=self.logger)
        self.logger.info("DarkSkyApp: build")
        return DarkSky()

    def on_start(self):
        self.logger.info("DarkSkyApp: on_start")

    def on_stop(self):
        self.logger.info("DarkSkyApp: on_stop")

    def on_pause(self):
        self.logger.info("DarkSkyApp: on_pause")

    def on_resume(self):
        self.logger.info("DarkSkyApp: on_resume")
