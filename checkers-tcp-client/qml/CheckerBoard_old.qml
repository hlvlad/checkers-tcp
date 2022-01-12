import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import game 1.0
import enums 1.0
import checkersmodel 1.0

Rectangle {
    color: "red"

    Grid {
        id: board
        anchors.centerIn: parent
        columns: 8
        rows: 8
        Repeater {
            id: repeater
            model: 64
            Rectangle {
                id: board_cell
                width: spot_size
                height: spot_size
                color: ((index / 8) + index) & 1 ? "black" : "white"
            }
        }
    }
}

