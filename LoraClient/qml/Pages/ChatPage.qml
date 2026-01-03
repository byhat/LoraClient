import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Page {
    id: chatPage

    background: Rectangle {
        color: "#22233a"
        border.color: "#2d2d44"
        border.width: 1
    }
    property var fileDialogComponent: null

    Component {
        id: fileDialogComponentDef
        FileDialog {
            id: fileDialog
            modality: Qt.ApplicationModal

            onAccepted: {
                appEngine.onSendImage(selectedFile)
                destroy()
            }

            onRejected: {
                destroy()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        ListView {
            id: messageList
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 8

            onCountChanged: {
                if (count > 0) {
                    positionViewAtEnd()
                }
            }

            model: appEngine.messages
            delegate: ItemDelegate {
                width: ListView.view.width
                height: Math.max(40, contentItem.height + 12)

                leftPadding: 12
                rightPadding: 12
                topPadding: 6
                bottomPadding: 6

                contentItem: Item {
                    width: parent.width - (parent.leftPadding + parent.rightPadding)
                    height: childrenRect.height

                    Column {
                        spacing: 4
                        width: parent.width

                        Text {
                            id: messageText
                            width: parent.width
                            text: modelData.text
                            wrapMode: Text.Wrap
                            font.pixelSize: 14
                            color: modelData.type === "sent" ? "#a5baff" :
                                   modelData.type === "received" ? "#a5e0aa" :
                                   "#fca5a5"
                            visible: modelData.text && modelData.text !== ""
                        }

                        Image {
                            id: messageImage
                            source: modelData.image || ""
                            sourceSize.width: parent.width * 0.8
                            sourceSize.height: parent.width * 0.8 * 0.75
                            fillMode: Image.PreserveAspectFit
                            visible: modelData.image !== undefined && modelData.image !== ""
                            anchors.horizontalCenter: parent.horizontalCenter

                            width: Math.min(parent.width * 0.8, 600)
                            height: Math.min(parent.width * 0.8 * 0.75, 450)
                        }

                        Text {
                            text: modelData.time
                            font.pixelSize: 12
                            color: "#888"
                            opacity: 0.7
                            horizontalAlignment: Text.AlignRight
                            width: parent.width
                        }
                    }
                }

                background: Rectangle {
                    color: modelData.type === "sent" ? "#2a3b6e" :
                           modelData.type === "received" ? "#2a4a3a" :
                           "#4a2a3a"
                    border.color: "#333d66"
                    border.width: 1
                    radius: 8
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 6
            visible: appEngine.sendProgress > 0 || appEngine.receiveProgress > 0

            ColumnLayout {
                visible: appEngine.sendProgress > 0
                spacing: 2

                RowLayout {
                    spacing: 8
                    Label {
                        text: "Отправка:"
                        font.bold: true
                        color: "#a5baff"
                    }
                    Label {
                        text: appEngine.sendProgressText
                        color: "#e6e7ee"
                    }
                    Label {
                        text: "(" + appEngine.sendProgress + "%)"
                        color: "#888"
                    }
                }

                ProgressBar {
                    Layout.fillWidth: true
                    value: appEngine.sendProgress / 100.0
                    palette.highlight: "#4ade80"
                }
            }

            ColumnLayout {
                visible: appEngine.receiveProgress > 0
                spacing: 2

                RowLayout {
                    spacing: 8
                    Label {
                        text: "Приём:"
                        font.bold: true
                        color: "#a5e0aa"
                    }
                    Label {
                        text: appEngine.receiveProgressText
                        color: "#e6e7ee"
                    }
                    Label {
                        text: "(" + appEngine.receiveProgress + "%)"
                        color: "#888"
                    }
                }

                ProgressBar {
                    Layout.fillWidth: true
                    value: appEngine.receiveProgress / 100.0
                    palette.highlight: "#60a5fa"
                }
            }
        }

        RowLayout {
            spacing: 10

            TextField {
                id: messageInput
                placeholderText: "Введите сообщение..."
                Layout.fillWidth: true
                onAccepted: sendButton.clicked()

                color: "#e6e7ee"
                placeholderTextColor: "#6b7280"

                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 36
                    color: "#1e2035"
                    border.color: "#374151"
                    radius: 6
                }
            }

            Button {
                id: sendButton
                text: "Отправить"
                onClicked: {
                    if (messageInput.text.trim() !== "") {
                        appEngine.onSendText(messageInput.text.trim())
                        messageInput.text = ""
                    }
                }

                palette: Palette {
                    button: "#4ade80"
                    buttonText: "#0f172a"
                }
            }

            Button {
                text: "+"
                width: 40
                height: 40
                onClicked: {
                    if (!fileDialogComponent) {
                        fileDialogComponent = fileDialogComponentDef
                    }

                    var dialog = fileDialogComponent.createObject(parent)
                    if (dialog) {
                        dialog.title = "Выберите изображение"
                        dialog.nameFilters = ["Изображения (*.png *.jpg *.jpeg *.bmp)", "Все файлы (*)"]
                        dialog.open()
                    } else {
                        console.error("Не удалось создать FileDialog")
                    }
                }

                palette: Palette {
                    button: "#60a5fa"
                    buttonText: "#e6e7ee"
                }
                font.pixelSize: 20
            }
        }
    }
}
