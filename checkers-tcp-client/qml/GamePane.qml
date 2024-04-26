import QtQuick
import QtQuick.Controls

import network 1.0


Item {
    id: game_pane
    property int spot_size: 60
    property int game_flags: 0
    property alias isWhiteSide: board.isWhiteSide

    Label {
        id: status_label
        text: ""
        font.pointSize: 14
        anchors.horizontalCenter: board.horizontalCenter
        anchors.bottom: board.top
        anchors.bottomMargin: 70
    }

    CheckerBoard {
        id: board
        anchors.centerIn: parent
        width: 8 * parent.spot_size + 6
        height: width
    }
}
