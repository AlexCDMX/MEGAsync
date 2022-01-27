#ifndef INFODIALOGTRANSFERDELEGATEWIDGET_H
#define INFODIALOGTRANSFERDELEGATEWIDGET_H

#include <QWidget>
#include <QFileInfo>
#include <QDateTime>
#include <QMenu>
#include "megaapi.h"
#include "TransferRemainingTime.h"
#include "TransferBaseDelegateWidget.h"

namespace Ui {
class InfoDialogTransferDelegateWidget;
}

class InfoDialogTransferDelegateWidget : public TransferBaseDelegateWidget
{
    Q_OBJECT

    static const QRect fullRect;
    static const QRect innerRect;

public:
    explicit InfoDialogTransferDelegateWidget(QWidget *parent = 0);

    ~InfoDialogTransferDelegateWidget();

    ActionHoverType mouseHoverTransfer(bool isHover, const QPoint &pos) override;

    void finishTransfer();

    void updateTransferState() override;
    void setFileNameAndType() override;
    void setType() override;

    void setFileType(const QString& fileName);
    QString getTransferName();

    bool mouseHoverRetryingLabel(QPoint pos);

    void updateFinishedTime();
    void loadDefaultTransferIcon() {}
    void updateAnimation() {}

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

signals:
    void copyTransferLink();
    void openTransferFolder();

private slots: 
    void on_lShowInFolder_clicked();
    void on_lActionTransfer_clicked();

private:
    Ui::InfoDialogTransferDelegateWidget *mUi;
    mega::MegaApi *mMegaApi;
    bool mIsHover;
    TransferRemainingTime mTransferRemainingTime;

    void updateFinishedIco(int transferType, int errorCode);
    void updateTransferActive(const QExplicitlySharedDataPointer<TransferData> data);
    void updateTransferCompletedOrFailed(const QExplicitlySharedDataPointer<TransferData> data);
    void updateTransferCompleting(const QExplicitlySharedDataPointer<TransferData> data);

protected:
    bool mActionButtonsEnabled;
};

#endif // INFODIALOGTRANSFERDELEGATEWIDGET_H
