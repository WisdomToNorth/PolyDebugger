import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.13
import QtQuick.Window 2.13
import "combine"
import "offset"
import "offsetisland"
import "settings"

ApplicationWindow {
    id: mainWindow

    function adjustFontSize(baseSize) {
        return baseSize * Screen.pixelDensity;
    }

    Material.theme: Material.Light
    Material.accent: Material.Blue
    Material.primary: Material.Indigo
    Material.elevation: 5
    Material.background: Material.White
    Material.foreground: Material.Black
    visible: true
    width: Screen.width - 200
    height: Screen.height - 200
    title: qsTr("Cavalier Contours")

    font {
        family: "Consolas"
    }

    Page {
        anchors.fill: parent

        Rectangle {
            anchors.fill: parent
            border.width: 2
            border.color: "grey"

            StackLayout {
                anchors.fill: parent
                anchors.margins: 2
                currentIndex: tabBar.currentIndex

                PlineOffsetIslandsScene {
                }

                PlineOffsetScene {
                }

                PlineCombineScene {
                }

                DebuggerSettings {
                }

            }

        }

    }

    header: TabBar {
        id: tabBar

        TabButton {
            text: "Polyline Offset Islands"
        }

        TabButton {
            text: "Polyline Offset"
        }

        TabButton {
            text: "Polyline Combine"
        }

        TabButton {
            text: "Debugger Settings"
        }

    }

}
