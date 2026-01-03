import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Particles

import "../Elements"

Page {
    id: connectionPage

    background: Rectangle {
        color: "#22233a"
        border.color: "#2d2d44"
        border.width: 1

    }

    SnowBG {}

    ScrollView {
        anchors.fill: parent
        anchors.margins: 12

        ColumnLayout {
            width: parent.width
            spacing: 12

            Text {
                text: "Настройка подключения LoRa USB адаптера"
                font.bold: true
                font.pixelSize: 18
                color: "#e6e7ee"
                Layout.alignment: Qt.AlignHCenter
                Layout.topMargin: 12
            }

            RowLayout {
                spacing: 12
                Layout.fillWidth: true
                Layout.leftMargin: 14
                Layout.rightMargin: 14

                Label {
                    text: "Порт:"
                    font.pixelSize: 14
                    color: "#c5c6d0"
                }

                ComboBox {
                    id: portSelector
                    Layout.fillWidth: true
                    model: appEngine.availablePorts
                    currentIndex: 0
                }

                Button {
                    text: "Обновить"
                    onClicked: appEngine.updateAvailablePorts()

                    palette: Palette {
                        button: "#3b4a8c"
                        buttonText: "#e6e7ee"
                    }
                }
            }

            RowLayout {
                spacing: 12
                Layout.fillWidth: true
                Layout.leftMargin: 14
                Layout.rightMargin: 14

                Label {
                    text: "Скорость:"
                    font.pixelSize: 14
                    color: "#c5c6d0"
                }

                ComboBox {
                    id: baudSelector
                    Layout.fillWidth: true
                    model: ["1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"]
                    currentIndex: 3
                }
            }

            Text {
                text: "Статус: " + (appEngine.isConnected ? "Подключено" : "Отключено")
                color: appEngine.isConnected ? "#4ade80" : "#f87171"
                font.pixelSize: 14
                Layout.leftMargin: 14
                Layout.rightMargin: 14
            }

            Text {
                text: appEngine.lastError
                color: "#f87171"
                wrapMode: Text.Wrap
                visible: appEngine.lastError !== ""
                font.pixelSize: 13
                Layout.fillWidth: true
                Layout.leftMargin: 14
                Layout.rightMargin: 14
            }

            RowLayout {
                spacing: 12
                Layout.leftMargin: 14
                Layout.rightMargin: 14

                Button {
                    text: "Подключиться"
                    onClicked: {
                        appEngine.portName = portSelector.currentText
                        appEngine.baudRate = parseInt(baudSelector.currentText)
                        appEngine.openPort()
                    }
                    enabled: !appEngine.isConnected

                    palette: Palette {
                        button: enabled ? "#4ade80" : "#374151"
                        buttonText: enabled ? "#111827" : "#9ca3af"
                    }
                }

                Button {
                    text: "Отключиться"
                    onClicked: appEngine.closePort()
                    enabled: appEngine.isConnected

                    palette: Palette {
                        button: enabled ? "#f87171" : "#374151"
                        buttonText: enabled ? "#111827" : "#9ca3af"
                    }
                }
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }
}
