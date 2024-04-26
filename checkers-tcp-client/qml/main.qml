import QtQuick
import QtQuick.Window
import QtQuick.Controls 
import QtQuick.Layouts

import network 1.0


Window {
    id: main_window
    width: 1280
    height: 720
    visible: true
    title: qsTr("Qt Checkers TCP")


    MainMenu {
        id: game_menu
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: 300

        onCreateLobby: NetworkSession.create_lobby()
        onConnectToLobby: stack.push(connecting_view, {"lobby_id": lobby_id})
        onResign: {
            NetworkSession.resign();
            stack.pop(stack.initialItem)
        }
    }

    StackView {
        id: stack
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.left: game_menu.right
        initialItem: main_view

        pushEnter: Transition { PropertyAnimation { property: "opacity"; from: 0; to: 1; duration: 200 } }
        pushExit: Transition { PropertyAnimation { property: "opacity"; from: 1; to:0; duration: 200 } }
        popEnter: Transition { PropertyAnimation { property: "opacity"; from: 0; to: 1; duration: 200 } }
        popExit: Transition { PropertyAnimation { property: "opacity"; from: 1; to: 0; duration: 200 } }
    }

    Item {
        id: main_view
        Text {
            anchors.centerIn: parent
            text: "Create new lobby or connect to existing lobby."
            font.pointSize: 24
            color: "gray"
        }
    }

    Component {
        id: lobby_view
        CreatedLobbyPane { }
    }

    Component {
        id: connecting_view
        ConnectingPane {
          Component.onCompleted: NetworkSession.connect_lobby(lobby_id)
        }
    }

    Component {
        id: game_view
        GamePane { }
    }

    function showError(error_message) {
        message_dialog.type = "error"
        message_dialog.text = error_message
        message_dialog.open()
    }

    function showNotification(info_message) {
        message_dialog.type = "info"
        message_dialog.text = info_message
        message_dialog.open()
    }

    Connections {
        target: NetworkSession

        function onServerErrorOccurred(error_message) {
            showError(error_message)
            stack.pop(stack.initialItem)
        }

        function onLobbyCreated(lobby_id) {
            stack.push(lobby_view, {"lobby_id": lobby_id});
        }

        function onGameStarted(is_white) {
            stack.push(game_view, {"isWhiteSide": is_white});
            game_menu.isGameRunning = true;
        }

        function onResignReceived() {
            showNotification("Opponent resigned.")
            stack.pop(stack.initialItem)
        }
    }

    NotificationDialog {
        id: message_dialog
    }
}
