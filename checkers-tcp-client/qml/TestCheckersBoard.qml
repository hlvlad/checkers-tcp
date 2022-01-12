import QtQuick 2.15
import QtQuick.Controls 2.15
import QtGraphicalEffects 1.15

import checkersmodel 1.0

Rectangle {
    border.color: "saddlebrown"
    border.width: 10
    id: root
    width: 1000; height: 1000
    property int cellSize: width/8
    readonly property point cellCenter: Qt.point(cellSize/2, cellSize/2)
    property int newCellIndex: 1

    Grid {
        rows: 8
        columns: 8
        rotation: 180

        Repeater {
            id: repeater
            model: 64

        Rectangle {
            id: cell
            rotation: 180
            width: root.cellSize
            height: root.cellSize
            color: ((index / 8) + index) & 1 ? "black" : "white"

            Item {
                visible: index < 32
                id: piece
                width: root.cellSize * 0.8
                height: width
                x: parent.width/2
                y: parent.height/2
                Image {
                    id: piece_image
                    source: "/images/manb.png"
                    anchors.fill: parent
                }
                ColorOverlay {
                    id: overlay
                    visible: piece_mouse_area.containsMouse
                    anchors.fill: piece_image
                    source: piece_image
                    color: "#80000080"
                }
                transform: Translate {
                    x: -piece.width/2
                    y: -piece.height/2
                }
                states: State {
                    name: "moving"
                    ParentChange { target: piece; parent: repeater.itemAt(root.newCellIndex); x: cellSize/2; y: cellSize/2}
                }

                transitions: Transition {
                    ParentAnimation {
                        via: root
                        NumberAnimation { properties: "x,y"; duration: 400 }
                    }
                }

                MouseArea {
                    id: piece_mouse_area
                    anchors.fill: parent;
                    hoverEnabled: true
                    onClicked: {
                        piece.state = "moving"
                    }
                }

            }

        }
        }
    }
}
