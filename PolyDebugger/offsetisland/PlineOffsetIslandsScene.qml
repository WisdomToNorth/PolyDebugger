import ".."
import Polyline 1.0
import QtQuick 2.13
import QtQuick.Controls 1.4
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts 1.13

SplitView {
    id: splitViewHorizontal

    Material.theme: Material.Light
    Material.accent: Material.Indigo
    Material.primary: Material.Indigo
    Material.elevation: 5
    Material.background: Material.White
    Material.foreground: Material.Black
    orientation: Qt.Horizontal

    GeometrySceneView {
        implicitWidth: splitViewHorizontal.width * 0.8 // 80% of the SplitView's width

        OffsetIslandsView {
            id: algorithmView

            anchors.fill: parent
        }

    }

    SplitView {
        id: splitViewVertical

        orientation: Qt.Vertical
        implicitWidth: splitViewHorizontal.width * 0.2

        TreeView {
            id: fileSystemView

            implicitHeight: splitViewVertical.height * 0.3

            TableViewColumn {
                title: "Name"
                role: "fileName"
                width: fileSystemView.width * 0.7
                resizable: false
            }

            TableViewColumn {
                title: "info"
                role: "filePermissions"
                width: fileSystemView.width * 0.25
                resizable: false
            }

            model: ListModel {
                ListElement {
                    fileName: "File 1"
                    filePermissions: "Read-Write"
                }

                ListElement {
                    fileName: "File 2"
                    filePermissions: "Read-Only"
                }

                ListElement {
                    fileName: "File 3"
                    filePermissions: "Read-Write"
                }

            }
            // 50% of the SplitView's height

            itemDelegate: Item {
                width: parent.width
                height: 20

                Loader {
                    sourceComponent: styleData.column === 0 ? checkBoxComponent : textComponent
                    anchors.verticalCenter: parent.verticalCenter
                }

                Component {
                    id: checkBoxComponent

                    CheckBox {
                        checked: model.checked
                        text: model.fileName
                    }

                }

                Component {
                    id: textComponent

                    Text {
                        text: model.filePermissions
                    }

                }

            }

        }

        GroupBox {
            id: controlPanel

            implicitHeight: splitViewVertical.height * 0.7 // 20% of the SplitView's width

            ComboBox {
                id: caseIndexComboBox

                anchors.top: parent.top
                anchors.topMargin: 15
                leftInset: 5
                rightInset: 15
                width: parent.width
                model: ["default1", "default2", "default3"]
                currentIndex: 0
                onCurrentIndexChanged: {
                    algorithmView.caseIndex = model[currentIndex];
                }
            }

            Label {
                id: offsetDeltaLabel

                anchors.top: caseIndexComboBox.bottom
                anchors.topMargin: 15
                topPadding: 5
                text: "Offset Delta"
            }

            Item {
                id: offsetItem

                anchors.top: offsetDeltaLabel.bottom
                width: parent.width
                implicitWidth: offsetSlider.implicitWidth
                implicitHeight: offsetTextField.implicitHeight + offsetSlider.implicitHeight + minText.implicitHeight

                TextField {
                    id: offsetTextField

                    width: parent.width
                    selectByMouse: true // 鼠标可以选中文本
                    anchors.top: offsetItem.top
                    text: parseFloat(offsetSlider.value).toFixed(2)
                    onEditingFinished: {
                        let f = parseFloat(text);
                        if (!isNaN(f))
                            offsetSlider.value = f;

                    }

                    validator: DoubleValidator {
                        bottom: offsetSlider.from
                        top: offsetSlider.to
                    }

                }

                Slider {
                    id: offsetSlider

                    width: parent.width
                    anchors.top: offsetTextField.bottom
                    from: -15
                    to: 15
                    value: algorithmView.offsetDelta
                    onValueChanged: {
                        algorithmView.offsetDelta = value;
                    }
                }

                Text {
                    id: minText

                    anchors.left: offsetSlider.left
                    anchors.top: offsetSlider.bottom
                    text: offsetSlider.from
                }

                Text {
                    id: maxText

                    anchors.right: offsetSlider.right
                    anchors.top: offsetSlider.bottom
                    text: offsetSlider.to
                }

            }

            Label {
                id: offsetCountLabel

                anchors.top: offsetItem.bottom
                anchors.topMargin: 30
                topPadding: 5
                text: "Offset Count"
            }

            TextField {
                id: offsetCountField

                anchors.top: offsetCountLabel.bottom
                width: parent.width
                text: algorithmView.offsetCount
                selectByMouse: true // 鼠标可以选中文本
                onTextChanged: {
                    let c = parseInt(text);
                    if (isNaN(c))
                        return ;

                    algorithmView.offsetCount = c;
                }

                validator: IntValidator {
                    bottom: 0
                    top: 1000
                }

            }

            CheckBox {
                anchors.top: offsetCountField.bottom
                anchors.topMargin: 30
                text: "Show Vertexes"
                checked: algorithmView.showVertexes
                onCheckedChanged: {
                    algorithmView.showVertexes = checked;
                }
            }

        }

    }

}
