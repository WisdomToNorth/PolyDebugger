import ".."
import Polyline 1.0
import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.13

SplitView {
    id: offsetsplitview

    GeometrySceneView {
        implicitWidth: offsetsplitview.width * 0.8

        PlineOffsetAlgorithmView {
            id: algorithmView

            anchors.fill: parent
            spatialIndexTarget: spatialIndexTargetComboBox.currentIndex
            selfIntersectsTarget: selfIntersectsTargetComboBox.currentIndex
            finishedPolyline: finishedPlinesComboBox.currentIndex
        }

    }

    ScrollView {
        id: offset_scrollview

        implicitWidth: offsetsplitview.width * 0.2
        anchors.topMargin: 10
        anchors.bottomMargin: 30
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        clip: true
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn

        ColumnLayout {
            id: first_clayout

            anchors.topMargin: 10
            anchors.bottomMargin: 30
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: offset_scrollview.width
            height: parent.height + 30

            GroupBox {
                id: offsetGroupBox1

                title: "Offset"
                leftInset: 5
                rightInset: 5
                implicitWidth: first_clayout.width

                ColumnLayout {
                    implicitWidth: parent.width

                    Item {
                        id: offsetItem

                        width: offsetGroupBox1.width
                        implicitHeight: offsetTextField.implicitHeight + offsetSlider.implicitHeight + minText.implicitHeight

                        TextField {
                            id: offsetTextField

                            width: offsetGroupBox1.width - 25
                            anchors.top: offsetItem.top
                            text: {
                                if (!focus)
                                    offsetSlider.value;
                                else
                                    "";
                            }
                            onTextChanged: {
                                let f = parseFloat(text);
                                if (isNaN(f))
                                    return ;

                                offsetSlider.value = f;
                            }

                            validator: DoubleValidator {
                                bottom: offsetSlider.from
                                top: offsetSlider.to
                            }

                        }

                        Slider {
                            id: offsetSlider

                            width: offsetGroupBox1.width - 25
                            anchors.top: offsetTextField.bottom
                            from: -40
                            to: 40
                            value: algorithmView.plineOffset
                            onValueChanged: {
                                algorithmView.plineOffset = value;
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
                        topPadding: 5
                        text: "Offset Count"
                    }

                    TextField {
                        width: parent.width
                        text: algorithmView.offsetCount
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
                        width: parent.width - 25
                        text: "Show Last Pruned Raw Offsets"
                        checked: algorithmView.showLastPrunedRawOffsets
                        onCheckedChanged: {
                            algorithmView.showLastPrunedRawOffsets = checked;
                        }
                    }

                }

            }

            CheckBox {
                implicitWidth: first_clayout.width
                text: "Show Original Polyline Vertexes"
                checked: algorithmView.showOrigPlineVertexes
                onCheckedChanged: {
                    algorithmView.showOrigPlineVertexes = checked;
                }
            }

            GroupBox {
                title: "Raw Offsets"
                leftInset: 5
                rightInset: 5
                implicitWidth: first_clayout.width

                ColumnLayout {
                    implicitWidth: parent.width

                    CheckBox {
                        id: showRawOffsetCheckBox

                        text: "Show Raw Offset Polyline"
                        checked: algorithmView.showRawOffsetPolyline
                        onCheckedChanged: {
                            algorithmView.showRawOffsetPolyline = checked;
                        }
                    }

                    CheckBox {
                        enabled: showRawOffsetCheckBox.checked
                        text: "Show Dual Raw Offset Polyline"
                        checked: algorithmView.showDualRawOffsetPolyline
                        onCheckedChanged: {
                            algorithmView.showDualRawOffsetPolyline = checked;
                        }
                    }

                    CheckBox {
                        enabled: showRawOffsetCheckBox.checked
                        text: "Show Raw Offset Polyline Vertexes"
                        checked: algorithmView.showRawOffsetPlineVertexes
                        onCheckedChanged: {
                            algorithmView.showRawOffsetPlineVertexes = checked;
                        }
                    }

                    CheckBox {
                        text: "Show Raw Offset Segments"
                        checked: algorithmView.showRawOffsetSegments
                        onCheckedChanged: {
                            algorithmView.showRawOffsetSegments = checked;
                        }
                    }

                }

            }

            GroupBox {
                title: "Self Intersects"
                leftInset: 5
                rightInset: 5
                implicitWidth: first_clayout.width

                ComboBox {
                    id: selfIntersectsTargetComboBox

                    width: parent.width
                    leftInset: 5
                    rightInset: 5
                    model: ["None", "Original Polyline", "Raw Offset Polyline"]
                }

            }

            GroupBox {
                title: "Finished Polylines"
                leftInset: 5
                rightInset: 5
                implicitWidth: first_clayout.width

                ComboBox {
                    id: finishedPlinesComboBox

                    width: parent.width
                    leftInset: 5
                    rightInset: 5
                    model: ["None", "Slices", "DualSlices", "Joined"]
                }

            }

            GroupBox {
                title: "Spatial Index"
                leftInset: 5
                rightInset: 5
                implicitWidth: first_clayout.width

                ComboBox {
                    id: spatialIndexTargetComboBox

                    leftInset: 5
                    rightInset: 5
                    width: parent.width
                    model: ["None", "Original Polyline", "Raw Offset Polyline"]
                }

            }

            CheckBox {
                id: showStartPointIntersectCirclesCheckBox

                width: first_clayout.width
                text: "Show End Intersect Circles"
                checked: algorithmView.showEndPointIntersectCircles
                onCheckedChanged: {
                    algorithmView.showEndPointIntersectCircles = checked;
                }
            }

        }

    }

}
