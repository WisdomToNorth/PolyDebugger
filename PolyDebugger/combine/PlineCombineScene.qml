import ".."
import Polyline 1.0
import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.13

SplitView {
    id: splitView

    Material.theme: Material.Light
    Material.accent: Material.Blue
    Material.primary: Material.Indigo
    Material.elevation: 5
    Material.background: Material.White
    Material.foreground: Material.Black
    orientation: Qt.Horizontal

    GeometrySceneView {
        implicitWidth: splitView.width * 0.8

        PlineCombineAlgorithmView {
            id: algorithmView

            anchors.fill: parent
            plineCombineMode: combineModeComboBox.currentIndex
        }

    }

    ColumnLayout {
        anchors.topMargin: 5
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        implicitWidth: splitView.width * 0.2

        CheckBox {
            width: parent.width
            text: "Show Vertexes"
            checked: algorithmView.showVertexes
            onCheckedChanged: {
                algorithmView.showVertexes = checked;
            }
        }

        CheckBox {
            width: parent.width
            text: "Show Intersects"
            checked: algorithmView.showIntersects
            onCheckedChanged: {
                algorithmView.showIntersects = checked;
            }
        }

        GroupBox {
            title: "Combine Mode"
            leftInset: 5
            rightInset: 15
            implicitWidth: parent.width

            ColumnLayout {
                implicitWidth: parent.width

                ComboBox {
                    id: combineModeComboBox

                    implicitWidth: parent.width
                    leftInset: 5
                    rightInset: 15
                    model: ["None", "Union", "Exclude", "Intersect", "XOR", "Coincident Slices"]
                }

                CheckBox {
                    text: "Flip Arg Order"
                    checked: algorithmView.flipArgOrder
                    onCheckedChanged: {
                        algorithmView.flipArgOrder = checked;
                    }
                }

            }

        }

        GroupBox {
            title: "Winding Number"
            leftInset: 5
            rightInset: 15
            implicitWidth: parent.width

            ColumnLayout {
                implicitWidth: parent.width

                CheckBox {
                    text: "Show Test Point"
                    checked: algorithmView.showWindingNumberPoint
                    onCheckedChanged: {
                        algorithmView.showWindingNumberPoint = checked;
                    }
                }

                Text {
                    id: windingNumberText

                    leftPadding: 6
                    text: "Winding Number: " + algorithmView.windingNumber
                }

            }

        }

        Item {
            Layout.fillHeight: true
        }

    }

}
