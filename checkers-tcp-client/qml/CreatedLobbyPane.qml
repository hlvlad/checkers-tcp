import QtQuick
import QtQuick.Controls
import QtQuick.Layouts



Item {
    id: created_lobby_view
    property alias lobby_id : lobby_id_label.text

    ColumnLayout {
        anchors.centerIn: parent

        Label {
            text: "Your lobby number is:"
            font.pointSize: 24
            Layout.alignment: Qt.AlignHCenter
        }
        TextInput {
            id: lobby_id_label
            readOnly: true
            selectByMouse: true
            font.pointSize: 40
            Layout.alignment: Qt.AlignHCenter
        }
    }
}
