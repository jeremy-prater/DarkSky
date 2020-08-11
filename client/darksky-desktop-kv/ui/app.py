import coloredlogs, logging

from ui.darksky import DarkSky
from kivy.app import App
from darksky_socket import SocketClient

from ui.skyview import SkyView

class DarkSkyApp(App):
    def build(self):
        self.logger = logging.getLogger(__name__)
        coloredlogs.install(level='DEBUG', logger=self.logger)
        self.logger.info("build")
        self.context = DarkSky()
        self.context.backend = SocketClient()
        return self.context

    def on_start(self):
        self.logger.info("on_start")
        self.context.backend.doConnect()

    def on_stop(self):
        self.logger.info("on_stop")
        self.context.backend.doDisconnect()

    def on_pause(self):
        self.logger.info("on_pause")

    def on_resume(self):
        self.logger.info("on_resume")
