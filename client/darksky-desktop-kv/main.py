from kivy.app import App
from kivy.uix.widget import Widget


class DarkSky(Widget):
    pass


class DarkSkyApp(App):
    def build(self):
        return DarkSky()


if __name__ == '__main__':
    DarkSkyApp().run()