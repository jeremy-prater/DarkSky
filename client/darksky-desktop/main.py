import sys
import random
import time
from PySide2.QtQuick import QQuickView
from PySide2.QtCore import QUrl
from PySide2.QtWidgets import QApplication, QLabel
                                                     
if __name__ == "__main__":
    app = QApplication([])
    view = QQuickView()
    url = QUrl("main.qml")

    view.setSource(url)
    view.setResizeMode(QQuickView.SizeRootObjectToView)
    view.show()
    sys.exit(app.exec_())