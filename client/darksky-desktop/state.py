from PySide2.QtCore import QObject, Slot, Signal, Property
from singleton import Singleton
import logging

class State(QObject):
    textChanged = Signal(str)
    intext="mcguffin"

    @Property(str, notify=textChanged)
    def text(self):
        return self.intext

    @Slot(str)
    def updateText(self, newText):
        print("yoyo engaging...")
        self.intext = newText
        self.textChanged.emit(self.intext)