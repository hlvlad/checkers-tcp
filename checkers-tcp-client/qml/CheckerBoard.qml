import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

import checkersmodel 1.0
import network 1.0

Rectangle {
    id: root
    border.color: "saddlebrown"
    border.width: 10
    width: 500; height: 500
    property int cellSize: width/8
    readonly property point cellCenter: Qt.point(cellSize/2, cellSize/2)
    property int fromSquareIndex: -1
    property int toSquareIndex: -1
    property bool isWhiteSide: true

    CheckersModel {
        id: checkers_model
    }

    Connections {
        target: NetworkSession

        function onMoveReceived(from, to, type) {
            checkers_model.make_move(from, to, type)
        }
    }

    Grid {
        id: board
        anchors.centerIn: parent
        columns: 8
        rows: 8
        rotation: (root.isWhiteSide) ? 0 : 180
        Repeater {
            id: repeater

            model: checkers_model
            Rectangle {
                id: cell
                rotation: (root.isWhiteSide) ? 0 : 180
                width: root.cellSize
                height: root.cellSize
                color: model.is_spot ? "black" : "white";
                MouseArea {
                    id: square_mouse_area
                    anchors.fill: parent;
                    hoverEnabled: true
                    onClicked: {
                        if (model.has_moves) {
                            console.log(`SELECTED FROM spot: ${model.spot_number}`);
                            root.fromSquareIndex = model.spot_number;
                        } else if (model.square_state === CheckersModel.PossibleMove) {
                            console.log(`SELECTED TO spot: ${model.spot_number}`);
                            root.toSquareIndex = model.spot_number;
                            console.log(`MOVE_REQUEST from ${root.fromSquareIndex} to ${root.toSquareIndex}`);
                            const move_type = checkers_model.make_move(root.fromSquareIndex, root.toSquareIndex);
                            // if move type is not INVALID
                            if (move_type !== 4) NetworkSession.send_move(root.fromSquareIndex, root.toSquareIndex, move_type);
                            root.fromSquareIndex = -1;
                        }
                    }
                }
//                Text {
//                    visible: model.is_spot
//                    text : visible ? model.spot_number : ""
//                    color: "red"
//                }
                Rectangle {
                    id: square_overlay
                    anchors.fill: cell
                    color: "#800000FF"
                    visible: false
                }
                states: [
//                    State {
//                        name: "moving";
//                        ParentChange { target: piece; parent: repeater.itemAt(root.toSquareIndex); x: cellSize/2; y: cellSize/2}
//                    },
                    State {
                        name: "selected";
                        when: model.is_spot && root.fromSquareIndex === model.spot_number
//                            (piece_mouse_area.containsMouse || root.fromSquareIndex === model.spot_number)
                        PropertyChanges { target: piece_overlay; visible: true }
                        StateChangeScript {
                            name: "selectPiece";
                            script: {
                                console.log(`SELECTED spot ${model.spot_number}`);
                                model.square_state = CheckersModel.Selected
                            }
                        }
                    },
                    State {
                        name: "";
                        StateChangeScript {
                            name: "resetPiece";
                            script: {
                                console.log(`RESET square: ${model.square_number}, spot: ${model.is_spot ? model.spot_number : "-"}`);

//                                if (root.fromSquareIndex !== index) {
//                                    console.log(`Setting piece ${model.spot_number} to default.`);
//                                    model.square_state = CheckersModel.Default
//                                }
                            }
                        }
                    },

                    State {
                        name: "captured"; when: model.square_state === CheckersModel.Captured
                        PropertyChanges { target: piece_overlay; visible: true; color: "#0000008080" }
                    },
                    State {
                        name: "white"; when: !model.is_spot
                    },
                    State {
                        name: "possible_move";
                        when: model.is_spot && model.square_state === CheckersModel.PossibleMove
                        PropertyChanges { target: square_overlay; visible: true }
                        StateChangeScript {
                            name: "possibleMove";
                            script: {
                                console.log(`POSSIBLE_MOVE: spot: ${model.spot_number}`);
                            }
                        }
                    }
                ]

                Item {
                    id: piece
                    visible: model.is_spot && model.is_occupied
                    width: root.cellSize * 0.8
                    height: width
                    x: parent.width/2
                    y: parent.height/2

                    transform: Translate {
                        x: -piece.width/2
                        y: -piece.height/2
                    }

                    Image {
                        id: piece_image
                        anchors.fill: parent
                        onVisibleChanged: if (visible) source = getPictureSource(model.piece_type)
                        Component.onCompleted: if(model.is_spot && model.is_occupied) source = getPictureSource(model.piece_type)

                        function getPictureSource(piece_type) {
                            switch(piece_type) {
                            case CheckersModel.WhiteMan: return "/images/manw.png";
                            case CheckersModel.BlackMan: return "/images/manb.png";
                            case CheckersModel.WhiteKing: return "/images/kingw.png";
                            case CheckersModel.BlackKing: return "/images/kingb.png";
                            default: source = "";
                            }
                        }
                    }
                    ColorOverlay {
                        id: piece_overlay
                        visible: false
                        anchors.fill: piece_image
                        source: piece_image
                        color: "#800000FF"
                    }

                    transitions: Transition {
                        ParentAnimation {
                            via: root
                            NumberAnimation { properties: "x,y"; duration: 400 }
                        }
                    }
                }
            }
        }
    }
}


