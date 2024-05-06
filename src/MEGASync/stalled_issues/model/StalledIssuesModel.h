#ifndef STALLEDISSUESMODEL_H
#define STALLEDISSUESMODEL_H

#include "QTMegaRequestListener.h"
#include "QTMegaGlobalListener.h"
#include "StalledIssue.h"
#include "StalledIssuesUtilities.h"
#include "ViewLoadingScene.h"
#include <MoveOrRenameCannotOccurIssue.h>
#include <StalledIssuesFactory.h>
#include "QMegaMessageBox.h"

#include <QObject>
#include <QReadWriteLock>
#include <QAbstractItemModel>
#include <QTimer>
#include <QPointer>

class LoadingSceneMessageHandler;
class NameConflictedStalledIssue;

class StalledIssuesReceiver : public QObject, public mega::MegaRequestListener
{
    Q_OBJECT
public:
    explicit StalledIssuesReceiver(QObject* parent = nullptr);
    ~StalledIssuesReceiver(){}

    template <class ISSUE_TYPE>
    void registerAutoRefreshDetector(QPointer<AutoRefreshByConditionBase> autoRefreshDetector)
    {
        if(mAutoRefreshDetector)
        {
            mAutoRefreshDetector->remove();
        }
        mAutoRefreshDetector = autoRefreshDetector;
        mAutoRefreshDetector->refresh();
    }

public slots:
    void onSetIsEventRequest();

signals:
    void stalledIssuesReady(StalledIssuesVariantList);
    void solvingIssues(int issueCount, int total);
    void moveOrRenameCannotOccurFound();

protected:
    void onRequestFinish(::mega::MegaApi*, ::mega::MegaRequest* request, ::mega::MegaError*);

private:
    QMutex mCacheMutex;
    StalledIssuesVariantList mStalledIssues;
    std::atomic_bool mIsEventRequest { false };
    StalledIssuesCreator mIssueCreator;
    QPointer<AutoRefreshByConditionBase> mAutoRefreshDetector;
};

class StalledIssuesModel : public QAbstractItemModel, public mega::MegaGlobalListener
{
    Q_OBJECT

public:
    static const int ADAPTATIVE_HEIGHT_ROLE;

    explicit StalledIssuesModel(QObject* parent = 0);
    ~StalledIssuesModel();

    virtual Qt::DropActions supportedDropActions() const override;
    bool hasChildren(const QModelIndex& parent) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    StalledIssueVariant getStalledIssueByRow(int row) const;
    QModelIndex parent(const QModelIndex& index) const override;
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    //Removes the pending and solved issues
    void fullReset();

    bool isEmpty() const;

    int getCountByFilterCriterion(StalledIssueFilterCriterion criterion);

    void finishStalledIssues(const QModelIndexList& indexes);
    void updateStalledIssues();

    void blockUi();
    void unBlockUi();

    void updateIndex(const QModelIndex& index);

    QModelIndexList getIssuesByReason(QList<mega::MegaSyncStall::SyncStallReason> reasons);
    QModelIndexList getIssues(std::function<bool (const std::shared_ptr<const StalledIssue>)> checker);

    //SHOW RAW INFO
    void showRawInfo(bool state);
    bool isRawInfoVisible() const;

    //ISSUE USE FOR UI ITEM
    void UiItemUpdate(const QModelIndex& oldIndex, const QModelIndex& newIndex);

    //SOLVE PROBLEMS
    void stopSolvingIssues();

    //Solve all issues
    void solveAllIssues();

    bool checkForExternalChanges(const QModelIndex& index);

    //Name conflicts
    bool solveLocalConflictedNameByRemove(int conflictIndex, const QModelIndex& index);
    bool solveLocalConflictedNameByRename(const QString& renameTo, int conflictIndex, const QModelIndex& index);

    bool solveCloudConflictedNameByRemove(int conflictIndex, const QModelIndex& index);
    bool solveCloudConflictedNameByRename(const QString& renameTo, int conflictIndex, const QModelIndex& index);

    void finishConflictManually();

    void semiAutoSolveNameConflictIssues(const QModelIndexList& list, int option);

    //LocalOrRemoteConflicts
    void chooseRemoteForBackups(const QModelIndexList& list);
    void chooseSideManually(bool remote, const QModelIndexList& list);
    void chooseBothSides(const QModelIndexList& list);
    void semiAutoSolveLocalRemoteIssues(const QModelIndexList& list);

    //IgnoreConflicts
    void ignoreItems(const QModelIndexList& list, bool isSymLink);
    void ignoreSymLinks();
    void showIgnoreItemsError(bool allFailed);

    //Fingerprint missing
    void fixFingerprint(const QModelIndexList& list);

    //MoveOrRename issue
    void fixMoveOrRenameCannotOccur(const QModelIndex& index, MoveOrRenameIssueChosenSide side);

    bool issuesRequested() const;

signals:
    void stalledIssuesChanged();
    void stalledIssuesCountChanged();
    void stalledIssuesReceived();

    void uiBlocked();
    void uiUnblocked();

    void setIsEventRequest();

    void showRawInfoChanged();

    void updateLoadingMessage(std::shared_ptr<MessageInfo> message);

    void refreshFilter();

protected slots:
    void onGlobalSyncStateChanged(mega::MegaApi* api) override;
    void onNodesUpdate(mega::MegaApi*, mega::MegaNodeList* nodes) override;

private slots:
    void onProcessStalledIssues(StalledIssuesVariantList issuesReceived);
    void onSendEvent();

private:
    void runMessageBox(QMegaMessageBox::MessageBoxInfo info);

    void removeRows(QModelIndexList& indexesToRemove);
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    void updateStalledIssuedByOrder();
    void reset();
    QModelIndex getSolveIssueIndex(const QModelIndex& index);
    void quitReceiverThread();

    bool checkIfUserStopSolving();
    void startSolvingIssues();
    void finishSolvingIssues(int issuesFixed, bool sendMessage = true, const QString& message = QString());

    void sendFixingIssuesMessage(int issue, int totalIssues);

    struct SolveListInfo
    {
        SolveListInfo(const QModelIndexList& uIndexes, std::function<bool(int)> uSolveFunc)
            : indexes(uIndexes),
            solveFunc(uSolveFunc)
        {
            Q_ASSERT(solveFunc);
        }

        bool async = false;
        QModelIndexList indexes;
        std::function<bool(int)> solveFunc = nullptr;
        std::function<void ()> startFunc = nullptr;
        std::function<void (int, bool)> finishFunc = nullptr;
        QString solveMessage;
    };

    void solveListOfIssues(const SolveListInfo& info);
    void issueSolved(const StalledIssueVariant &issue);
    
    StalledIssuesModel(const StalledIssuesModel&) = delete;
    void operator=(const StalledIssuesModel&) = delete;
    
    QThread* mStalledIssuesThread;
    StalledIssuesReceiver* mStalledIssuesReceiver;
    std::atomic_bool mThreadFinished { false };
    mega::QTMegaRequestListener* mRequestListener;
    mega::QTMegaGlobalListener* mGlobalListener;
    mega::MegaApi* mMegaApi;
    std::atomic_bool mIssuesRequested {false};
    bool mIsStalled;

    mutable QReadWriteLock mModelMutex;

    mutable StalledIssuesVariantList mStalledIssues;
    mutable StalledIssuesVariantList mSolvedStalledIssues;
    mutable StalledIssueVariant mLastSolvedStalledIssue;
    mutable QHash<const StalledIssue*, int> mStalledIssuesByOrder;

    QHash<int, int> mCountByFilterCriterion;

    QTimer mEventTimer;
    bool mRawInfoVisible;

    std::atomic_bool mSolvingIssues {false};
    std::atomic_bool mIssuesSolved {false};
    std::atomic_bool mSolvingIssuesStopped {false};

    //SyncDisable for backups
    QList<std::shared_ptr<SyncSettings>> mSyncsToDisable;

    //Ignored items
    QMap<mega::MegaHandle, QStringList> mIgnoredItemsBySync;
    
    //Fix fingerprint
    QList<StalledIssueVariant> mFingerprintIssuesToFix;
    FingerprintMissingSolver mFingerprintIssuesSolver;
};

#endif // STALLEDISSUESMODEL_H
