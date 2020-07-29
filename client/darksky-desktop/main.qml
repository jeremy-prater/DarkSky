import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

ApplicationWindow {
    id: darksky
    width: 1280
    height: 720
    // minimumWidth: 1180
    // minimumHeight: 663
    visible: true
    title: "DarkSky Desktop"

    Material.theme: Material.Dark
    Material.accent: Material.BlueGrey

    Rectangle {
        anchors.fill: parent
        color: "#404040"
    }

    Column {
        anchors.centerIn: parent

        RadioButton {
            text: qsTr("Small")
        }
        RadioButton {
            text: qsTr("Medium")
            checked: true
        }
        RadioButton {
            text: qsTr("Large")
        }
    }

    jbox {
        x: 500
        y: 100
        width: 200
        height: 100
        visible: true
    }
}
