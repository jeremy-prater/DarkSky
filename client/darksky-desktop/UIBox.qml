import QtQuick 2.12

Rectangle {
    default property string title : "Unnamed Window"
    required property Component uibody

    color: "#808080"
    radius: 5
    antialiasing: true


    Rectangle {
        color: "#8080C0"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: parent.height * 0.1
        radius: 5
        antialiasing: true

        MouseArea {
            default property bool dragging : false

            property int oldX
            property int oldY

            anchors.fill: parent
            hoverEnabled: true

            onPressed: {
                mouse.accepted = true;
                dragging = true;
                oldX = mouse.x;
                oldY = mouse.y;
            }
            onReleased: {
                mouse.accepted = true;
                dragging = false;
            }
            onPositionChanged: {
                mouse.accepted = true;
                if (dragging == true)
                {
                    // var dX =  mouse.x - oldX;
                    // var dY = mouse.y - oldY;
                    // oldX = mouse.x;
                    // oldY = mouse.y;

                    // console.log(`Mouse abs ${mouse.x}x${mouse.y} old ${oldX}x${oldY}`);
                    // console.log(`Moving window ${title} by ${dX}x${dY}`);
                    // parent.parent.x += dX
                    // parent.parent.y += dY
                }
            }
        }

        Text {
            anchors.fill: parent
            anchors.margins: 5
            text: title
            antialiasing: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.family: "Monospace"
        }
    }

    Loader {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: parent.height * 0.9

        sourceComponent: uibody
    }
}
