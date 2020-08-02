import sys
from PySide2.QtWidgets import QApplication
from PySide2.QtWidgets import QApplication
from PySide2.QtCore import Qt, QCoreApplication
from PySide2.QtQml import QQmlApplicationEngine, QQmlDebuggingEnabler

import sys
from os.path import abspath, dirname, join

from PySide2.QtCore import QObject, Slot
from PySide2.QtGui import QGuiApplication
from PySide2.QtQml import QQmlApplicationEngine, QQmlComponent
from PySide2.QtQuickControls2 import QQuickStyle

from state import State

# if __name__ == '__main__':
#     QApplication.setAttribute(Qt.AA_EnableHighDpiScaling)

#     QCoreApplication.setAttribute(Qt.AA_UseHighDpiPixmaps)

#     sys.argv += ['--style', 'Material']
#     app = QApplication(sys.argv)

#     engine = QQmlApplicationEngine('main.qml')

#     sys.exit(app.exec_())

if __name__ == '__main__':
    QQmlDebuggingEnabler()
    app = QGuiApplication(sys.argv)
    QQuickStyle.setStyle("Material")
    engine = QQmlApplicationEngine()

    # Expose the Python object to QML
    state = State()

    context = engine.rootContext()
    context.setContextProperty("DarkSkyState", state)

    # Get the path of the current directory, and then add the name
    # of the QML file, to load it.
    qmlFile = join(dirname(__file__), 'main.qml')
    engine.load(abspath(qmlFile))

    if not engine.rootObjects():
        sys.exit(-1)

    sys.exit(app.exec_())