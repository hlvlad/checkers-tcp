import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: menu_item
    property string serverAddress: "localhost"
    property string serverPort: "3000"

    signal createLobby
    signal startGame
    signal stopGame
    signal connectToLobby(int lobby_id)

    SettingsDialog {
        id: settings_dialog

        Component.onCompleted: {
            serverAddress = menu_item.serverAddress;
            serverPort = menu_item.serverPort;
        }

        onAccepted: {
            menu_item.serverAddress = serverAddress;
            menu_item.serverPort = serverPort;
        }
    }

    Rectangle {
        color: "#38A295"
        anchors.fill: parent

        ColumnLayout {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.topMargin: 20

            MenuButton {
                id: create_lobby_button
                text: "Create lobby"
                onClicked: {
                    createLobby()
                }
                Layout.alignment: Qt.AlignHCenter
            }
            TextField {
                id: lobbyid_field
                placeholderText: "Enter lobby id"
                Layout.alignment: Qt.AlignHCenter
            }
            MenuButton {
                id: connect_button
                text: "Connect to lobby"
                onClicked: {
                    let lobby_id = parseInt(lobbyid_field.text)
                    connectToLobby(lobby_id)
                }
                Layout.alignment: Qt.AlignHCenter
            }
            MenuButton {
                id: settings_button
                text: "Settings"
                onClicked: {
                    settings_dialog.open()
                }
                Layout.alignment: Qt.AlignHCenter
            }
        }
        Row {
            anchors.leftMargin: 20
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
            Label {
                id: connection_status
                color: "white"
                text: "Connection status: "
            }
            Text {
                color: "white"
                text: "Disconnected"
            }
        }
    }

}
