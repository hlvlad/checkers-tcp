import QtQuick
import QtQuick.Dialogs

MessageDialog {
    property string type: "info"
    text: type === "info" ? "Info" : "Error"
}
