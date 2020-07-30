from PySide2.QtCore import QObject, Slot


class State(QObject):    
    @Slot(result=str)
    def test1(self):
        return "data"