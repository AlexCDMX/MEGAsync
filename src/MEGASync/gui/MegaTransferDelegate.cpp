#include "MegaTransferDelegate.h"
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QMessageBox>
#include <QToolTip>
#include "control/Utilities.h"
#include "Preferences.h"
#include "gui/QMegaMessageBox.h"
#include "megaapi.h"
#include "QTransfersModel.h"
#include "MegaApplication.h"
#include "platform/Platform.h"

using namespace mega;

MegaTransferDelegate::MegaTransferDelegate(QTransfersModel *model, QObject *parent)
    : QStyledItemDelegate(parent)
{
    this->model = model;
}

void MegaTransferDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid())
    {
        if (option.state & QStyle::State_Selected)
        {
            painter->fillRect(option.rect, QColor(247, 247, 247));
        }

        int tag = index.internalId();
        int modelType = model->getModelType();
        TransferItem *ti = model->transferItems[tag];
        if (!ti)
        {
            if (static_cast<MegaApplication*>(qApp)->megaApiLock)
            {
                // We will call the SDK several times, and if there is one new transfer there may be many, and the SDK may be working a lot.  
                // Here we lock the SDK mutex so we can make all these calls back into it in one go, and get the painting done.  
                // The lock will be released at the end of the infoDialog or transferDialog paintEvent().
                static_cast<MegaApplication*>(qApp)->megaApiLock->lockOnce();
            }

            if (modelType == QTransfersModel::TYPE_CUSTOM_TRANSFERS)
            {
                ti = new CustomTransferItem();
            }
            else
            {
                ti = new TransferManagerItem();
            }

            ti->setTransferTag(tag);
            connect(ti, SIGNAL(refreshTransfer(int)), model, SLOT(refreshTransferItem(int)));
            model->transferItems.insert(tag, ti);
            MegaTransfer *transfer = model->getTransferByTag(tag);

            if (transfer)
            {
                //Check if transfer finishes while the account was blocked, in order to provide the right context for failed error
                bool blockedTransfer = static_cast<MegaApplication*>(qApp)->finishedTransfersWhileBlocked(transfer->getTag());
                if (blockedTransfer)
                {
                    ti->setTransferFinishedWhileBlocked(blockedTransfer);
                    static_cast<MegaApplication*>(qApp)->removeFinishedBlockedTransfer(transfer->getTag());
                }

                ti->setType(transfer->getType(), transfer->isSyncTransfer());
                ti->setFileName(QString::fromUtf8(transfer->getFileName()));
                ti->setTotalSize(transfer->getTotalBytes());
                ti->setSpeed(transfer->getSpeed(), transfer->getMeanSpeed());
                ti->setTransferredBytes(transfer->getTransferredBytes(), !transfer->isSyncTransfer());           
                ti->setPriority(transfer->getPriority());

                int tError = transfer->getLastError().getErrorCode();
                if (tError != MegaError::API_OK)
                {
                    ti->setTransferError(tError, transfer->getLastError().getValue());
                }

                ti->setTransferState(transfer->getState());

                if (ti->isTransferFinished())
                {
                    if (transfer->getUpdateTime() != ti->getFinishedTime())
                    {
                        ti->setFinishedTime(transfer->getUpdateTime());
                        ti->updateFinishedTime(); // applies styles which can be slow - just do it when the finished time changes
                    }

                    MegaNode *node = transfer->getPublicMegaNode();
                    if (node && node->isPublic())
                    {
                        ti->setIsLinkAvailable(true);
                    }
                    else
                    {
                        MegaNode *ownNode = ((MegaApplication*)qApp)->getMegaApi()->getNodeByHandle(transfer->getNodeHandle());
                        if (ownNode)
                        {
                            int access = ((MegaApplication*)qApp)->getMegaApi()->getAccess(ownNode);
                            if (access == MegaShare::ACCESS_OWNER)
                            {
                                ti->setIsLinkAvailable(true);
                            }

                            ti->setNodeAccess(access);
                            delete ownNode;
                        }
                    }
                    delete node;
                }

                delete transfer;
            }            
        }
        else
        {
            if (!ti->isTransferFinished())
            {
                ti->updateTransfer();
            }
            else
            {
                ti->updateFinishedTime();
            }
        }

        Preferences *preferences = Preferences::instance();
        if (ti->getType() == MegaTransfer::TYPE_DOWNLOAD)
        {
            if (preferences->getDownloadsPaused())
            {
                if (modelType == QTransfersModel::TYPE_DOWNLOAD)
                {
                    ti->setStateLabel(tr("paused"));
                    ti->loadDefaultTransferIcon();
                }
                else if (modelType == QTransfersModel::TYPE_CUSTOM_TRANSFERS)
                {
                    ti->setStateLabel(tr("PAUSED"));
                }
            }
            else
            {
                ti->updateAnimation();
            }
        }
        else if (ti->getType() == MegaTransfer::TYPE_UPLOAD)
        {
            if (preferences->getUploadsPaused())
            {
               if (modelType == QTransfersModel::TYPE_UPLOAD)
               {
                   ti->setStateLabel(tr("paused"));
                   ti->loadDefaultTransferIcon();
               }
               else if (modelType == QTransfersModel::TYPE_CUSTOM_TRANSFERS)
               {
                   ti->setStateLabel(tr("PAUSED"));
               }
            }
            else
            {
                ti->updateAnimation();
            }
        }

        painter->save();
        painter->translate(option.rect.topLeft());

        if (modelType == QTransfersModel::TYPE_CUSTOM_TRANSFERS) //If custom transfer model, we should adjust width in case of vertical scrollbar is shown
        {
            ti->resize(option.rect.width(), option.rect.height());
        }

        ti->render(painter, QPoint(0, 0), QRegion(0, 0, option.rect.width(), option.rect.height()));
        painter->restore();
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize MegaTransferDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid())
    {
        int modelType = model->getModelType();
        if (modelType == QTransfersModel::TYPE_CUSTOM_TRANSFERS)
        {
            return QSize(400, 60);
        }

        return QSize(800, 48);
    }
    else
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

void MegaTransferDelegate::processCancel(int tag)
{
    if (model->getModelType() == QTransfersModel::TYPE_FINISHED)
    {
        model->removeTransferByTag(tag);
    }
    else
    {
        QPointer<QTransfersModel> modelPointer = model;

        QMessageBox warning;
        HighDpiResize hDpiResizer(&warning);
        warning.setWindowTitle(QString::fromUtf8("MEGAsync"));
        warning.setText(tr("Are you sure you want to cancel this transfer?"));
        warning.setIcon(QMessageBox::Warning);
        warning.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        warning.setDefaultButton(QMessageBox::No);
        int result = warning.exec();
        if (modelPointer && result == QMessageBox::Yes)
        {
            model->megaApi->cancelTransferByTag(tag);
        }
    }
}

bool MegaTransferDelegate::editorEvent(QEvent *event, QAbstractItemModel *, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (QEvent::MouseButtonPress ==  event->type())
    {
        int tag = index.internalId();
        TransferItem *item = model->transferItems[tag];
        if (!item)
        {
            return true;
        }

        if (item->cancelButtonClicked(((QMouseEvent *)event)->pos() - option.rect.topLeft()))
        {
            processCancel(tag);
            return true; // click consumed
        }
        else if (item && item->checkIsInsideButton(((QMouseEvent *)event)->pos() - option.rect.topLeft(), TransferItem::ACTION_BUTTON))
        {
            int modelType = model->getModelType();
            if (modelType == QTransfersModel::TYPE_FINISHED
                    || modelType == QTransfersModel::TYPE_CUSTOM_TRANSFERS)
            {
                const auto &ti = model->transferItems[tag];
                if (modelType == QTransfersModel::TYPE_CUSTOM_TRANSFERS && !ti->getIsLinkAvailable() && !ti->getTransferError())
                {
                    processShowInFolder(tag);
                }
                else if (MegaTransfer *transfer = model->getTransferByTag(tag) )
                {
                    if (!transfer->getLastError().getErrorCode())
                    {
                        QList<MegaHandle> exportList;
                        QStringList linkList;

                        MegaNode *node = transfer->getPublicMegaNode();
                        if (!node || !node->isPublic())
                        {
                            exportList.push_back(transfer->getNodeHandle());
                        }
                        else
                        {
                            char *handle = node->getBase64Handle();
                            char *key = node->getBase64Key();
                            if (handle && key)
                            {
                                QString link = Preferences::BASE_URL + QString::fromUtf8("/#!%1!%2")
                                        .arg(QString::fromUtf8(handle)).arg(QString::fromUtf8(key));
                                linkList.append(link);
                            }
                            delete [] handle;
                            delete [] key;
                        }
                        delete node;

                        if (exportList.size() || linkList.size())
                        {
                            ((MegaApplication*)qApp)->exportNodes(exportList, linkList);
                        }
                    }
                    else
                    {
                        ((MegaApplication*)qApp)->getMegaApi()->retryTransfer(transfer);
                    }

                    delete transfer;
                }
             }
             return true; // click consumed
        }
        else if (item && item->checkIsInsideButton(((QMouseEvent *)event)->pos() - option.rect.topLeft(), TransferItem::SHOW_IN_FOLDER_BUTTON))
        {
            if (model->getModelType() == QTransfersModel::TYPE_CUSTOM_TRANSFERS)
            {
                processShowInFolder(tag);
                return true; // click consumed
            }
            // we are not consuming the click; fall through to do the usual thing (of selecting the clicked row)
        }
    }
    else if (QEvent::MouseButtonDblClick == event->type() && model->getModelType() == QTransfersModel::TYPE_FINISHED)
    {
        int tag = index.internalId();
        processShowInFolder(tag);
        return true; // double-click consumed
    }

    return QAbstractItemDelegate::editorEvent(event, model, option, index);
}

bool MegaTransferDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::ToolTip)
    {
        int tag = index.internalId();
        TransferItem *item = model->transferItems[tag];
        if (item)
        {
            if (item->checkIsInsideButton(event->pos() - option.rect.topLeft(), TransferItem::ACTION_BUTTON))
            {
                int modelType = model->getModelType();

                if (modelType == QTransfersModel::TYPE_CUSTOM_TRANSFERS)
                {
                    const auto &ti = model->transferItems[tag];

                    if (!ti->getIsLinkAvailable() && !ti->getTransferError())
                    {
                        QToolTip::showText(event->globalPos(), tr("Show in folder"));
                    }
                    else if (MegaTransfer *transfer = model->getTransferByTag(tag) )
                    {
                        if (!transfer->getLastError().getErrorCode())
                        {
                            QToolTip::showText(event->globalPos(), tr("Get link"));
                        }
                        else
                        {
                            QToolTip::showText(event->globalPos(), tr("Retry"));
                        }
                        delete transfer;
                        return true;
                    }
                }
            }
            else if (item->checkIsInsideButton(event->pos() - option.rect.topLeft(), TransferItem::SHOW_IN_FOLDER_BUTTON))
            {
                int modelType = model->getModelType();
                if (modelType == QTransfersModel::TYPE_CUSTOM_TRANSFERS)
                {
                    QToolTip::showText(event->globalPos(), tr("Show in folder"));
                    return true;
                }
            }

            QString fileName = item->getFileName();
            if (fileName != item->getTransferName())
            {
                QToolTip::showText(event->globalPos(), fileName);
                return true;
            }
        }
    }
    return QStyledItemDelegate::helpEvent(event, view, option, index);
}

void MegaTransferDelegate::processShowInFolder(int tag)
{
    MegaTransfer *transfer = NULL;
    transfer = model->getTransferByTag(tag);
    if (transfer && transfer->getState() == MegaTransfer::STATE_COMPLETED
                 && transfer->getPath())
    {
        QString localPath = QString::fromUtf8(transfer->getPath());
        #ifdef WIN32
        if (localPath.startsWith(QString::fromAscii("\\\\?\\")))
        {
            localPath = localPath.mid(4);
        }
        #endif
        Platform::showInFolder(localPath);
    }
    delete transfer;
}
