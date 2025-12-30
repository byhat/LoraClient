import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Particles

import "Pages"

ApplicationWindow {
    id: window
    width: 800
    height: 600
    visible: true
    title: "LoRa Messenger"

    background: Rectangle {
        color: "#0f0f1b"
    }

    header: ToolBar {
        background: Rectangle {
            color: "#1a1a2e"
            border.color: "#2d2d44"
            border.width: 1
        }

        RowLayout {
            anchors.fill: parent
            spacing: 0

            ToolButton {
                text: "☰"
                font.pixelSize: 18
                onClicked: drawer.open()
                Layout.preferredWidth: 56
                Layout.fillHeight: true
                contentItem: Text {
                    text: parent.text
                    color: "#e6e7ee"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 18
                }
                hoverEnabled: true
                background: Rectangle {
                    color: parent.hovered ? "#25253a" : "transparent"
                    radius: 0
                }
            }

            Label {
                text: loader.sourceComponent === connectionComponent ? "Настройки" :
                      loader.sourceComponent === chatComponent ? "Чат" : "LoRa Messenger"
                color: "#e6e7ee"
                font.pixelSize: 18
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                Layout.fillWidth: true
                elide: Label.ElideRight
            }

            Item {
                Layout.preferredWidth: 56
            }
        }
    }

    Loader {
        id: loader
        anchors.fill: parent
        sourceComponent: connectionComponent
    }

    Drawer {
        id: drawer
        width: Math.min(window.width * 0.8, 280)
        height: window.height
        edge: Qt.LeftEdge
        modal: true
        interactive: true

        background: Rectangle {
            color: "#1a1a2e"
            border.color: "#2d2d44"
            border.width: 1
        }

        ListView {
            anchors.fill: parent
            model: [
                { text: "Настройки", icon: "qrc:/AppQml/presentation/resources/icons/setting.svg", component: connectionComponent },
                { text: "Чат", icon: "qrc:/AppQml/presentation/resources/icons/chat.svg", component: chatComponent }
            ]
            delegate: ItemDelegate {
                id: delegate
                width: parent.width
                height: 56

                property bool isSelected: loader.sourceComponent === modelData.component
                property bool isHovered: delegate.hovered
                property bool isPressed: delegate.down

                background: Rectangle {
                    color: {
                        if (isPressed) return "#29293d";
                        if (isHovered) return "#222236";
                        if (isSelected) return "#5a5fcf";
                        return "transparent";
                    }
                    radius: 0
                }

                contentItem: Row {
                    spacing: 16
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20

                    Image {
                        source: modelData.icon
                        visible: source !== "" && source !== "qrc:/"
                        width: 20; height: 20
                        fillMode: Image.PreserveAspectFit
                        opacity: isSelected ? 1.0 : 0.7
                    }

                    Text {
                        text: modelData.text
                        font.pixelSize: 16
                        color: isSelected ? "#e6e7ee" : "#c5c6d0"
                        font.bold: isSelected
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                onClicked: {
                    loader.sourceComponent = modelData.component
                    drawer.close()
                }

                hoverEnabled: true
            }

            highlightFollowsCurrentItem: false
            currentIndex: -1
        }
    }

    Component {
        id: connectionComponent
        ConnectionPage_Snow {}
    }

    Component {
        id: chatComponent
        ChatPage_Snow {}
    }
}
