import ".."
import NgSettings 1.0
import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13

SplitView {
    // Other children of SplitView...

    id: settings

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
                    currentIndex: 1
                }

            }

            Item {
                Layout.fillHeight: true
            }

        }

    }

}