from PySide2.QtCore import QObject, Slot, Signal, Property


class State(QObject):
    intext="mcguffin"
    textChanged = Signal(str)

    @Property(str, notify=textChanged)
    def text(self):
        return self.intext

    @Slot(str)
    def updateText(self, newText):
        print("yoyo engaging...")
        self.intext = newText
        self.textChanged.emit(self.intext)