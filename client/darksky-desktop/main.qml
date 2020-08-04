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
    Material.accent: Material.Purple

    Component.onCompleted: {
        console.log("Init...");
        SocketClient.doConnect();
    }

    
    onClosing: {
        console.log("Shutdown starting ...");
        SocketClient.doDisconnect();
    }

    Rectangle {
        anchors.fill: parent
        color: "#404040"

        MouseArea {
            anchors.fill: parent

            onPressed: {
                console.log("yoyo-ing duke...");
                DarkSkyState.updateText("yoyo duke");
            }
        }
    }

    UIBox {
        title: "Status"
        width: 200
        height: 300
        visible: true
        anchors.margins : 10
        x: parent.width - 220
        y: parent.height - 320
        
        uibody : 
            Column {

                anchors.centerIn: parent

                RadioButton {
                    text: DarkSkyState.text
                    onPressed : {
                        console.log("You clicked it")
                        console.log(DarkSkyState.text)   
                    }
                }
                RadioButton {
                    text: "Medium"
                    checked: true
                }
                RadioButton {
                    text: "Large"
                }
            }
    }
}
