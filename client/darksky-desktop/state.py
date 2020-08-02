from PySide2.QtCore import QObject, Slot, Signal, Property


class State(QObject):
    textChanged = Signal(str)

    def __init__(self, parent=None):
        QObject.__init__(self, parent)
        self.m_text = "I'm a test"

    @Property(str, notify=textChanged)
    def text(self):
        return self.m_text