// System
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

// QML common
import Components.Buttons 1.0 as MegaButtons
import Common 1.0

// Local
import Onboard 1.0

// C++
import Onboarding 1.0

RowLayout {

    property alias leftSecondary: leftSecondary
    property alias rightSecondary: rightSecondary
    property alias rightPrimary: rightPrimary

    anchors {
        bottom: parent.bottom
        right: parent.right
        left: parent.left
        leftMargin: -leftSecondary.sizes.focusBorderWidth
        bottomMargin: -leftSecondary.sizes.focusBorderWidth
        rightMargin: -rightPrimary.sizes.focusBorderWidth
    }

    MegaButtons.OutlineButton {
        id: leftSecondary

        text: OnboardingStrings.skip
        onClicked: {
            onboardingWindow.close();
        }
    }

    RowLayout {
        Layout.alignment: Qt.AlignRight

        MegaButtons.OutlineButton {
            id: rightSecondary

            text: OnboardingStrings.previous
        }

        MegaButtons.PrimaryButton {
            id: rightPrimary

            text: OnboardingStrings.next
            icons.source: Images.arrowRight
        }

    }
}
