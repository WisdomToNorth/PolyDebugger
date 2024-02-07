import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13
import QtQuick.Window 2.13
// import "combine"
// import "hilbert"
// import "offsetisland"
import "offset"

ApplicationWindow {
    // TabButton {
    //     text: "Polyline Offset Islands"
    // }
    // TabButton {
    //     text: "Polyline Combine"
    // }

    id: mainWindow

    visible: true
    width: Screen.width - 200
    height: Screen.height - 200
    title: qsTr("Cavalier Contours")

    font {
        family: "Consolas"
        pointSize: 20
    }

    Page {
        anchors.fill: parent

        Rectangle {
            anchors.fill: parent
            border.width: 2
            border.color: "grey"

            StackLayout {
                // PlineCombineScene {
                // }
                // PlineOffsetIslandsScene {
                // }
                // HilbertCurveScene {
                // }

                anchors.fill: parent
                anchors.margins: 2
                currentIndex: tabBar.currentIndex

                PlineOffsetScene {
                }

            }

        }

    }

    header: TabBar {
        id: tabBar

        // TabButton {
        //     text: "Hilbert Curve"
        // }
        TabButton {
            text: "Polyline Offset"
        }

    }

}
