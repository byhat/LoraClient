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
    property var saveFileDialogComponent: null
    property var currentImageData: null
    property var currentImageTime: null

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

    Component {
        id: saveFileDialogComponentDef
        FileDialog {
            id: saveFileDialog
            modality: Qt.ApplicationModal
            fileMode: FileDialog.SaveFile
            acceptLabel: "Сохранить"
            rejectLabel: "Отмена"

            onAccepted: {
                if (selectedFile && currentImageData) {
                    appEngine.saveImageToPath(currentImageData, currentImageTime, selectedFile)
                }
                currentImageData = null
                currentImageTime = null
                destroy()
            }

            onRejected: {
                currentImageData = null
                currentImageTime = null
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

                        Item {
                            width: Math.min(parent.width * 0.8, 600)
                            height: Math.min(parent.width * 0.8 * 0.75, 450) + 30
                            anchors.horizontalCenter: parent.horizontalCenter
                            visible: modelData.image !== undefined && modelData.image !== ""

                            Image {
                                id: messageImage
                                anchors.fill: parent
                                anchors.bottomMargin: 30
                                source: modelData.image || ""
                                sourceSize.width: parent.width
                                sourceSize.height: parent.height - 30
                                fillMode: Image.PreserveAspectFit
                            }

                            Button {
                                id: saveButton
                                anchors.bottom: parent.bottom
                                anchors.horizontalCenter: parent.horizontalCenter
                                width: 80
                                height: 24
                                text: "Сохранить"
                                font.pixelSize: 11

                                background: Rectangle {
                                    color: saveButton.hovered ? "#4ade80" : "#3b82f6"
                                    radius: 4
                                }

                                contentItem: Text {
                                    text: saveButton.text
                                    font: saveButton.font
                                    color: "#ffffff"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                onClicked: {
                                    currentImageData = modelData.image
                                    currentImageTime = modelData.time
                                    
                                    if (!saveFileDialogComponent) {
                                        saveFileDialogComponent = saveFileDialogComponentDef
                                    }

                                    var dialog = saveFileDialogComponent.createObject(parent)
                                    if (dialog) {
                                        dialog.title = "Сохранить изображение"
                                        
                                        // Generate default filename from timestamp
                                        var defaultFilename = "image_"
                                        if (modelData.time) {
                                            // Replace colons with dashes for valid filename
                                            var timeStr = modelData.time.replace(/:/g, "-")
                                            defaultFilename += timeStr
                                        } else {
                                            var now = new Date()
                                            defaultFilename += now.toISOString().slice(0, 19).replace(/:/g, "-").replace("T", "_")
                                        }
                                        defaultFilename += ".png"
                                        
                                        dialog.selectedFile = new URL("file://" + defaultFilename).toString()
                                        dialog.nameFilters = ["Изображения (*.png)", "Все файлы (*)"]
                                        dialog.open()
                                    } else {
                                        console.error("Не удалось создать SaveFileDialog")
                                    }
                                }
                            }
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
