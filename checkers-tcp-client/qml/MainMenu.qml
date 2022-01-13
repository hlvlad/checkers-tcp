import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import network 1.0

Item {
    id: menu_item
    property string serverAddress: "localhost"
    property string serverPort: "3000"
    property bool isGameRunning: false

    signal createLobby
    signal connectToLobby(int lobby_id)
    signal resign

    SettingsDialog {
        id: settings_dialog

        Component.onCompleted: {
            serverAddress = menu_item.serverAddress;
            serverPort = menu_item.serverPort;
        }

        onAccepted: {
            menu_item.serverAddress = serverAddress;
            menu_item.serverPort = serverPort;
            NetworkSession.set_server_address(menu_item.serverAddress, menu_item.serverPort)
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
        MenuButton {
            id: resign_button
            text: "Resign"
            enabled: menu_item.isGameRunning
            onClicked: resign()

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 20
        }
    }

}
