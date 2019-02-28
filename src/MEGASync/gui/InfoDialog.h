#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>
#include <QTimer>
#include <QProcess>
#include <QDateTime>
#include <QPainter>
#include "GuestWidget.h"
#include "SettingsDialog.h"
#include "DataUsageMenu.h"
#include "MenuItemAction.h"
#include "control/Preferences.h"
#include <QGraphicsOpacityEffect>

namespace Ui {
class InfoDialog;
}

class MegaApplication;
class InfoDialog : public QDialog, public mega::MegaTransferListener
{
    Q_OBJECT

    enum {
        STATE_STARTING,
        STATE_PAUSED,
        STATE_WAITING,
        STATE_INDEXING,
        STATE_UPDATED
    };

public:
    explicit InfoDialog(MegaApplication *app, QWidget *parent = 0);
    ~InfoDialog();

    void setUsage();
    void setAvatar();
    void setTransfer(mega::MegaTransfer *transfer);
    void refreshTransferItems();
    void transferFinished(int error);
    void setIndexing(bool indexing);
    void setWaiting(bool waiting);
    void setOverQuotaMode(bool state);
    void addSync(mega::MegaHandle h);
    void clearUserAttributes();
    void setPSAannouncement(int id, QString title, QString text, QString urlImage, QString textButton, QString linkButton);
    bool updateOverStorageState(int state);

    virtual void onTransferFinish(mega::MegaApi* api, mega::MegaTransfer *transfer, mega::MegaError* e);

#ifdef __APPLE__
    void moveArrow(QPoint p);
#endif

    void regenerateLayout();

private:
    void drawAvatar(QString email);
    void createQuotaUsedMenu();
    void animateStates(bool opt);

public slots:
   void addSync();
   void onAllUploadsFinished();
   void onAllDownloadsFinished();
   void onAllTransfersFinished();
   void updateDialogState();

private slots:
    void on_bSettings_clicked();
    void on_bUpgrade_clicked();
    void openFolder(QString path);
    void on_bChats_clicked();
    void onOverlayClicked();
    void on_bTransferManager_clicked();
    void onUserAction(int action);

    void on_bDotUsedStorage_clicked();
    void on_bDotUsedQuota_clicked();

    void on_bDiscard_clicked();
    void on_bBuyQuota_clicked();

    void hideUsageBalloon();
    void onAnimationFinished();

signals:
    void openTransferManager(int tab);
    void dismissOQ(bool oq);
    void userActivity();

private:
    Ui::InfoDialog *ui;
    QPushButton *overlay;
#ifdef __APPLE__
    QPushButton *arrow;
    QWidget *dummy; // Patch to let text input on line edits of GuestWidget
#endif

    QMenu *transferMenu;
    DataUsageMenu *storageUsedMenu;

    MenuItemAction *cloudItem;
    MenuItemAction *inboxItem;
    MenuItemAction *sharesItem;
    MenuItemAction *rubbishItem;

    int activeDownloadState, activeUploadState;
    int remainingUploads, remainingDownloads;
    bool indexing;
    bool waiting;
    GuestWidget *gWidget;
    int state;
    bool overQuotaState;
    int storageState;

    QPropertyAnimation *animation;
    QGraphicsOpacityEffect *opacityEffect;

protected:
    void updateBlockedState();
    void updateState();
    void changeEvent(QEvent * event);
    bool eventFilter(QObject *obj, QEvent *e);
#ifdef __APPLE__
    void paintEvent( QPaintEvent * e);
    void hideEvent(QHideEvent *event);
#endif

protected:
    QDateTime lastPopupUpdate;
    QTimer downloadsFinishedTimer;
    QTimer uploadsFinishedTimer;
    QTimer transfersFinishedTimer;
    MegaApplication *app;
    Preferences *preferences;
    mega::MegaApi *megaApi;
    mega::MegaTransfer *activeDownload;
    mega::MegaTransfer *activeUpload;
};

#endif // INFODIALOG_H
