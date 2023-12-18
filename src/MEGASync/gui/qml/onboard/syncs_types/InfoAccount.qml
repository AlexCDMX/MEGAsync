import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.15

import common 1.0

import components.texts 1.0 as Texts
import components.images 1.0

import onboard 1.0

import AccountInfoData 1.0
import Onboarding 1.0

Item {
    id: root

    readonly property string accountTypeFree: qsTr("Free")
    readonly property string accountTypeProI: qsTr("Pro I")
    readonly property string accountTypeProII: qsTr("Pro II")
    readonly property string accountTypeProIII: qsTr("Pro III")
    readonly property string accountTypeLite: qsTr("Pro Lite")
    readonly property string accountTypeBusiness: qsTr("Business")
    readonly property string accountTypeProFlexi: qsTr("Pro Flexi")
    readonly property string availableStorage: qsTr("Available storage:")
    readonly property string storageSpace: qsTr("Storage space:")

    function getAccountTypeImage() {
        switch(AccountInfoData.type) {
            case AccountInfoData.ACCOUNT_TYPE_FREE:
                return Images.shield_account_free;
            case AccountInfoData.ACCOUNT_TYPE_PROI:
                return Images.shield_account_proI;
            case AccountInfoData.ACCOUNT_TYPE_PROII:
                return Images.shield_account_proII;
            case AccountInfoData.ACCOUNT_TYPE_PROIII:
                return Images.shield_account_proIII;
            case AccountInfoData.ACCOUNT_TYPE_LITE:
                return Images.shield_account_lite;
            case AccountInfoData.ACCOUNT_TYPE_BUSINESS:
                return Images.building;
            case AccountInfoData.ACCOUNT_TYPE_PRO_FLEXI:
                return Images.infinity;
            default:
                return "";
        }
    }

    function getAccountTypeText() {
        switch(AccountInfoData.type) {
            case AccountInfoData.ACCOUNT_TYPE_FREE:
                return accountTypeFree;
            case AccountInfoData.ACCOUNT_TYPE_PROI:
                return accountTypeProI;
            case AccountInfoData.ACCOUNT_TYPE_PROII:
                return accountTypeProII;
            case AccountInfoData.ACCOUNT_TYPE_PROIII:
                return accountTypeProIII;
            case AccountInfoData.ACCOUNT_TYPE_LITE:
                return accountTypeLite;
            case AccountInfoData.ACCOUNT_TYPE_BUSINESS:
                return accountTypeBusiness;
            case AccountInfoData.ACCOUNT_TYPE_PRO_FLEXI:
                return accountTypeProFlexi;
            default:
                return "";
        }
    }

    width: parent.width
    height: 48

    Component.onCompleted: {
        AccountInfoData.requestAccountInfoData();
    }

    Rectangle {
        id: background

        anchors.fill: parent
        color: Styles.pageBackground
        radius: 8
        border {
            color: Styles.borderDisabled
            width: 1
        }

        RowLayout {
            id: internalLayout

            anchors.fill: parent
            spacing: 0
            visible: AccountInfoData.type !== AccountInfoData.ACCOUNT_TYPE_NOT_SET

            RowLayout {
                id: leftLayout

                Layout.alignment: Qt.AlignLeft
                Layout.leftMargin: 24
                spacing: 8

                SvgImage {
                    id: typeImage

                    source: getAccountTypeImage()
                    height: 16
                    width: 16
                    sourceSize: Qt.size(width, height)
                    opacity: enabled ? 1.0 : 0.2
                }

                Texts.Text {
                    id: typeText

                    Layout.alignment: Qt.AlignLeft
                    font.weight: Font.DemiBold
                    font.pixelSize: Texts.Text.Size.Medium
                    text: getAccountTypeText()
                }
            }

            RowLayout {
                id: rightLayout

                Layout.alignment: Qt.AlignRight
                Layout.rightMargin: 24
                visible: AccountInfoData.type !== AccountInfoData.ACCOUNT_TYPE_BUSINESS
                         && AccountInfoData.type !== AccountInfoData.ACCOUNT_TYPE_PRO_FLEXI
                         && AccountInfoData.type !== AccountInfoData.ACCOUNT_TYPE_NOT_SET

                Texts.Text {
                    id: constantText

                    text: AccountInfoData.belowMinUsedStorageThreshold
                          ? availableStorage
                          : storageSpace
                    font.weight: Font.DemiBold
                }

                Texts.Text {
                    id: usedStorageText

                    font.weight: Font.DemiBold
                    text: AccountInfoData.usedStorage
                    visible: !AccountInfoData.belowMinUsedStorageThreshold
                }

                Texts.SecondaryText {
                    id: separatorText

                    font.weight: Font.DemiBold
                    text: "/"
                    visible: !AccountInfoData.belowMinUsedStorageThreshold
                }

                Texts.SecondaryText {
                    id: totalStorage

                    font.weight: Font.DemiBold
                    text: AccountInfoData.totalStorage
                }
            }

        } // RowLayout: internalLayout

    } // Rectangle: background

    DropShadow {
        id: shadow

        anchors.fill: parent
        horizontalOffset: 0
        verticalOffset: 5
        radius: 5.0
        samples: 11
        cached: true
        color: "#0d000000"
        source: background
        visible: parent.enabled
    }
}