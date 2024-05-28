import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import common 1.0

import components.texts 1.0 as Texts
import components.images 1.0
import components.busyIndicator 1.0

import BackupsProxyModel 1.0
import BackupsModel 1.0

Rectangle {
    id: root

    required property BackupsProxyModel backupsProxyModelRef

    readonly property int headerFooterMargin: 24
    readonly property int headerFooterHeight: 40
    readonly property int tableRadius: 8

    Layout.preferredWidth: width
    Layout.preferredHeight: height
    width: 400
    height: 225
    radius: tableRadius
    color: colorStyle.pageBackground

    Rectangle {
        id: borderRectangle

        width: root.width
        height: root.height
        color: "transparent"
        border.color: colorStyle.borderStrong
        border.width: 1
        radius: 8
        z: 5
    }

    ListView {
        id: listView

        anchors.fill: parent
        model: root.backupsProxyModelRef
        headerPositioning: ListView.OverlayHeader
        focus: true
        clip: true
        delegate: folderComponent
        header: headerComponent
        footerPositioning: ListView.OverlayFooter
        footer: footerComponent
        ScrollBar.vertical: ScrollBar {}
    }

    Component {
        id: headerComponent

        Rectangle {
            id: headerRectangle

            anchors {
                left: parent.left
                right: parent.right
            }
            height: headerFooterHeight
            color: colorStyle.pageBackground
            radius: root.radius
            z: 3

            RowLayout {
                id: headerLayout

                anchors.verticalCenter: parent.verticalCenter
                width: parent.width
                spacing: 0

                RowLayout {
                    id: imageTextLayout

                    Layout.leftMargin: headerFooterMargin
                    Layout.fillWidth: true
                    spacing: headerFooterMargin / 2

                    SvgImage {
                        id: headerImage

                        source: Images.database
                        color: colorStyle.iconPrimary
                        sourceSize: Qt.size(16, 16)
                    }

                    Texts.Text {
                        id: headerText

                        text: BackupsStrings.backupFolders
                        font.weight: Font.DemiBold
                        color: colorStyle.textPrimary
                    }
                }

                Texts.Text {
                    id: totalSizeText

                    Layout.rightMargin: headerFooterMargin
                    Layout.alignment: Qt.AlignRight
                    text: backupsModelAccess.totalSize
                    color: colorStyle.textPrimary
                    visible: backupsModelAccess.totalSizeReady
                    font {
                        pixelSize: Texts.Text.Size.SMALL
                        weight: Font.DemiBold
                    }
                }

                BusyIndicator {
                    id: busyIndicatorItem

                    Layout.rightMargin: headerFooterMargin
                    Layout.alignment: Qt.AlignRight
                    Layout.preferredWidth: 16
                    Layout.preferredHeight: 16
                    imageSize: Qt.size(16, 16)
                    visible: !backupsModelAccess.totalSizeReady
                    color: colorStyle.textAccent
                }
            }

            Rectangle {
                id: lineRectangle

                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }
                height: borderRectangle.border.width
                color: colorStyle.borderSubtle

            }

            MouseArea {
                id: headerMouseArea

                anchors.fill: parent
                hoverEnabled: true
            }

        } // Rectangle: headerRectangle

    } // Component: headerComponent

    Component {
        id: folderComponent

        FolderRow {
            id: folderRow

            backupsProxyModelRef: root.backupsProxyModelRef
        }
    }

    Component {
        id: footerComponent

        Rectangle {
            id: notificationFooter

            anchors {
                left: parent.left
                right: parent.right
            }
            height: notificationItem.height
            radius: tableRadius
            color: colorStyle.pageBackground
            z: 3
            visible: backupsModelAccess.globalError !== backupsModelAccess.BackupErrorCode.NONE

            Texts.NotificationText {
                id: notificationItem

                width: parent.width
                type: backupsModelAccess.globalError === backupsModelAccess.BackupErrorCode.SDK_CREATION
                        ? Constants.MessageType.ERROR
                        : Constants.MessageType.WARNING
                topBorderRect: true
                text: backupsModelAccess.conflictsNotificationText
                visible: parent.visible
            }
        }
    }

}
