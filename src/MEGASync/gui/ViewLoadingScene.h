#ifndef VIEWLOADINGSCENE_H
#define VIEWLOADINGSCENE_H

#include <QWidget>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QTimer>
#include <QPainter>

class LoadingSceneDelegateBase : public QStyledItemDelegate
{
    Q_OBJECT

    const double MIN_OPACITY = 0.3;
    const double OPACITY_STEPS = 0.05;
    const double MAX_OPACITY = 1.0;

public:
    explicit LoadingSceneDelegateBase(QAbstractItemView* view) : mView(view), mOpacitySteps(OPACITY_STEPS)
    {
        connect(&mTimer, &QTimer::timeout, this, &LoadingSceneDelegateBase::onLoadingTimerTimeout);
    }

    inline void setLoading(bool state)
    {
        updateTimer(state);
        mOpacity = MAX_OPACITY;

        mView->update();
    }

protected:
    inline void updateTimer(bool state)
    {
        state ? mTimer.start(100) : mTimer.stop();
    }

    inline double getOpacity() const
    {
        return mOpacity;
    }

    inline QAbstractItemView* getView() const
    {
        return mView;
    }

private slots:
    void onLoadingTimerTimeout()
    {
        if(mOpacity < MIN_OPACITY)
        {
            mOpacitySteps = OPACITY_STEPS;
            mOpacity = MIN_OPACITY;
        }
        else if(mOpacity > MAX_OPACITY)
        {
            mOpacitySteps = -OPACITY_STEPS;
            mOpacity = MAX_OPACITY;
        }
        else
        {
            mOpacity += mOpacitySteps;
        }

        mView->update();
    }

private:
    QTimer mTimer;
    double mOpacity;
    double mOpacitySteps;
    QAbstractItemView* mView;
};

template <class DelegateWidget>
class LoadingSceneDelegate : public LoadingSceneDelegateBase
{
public:
    explicit LoadingSceneDelegate(QAbstractItemView* view) : LoadingSceneDelegateBase(view)
    {}

    inline QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const
    {
        return DelegateWidget::sizeHint();
    }

protected:
    inline void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        painter->fillRect(option.rect, Qt::white);

        auto pos (option.rect.topLeft());
        auto width (option.rect.width());
        auto height (option.rect.height());

        auto loadingItem = getLoadingWidget(index,option.rect.size());

        if(!loadingItem)
        {
            return;
        }

        // Move if position changed
        if (loadingItem->pos() != pos)
        {
            loadingItem->move(pos);
        }

        // Resize if window resized
        if (loadingItem->width() != width)
        {
            loadingItem->resize(width, height);
        }

        painter->save();

        painter->setOpacity(getOpacity());
        painter->translate(pos);
        loadingItem->render(painter,QPoint(0,0), QRegion(0, 0, width, height));

        painter->restore();
    }

private:
    inline DelegateWidget* getLoadingWidget(const QModelIndex &index, const QSize &size) const
    {
        auto nbRowsMaxInView(1);
        if(size.height() > 0)
        {
            nbRowsMaxInView = getView()->height() / size.height() + 1;
        }
        auto row (index.row() % nbRowsMaxInView);

        DelegateWidget* item(nullptr);

        if(row >= mLoadingItems.size())
        {
           item = new DelegateWidget(getView());
           mLoadingItems.append(item);
        }
        else
        {
            item = mLoadingItems.at(row);
        }

        return item;
    }

    mutable QVector<DelegateWidget*> mLoadingItems;
};


template <class DelegateWidget>
class ViewLoadingScene
{
    const uint8_t MAX_LOADING_ROWS = 20;

public:
    ViewLoadingScene() :
        mLoadingModel(nullptr),
        mLoadingDelegate(nullptr),
        mViewDelegate(nullptr),
        mView(nullptr),
        mViewModel(nullptr)
    {}

    inline void setView(QAbstractItemView* view)
    {
        mView = view;
        mViewDelegate = view->itemDelegate();
        mViewModel = view->model();
    }

    inline void setLoadingScene(bool state)
    {
        if(!mView)
        {
            return;
        }

        if(!mLoadingModel)
        {
            mLoadingModel = new QStandardItemModel(mView);
            mLoadingDelegate = new LoadingSceneDelegate<DelegateWidget>(mView);
        }

        if(state)
        {
            auto modelRowCount = mViewModel->rowCount();
            auto rows = modelRowCount < MAX_LOADING_ROWS && modelRowCount > 0 ? modelRowCount : MAX_LOADING_ROWS;
            for(int row = 0; row < rows; ++row)
            {
                mLoadingModel->appendRow(new QStandardItem());
            }

            mView->setModel(mLoadingModel);
            mView->setItemDelegate(mLoadingDelegate);
            mView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
        else
        {
            mLoadingModel->setRowCount(0);
            mView->setModel(mViewModel);
            mView->setItemDelegate(mViewDelegate);
            mView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        }

        mLoadingDelegate->setLoading(state);
        mView->update();
    }

private:
    LoadingSceneDelegate<DelegateWidget>* mLoadingDelegate;
    QStandardItemModel* mLoadingModel;
    QAbstractItemDelegate* mViewDelegate;
    QAbstractItemView* mView;
    QAbstractItemModel* mViewModel;
};

#endif // VIEWLOADINGSCENE_H
