QT       += network

SOURCES += $$PWD/SettingsDialog.cpp \
    $$PWD/InfoDialog.cpp \
    $$PWD/TransferProgressBar.cpp \
    $$PWD/UsageProgressBar.cpp \
    $$PWD/SetupWizard.cpp \
    $$PWD/NodeSelector.cpp \
    $$PWD/FolderBinder.cpp \
    $$PWD/BindFolderDialog.cpp \
    $$PWD/UploadToMegaDialog.cpp \
    $$PWD/PasteMegaLinksDialog.cpp \
    $$PWD/ImportMegaLinksDialog.cpp \
    $$PWD/ImportListWidgetItem.cpp \
    $$PWD/CrashReportDialog.cpp \
    $$PWD/MultiQFileDialog.cpp \
    $$PWD/MegaProxyStyle.cpp \
    $$PWD/AccountDetailsDialog.cpp \
    $$PWD/DownloadFromMegaDialog.cpp \
    $$PWD/SizeLimitDialog.cpp \
    $$PWD/UsageWidget.cpp \
    $$PWD/MessageBox.cpp \
    $$PWD/QMegaModel.cpp \
    $$PWD/MegaItem.cpp \
    $$PWD/ChangeLogDialog.cpp \
    $$PWD/GuestWidget.cpp \
    $$PWD/StreamingFromMegaDialog.cpp \
    $$PWD/ConfirmSSLexception.cpp \
    $$PWD/UpgradeDialog.cpp \
    $$PWD/PlanWidget.cpp \
    $$PWD/InfoWizard.cpp \
    $$PWD/TransferManager.cpp \
    $$PWD/TransfersWidget.cpp \
    $$PWD/QTransfersModel.cpp \
    $$PWD/QActiveTransfersModel.cpp \
    $$PWD/QFinishedTransfersModel.cpp \
    $$PWD/MegaTransferDelegate.cpp \
    $$PWD/MegaTransferView.cpp \
    $$PWD/QMegaMessageBox.cpp \
    $$PWD/TransfersStateInfoWidget.cpp \
    $$PWD/MegaSpeedGraph.cpp \
    $$PWD/ActiveTransfersWidget.cpp \
    $$PWD/AvatarWidget.cpp \
    $$PWD/MenuItemAction.cpp \
    $$PWD/DataUsageMenu.cpp \
    $$PWD/AddExclusionDialog.cpp \
    $$PWD/LocalCleanScheduler.cpp \
    $$PWD/TransferManagerItem.cpp \
    $$PWD/TransferItem.cpp \
    $$PWD/InfoDialogTransfersWidget.cpp \
    $$PWD/QCustomTransfersModel.cpp \
    $$PWD/StatusInfo.cpp \
    $$PWD/ChangePassword.cpp \
    $$PWD/CustomTransferItem.cpp \
    $$PWD/PSAwidget.cpp \
    $$PWD/ElidedLabel.cpp \
    $$PWD/UpgradeOverStorage.cpp \
    $$PWD/UpgradeWidget.cpp \
    $$PWD/Login2FA.cpp \
    $$PWD/QRWidget.cpp

HEADERS  += $$PWD/SettingsDialog.h \
    $$PWD/InfoDialog.h \
    $$PWD/TransferProgressBar.h \
    $$PWD/UsageProgressBar.h \
    $$PWD/SetupWizard.h \
    $$PWD/NodeSelector.h \
    $$PWD/FolderBinder.h \
    $$PWD/BindFolderDialog.h \
    $$PWD/UploadToMegaDialog.h \
    $$PWD/PasteMegaLinksDialog.h \
    $$PWD/ImportMegaLinksDialog.h \
    $$PWD/ImportListWidgetItem.h \
    $$PWD/CrashReportDialog.h \
    $$PWD/MultiQFileDialog.h \
    $$PWD/MegaProxyStyle.h \
    $$PWD/AccountDetailsDialog.h \
    $$PWD/DownloadFromMegaDialog.h \
    $$PWD/SizeLimitDialog.h \
    $$PWD/UsageWidget.h \
    $$PWD/MessageBox.h \
    $$PWD/QMegaModel.h \
    $$PWD/MegaItem.h \
    $$PWD/ChangeLogDialog.h \
    $$PWD/GuestWidget.h \
    $$PWD/StreamingFromMegaDialog.h \
    $$PWD/ConfirmSSLexception.h \
    $$PWD/UpgradeDialog.h \
    $$PWD/PlanWidget.h \
    $$PWD/InfoWizard.h \
    $$PWD/TransferManager.h \
    $$PWD/TransfersWidget.h \
    $$PWD/QTransfersModel.h \
    $$PWD/QActiveTransfersModel.h \
    $$PWD/QFinishedTransfersModel.h \
    $$PWD/MegaTransferDelegate.h \
    $$PWD/MegaTransferView.h \
    $$PWD/QMegaMessageBox.h \
    $$PWD/TransfersStateInfoWidget.h \
    $$PWD/MegaSpeedGraph.h \
    $$PWD/ActiveTransfersWidget.h \
    $$PWD/AvatarWidget.h \
    $$PWD/MenuItemAction.h \
    $$PWD/DataUsageMenu.h \
    $$PWD/AddExclusionDialog.h \
    $$PWD/LocalCleanScheduler.h \
    $$PWD/TransferManagerItem.h \
    $$PWD/TransferItem.h \
    $$PWD/InfoDialogTransfersWidget.h \
    $$PWD/QCustomTransfersModel.h \
    $$PWD/StatusInfo.h \
    $$PWD/CustomTransferItem.h \
    $$PWD/PSAwidget.h \
    $$PWD/ElidedLabel.h \
    $$PWD/UpgradeOverStorage.h \
    $$PWD/UpgradeWidget.h \
    $$PWD/ChangePassword.h \
    $$PWD/Login2FA.h \
    $$PWD/QRWidget.h

INCLUDEPATH += $$PWD

debug {
    DEFINES += SHOW_LOGS
}

win32 {
    RESOURCES += $$PWD/Resources_win.qrc
    INCLUDEPATH += $$PWD/win
    FORMS    += $$PWD/win/InfoDialog.ui \
                $$PWD/win/CustomTransferItem.ui \
                $$PWD/win/TransferProgressBar.ui \
                $$PWD/win/UsageProgressBar.ui \
                $$PWD/win/NodeSelector.ui \
                $$PWD/win/FolderBinder.ui \
                $$PWD/win/BindFolderDialog.ui \
                $$PWD/win/UploadToMegaDialog.ui \
                $$PWD/win/PasteMegaLinksDialog.ui \
                $$PWD/win/ImportMegaLinksDialog.ui \
                $$PWD/win/ImportListWidgetItem.ui \
                $$PWD/win/CrashReportDialog.ui \
                $$PWD/win/SetupWizard.ui \
                $$PWD/win/SettingsDialog.ui \
                $$PWD/win/AccountDetailsDialog.ui \
                $$PWD/win/DownloadFromMegaDialog.ui \
                $$PWD/win/SizeLimitDialog.ui \
                $$PWD/win/MessageBox.ui \
                $$PWD/win/ChangeLogDialog.ui \
                $$PWD/win/GuestWidget.ui \
                $$PWD/win/StreamingFromMegaDialog.ui \
                $$PWD/win/ConfirmSSLexception.ui \
                $$PWD/win/PlanWidget.ui \
                $$PWD/win/UpgradeDialog.ui \
                $$PWD/win/InfoWizard.ui \
                $$PWD/win/TransferManagerItem.ui \
                $$PWD/win/TransferManager.ui \
                $$PWD/win/TransfersWidget.ui \
                $$PWD/win/TransfersStateInfoWidget.ui \
                $$PWD/win/MegaSpeedGraph.ui \
                $$PWD/win/ActiveTransfersWidget.ui \
                $$PWD/win/AddExclusionDialog.ui \
                $$PWD/win/LocalCleanScheduler.ui \
                $$PWD/win/InfoDialogTransfersWidget.ui \
                $$PWD/win/StatusInfo.ui \
                $$PWD/win/PSAwidget.ui \
                $$PWD/win/UpgradeOverStorage.ui \
                $$PWD/win/UpgradeWidget.ui \
                $$PWD/win/ChangePassword.ui \
                $$PWD/win/Login2FA.ui
}

macx {
    RESOURCES += $$PWD/Resources_macx.qrc
    INCLUDEPATH += $$PWD/macx
    FORMS    += $$PWD/macx/InfoDialog.ui \
                $$PWD/macx/TransferProgressBar.ui \
                $$PWD/macx/UsageProgressBar.ui \
                $$PWD/macx/NodeSelector.ui \
                $$PWD/macx/FolderBinder.ui \
                $$PWD/macx/BindFolderDialog.ui \
                $$PWD/macx/UploadToMegaDialog.ui \
                $$PWD/macx/PasteMegaLinksDialog.ui \
                $$PWD/macx/ImportMegaLinksDialog.ui \
                $$PWD/macx/ImportListWidgetItem.ui \
                $$PWD/macx/CrashReportDialog.ui \
                $$PWD/macx/SetupWizard.ui \
                $$PWD/macx/SettingsDialog.ui \
                $$PWD/macx/AccountDetailsDialog.ui \
                $$PWD/macx/DownloadFromMegaDialog.ui \
                $$PWD/macx/SizeLimitDialog.ui \
                $$PWD/macx/MessageBox.ui \
                $$PWD/macx/ChangeLogDialog.ui \
                $$PWD/macx/GuestWidget.ui \
                $$PWD/macx/StreamingFromMegaDialog.ui \
                $$PWD/macx/PermissionsDialog.ui \
                $$PWD/macx/PermissionsWidget.ui \
                $$PWD/macx/ConfirmSSLexception.ui \
                $$PWD/macx/PlanWidget.ui \
                $$PWD/macx/UpgradeDialog.ui \
                $$PWD/macx/InfoWizard.ui \
                $$PWD/macx/TransferManagerItem.ui \
                $$PWD/macx/TransferManager.ui \
                $$PWD/macx/TransfersWidget.ui \
                $$PWD/macx/TransfersStateInfoWidget.ui \
                $$PWD/macx/MegaSpeedGraph.ui \
                $$PWD/macx/ActiveTransfersWidget.ui \
                $$PWD/macx/AddExclusionDialog.ui \
                $$PWD/macx/LocalCleanScheduler.ui \
                $$PWD/macx/InfoDialogTransfersWidget.ui \
                $$PWD/macx/StatusInfo.ui \
                $$PWD/macx/CustomTransferItem.ui \
                $$PWD/macx/PSAwidget.ui \
                $$PWD/macx/UpgradeOverStorage.ui \
                $$PWD/macx/UpgradeWidget.ui \
                $$PWD/macx/ChangePassword.ui \
                $$PWD/macx/Login2FA.ui

    QT += macextras
    OBJECTIVE_SOURCES +=    gui/CocoaHelpButton.mm gui/MegaSystemTrayIcon.mm gui/QSegmentedControl.mm
    HEADERS += gui/CocoaHelpButton.h gui/MegaSystemTrayIcon.h gui/QSegmentedControl.h

    HEADERS += $$PWD/PermissionsDialog.h \
               $$PWD/PermissionsWidget.h
    SOURCES += $$PWD/PermissionsDialog.cpp \
               $$PWD/PermissionsWidget.cpp
}

unix:!macx {
    RESOURCES += $$PWD/Resources_linux.qrc
    INCLUDEPATH += $$PWD/linux
    FORMS    += $$PWD/linux/InfoDialog.ui \
                $$PWD/linux/CustomTransferItem.ui \
                $$PWD/linux/TransferProgressBar.ui \
                $$PWD/linux/UsageProgressBar.ui \
                $$PWD/linux/NodeSelector.ui \
                $$PWD/linux/FolderBinder.ui \
                $$PWD/linux/BindFolderDialog.ui \
                $$PWD/linux/UploadToMegaDialog.ui \
                $$PWD/linux/PasteMegaLinksDialog.ui \
                $$PWD/linux/ImportMegaLinksDialog.ui \
                $$PWD/linux/ImportListWidgetItem.ui \
                $$PWD/linux/CrashReportDialog.ui \
                $$PWD/linux/SetupWizard.ui \
                $$PWD/linux/SettingsDialog.ui \
                $$PWD/linux/AccountDetailsDialog.ui \
                $$PWD/linux/DownloadFromMegaDialog.ui \
                $$PWD/linux/SizeLimitDialog.ui \
                $$PWD/linux/MessageBox.ui\
                $$PWD/linux/ChangeLogDialog.ui \
                $$PWD/linux/GuestWidget.ui \
                $$PWD/linux/StreamingFromMegaDialog.ui \
                $$PWD/linux/PermissionsDialog.ui \
                $$PWD/linux/PermissionsWidget.ui \
                $$PWD/linux/ConfirmSSLexception.ui \
                $$PWD/linux/PlanWidget.ui \
                $$PWD/linux/UpgradeDialog.ui \
                $$PWD/linux/InfoWizard.ui \
                $$PWD/linux/TransferManagerItem.ui \
                $$PWD/linux/TransferManager.ui \
                $$PWD/linux/TransfersWidget.ui \
                $$PWD/linux/TransfersStateInfoWidget.ui \
                $$PWD/linux/MegaSpeedGraph.ui \
                $$PWD/linux/ActiveTransfersWidget.ui \
                $$PWD/linux/AddExclusionDialog.ui \
                $$PWD/linux/LocalCleanScheduler.ui \
                $$PWD/linux/InfoDialogTransfersWidget.ui \
                $$PWD/linux/StatusInfo.ui \
                $$PWD/linux/PSAwidget.ui \
                $$PWD/linux/UpgradeOverStorage.ui \
                $$PWD/linux/UpgradeWidget.ui \
                $$PWD/linux/ChangePassword.ui \
                $$PWD/linux/Login2FA.ui

    HEADERS += $$PWD/PermissionsDialog.h \
               $$PWD/PermissionsWidget.h
    SOURCES += $$PWD/PermissionsDialog.cpp \
               $$PWD/PermissionsWidget.cpp
}
