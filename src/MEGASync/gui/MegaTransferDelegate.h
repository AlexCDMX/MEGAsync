#ifndef MEGATRANSFERDELEGATE_H
#define MEGATRANSFERDELEGATE_H

#include "TransferItem.h"
#include "QTransfersModel.h"

#include <QStyledItemDelegate>
#include <QAbstractItemView>

class TransfersSortFilterProxyModel;
class TransferBaseDelegateWidget;

class MegaTransferDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    MegaTransferDelegate(TransfersSortFilterProxyModel* model,  QAbstractItemView* view);
    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const;

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool event(QEvent *event) override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) override;

protected slots:
    void onHoverLeave(const QModelIndex& index, const QRect& rect);
    void onHoverEnter(const QModelIndex& index, const QRect& rect);
    void onHoverMove(const QModelIndex& index, const QRect& rect, const QPoint& point);

signals:
    void transferPaused(const TransferTag tag);
    void transferCanceled(const TransferTag tag);

private:
    TransferBaseDelegateWidget *getTransferItemWidget(const QModelIndex &index, const QSize &size) const;

    TransfersSortFilterProxyModel* mProxyModel;
    QTransfersModel* mSourceModel;
    mutable QVector<TransferBaseDelegateWidget*> mTransferItems;
    QAbstractItemView* mView;
};

#endif // MEGATRANSFERDELEGATE_H
