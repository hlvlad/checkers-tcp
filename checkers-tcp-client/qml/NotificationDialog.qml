import QtQuick 2.15
import QtQuick.Dialogs 1.3

MessageDialog {
    property string type: "info"
    icon: type === "info" ? StandardIcon.Information : StandardIcon.Warning
    title: type === "info" ? "Info" : "Error"
}
