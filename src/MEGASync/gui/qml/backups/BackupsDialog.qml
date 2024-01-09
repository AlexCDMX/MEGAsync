import QtQuick 2.0

import common 1.0

import BackupsQmlDialog 1.0

BackupsQmlDialog {
    id: backupsWindow

    title: "Configure backups"//OnboardingStrings.setUpMEGA
    visible: true
    modality: Qt.NonModal
    width: 496
    height: 560
    maximumHeight: 560
    maximumWidth: 496
    minimumHeight: 560
    minimumWidth: 496

    Rectangle {
        id: backgroundRect

        anchors.fill: parent
        color: Styles.surface1

        BackupsFlow {
            id: backupsFlowItem

            anchors.fill: parent
            anchors.margins: 48
        }
    }

    Connections {
        target: backupsFlowItem

        ignoreUnknownSignals: true

        function onBackupFlowMoveToBack() {
            backupsWindow.close();
        }

        function onBackupFlowMoveToFinal() {
            backupsWindow.close();
        }
    }
}
