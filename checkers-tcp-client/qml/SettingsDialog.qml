import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs


Dialog {
    id: settings_dialog
    title: "Settings"
    property alias serverAddress : server_address.text
    property alias serverPort : server_port.text

    visible: false
    standardButtons: Dialog.Save | Dialog.Cancel

    Rectangle {
        implicitWidth: 400
        implicitHeight: 200

        GridLayout {
            columns: 2
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 20
            Label { text: "Server address:" }
            TextField {
                id: server_address
                placeholderText: "Enter server IP (v4 or v6)"
                Layout.fillWidth: true
            }
            Label { text: "Server port:" }
            TextField {
                id: server_port
                validator: IntValidator {
                    bottom: 1
                    top: 65535
                }
                placeholderText: "Enter server port"
                Layout.fillWidth: true
            }
        }

    }

}


