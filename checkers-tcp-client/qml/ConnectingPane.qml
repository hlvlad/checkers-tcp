import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15



Item {
    id: created_lobby_view
    property alias lobby_id : connecting_label.text
    Label {
        id: connecting_label
        anchors.centerIn: parent

        property int lobby_id

        text: "Connecting to lobby..."
        font.pointSize: 24
    }
}
