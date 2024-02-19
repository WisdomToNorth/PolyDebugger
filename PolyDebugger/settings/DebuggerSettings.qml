import ".."
import NgSettings 1.0
import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13
import QtQuick.Controls.Material 2.12

SplitView {
    id: settings

    Material.theme: Material.Light
    Material.accent: Material.Indigo
    Material.primary: Material.Indigo
    Material.elevation: 5
    Material.background: Material.White
    Material.foreground: Material.Black
    orientation: Qt.Horizontal

    Rectangle {
        id: columnWrapper

        width: settings.width
        height: settings.height

        ColumnLayout {
            id: columnLayout

            width: parent.width * 0.3
            height: parent.height
            anchors.centerIn: parent

            NgSettings {
                id: ngsettings

                appAlgorithmCore: algorithmCoreComboBox.currentIndex
            }

            GroupBox {
                title: "Settings"
                implicitWidth: parent.width

                ComboBox {
                    id: algorithmCoreComboBox

                    implicitWidth: parent.width
                    model: ["Calc", "NgPoly", "Slipper"]
                    currentIndex: 0
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
