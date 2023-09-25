
#include "BackupsModel.h"

#include "megaapi.h"
#include "Utilities.h"
#include "syncs/control/SyncController.h"
#include "syncs/control/SyncInfo.h"
#include "MegaApplication.h"
#include "qml/StandardIconProvider.h"

#include <QQmlContext>

BackupFolder::BackupFolder(const QString& folder,
                           const QString& displayName,
                           bool selected, QObject *parent)
    : QObject(parent)
    , mName(displayName)
    , mFolder(folder)
    , mSize(QString::fromUtf8(""))
    , mSelected(selected)
    , mDone(false)
    , mFolderSizeReady(false)
    , mError(0)
    , folderSize(FileFolderAttributes::NOT_READY)
    , sdkError(QString())
    , mFolderAttr(nullptr)
{
}

void BackupFolder::setSize(qint64 size)
{
    QVector<int> changedRoles;

    if(size != FileFolderAttributes::NOT_READY)
    {
        mFolderSizeReady = true;
        changedRoles.append(BackupsModel::SizeReadyRole);
    }
    if(size > FileFolderAttributes::NOT_READY)
    {
        folderSize = size;
        mSize = Utilities::getSizeStringLocalized(size);
        changedRoles.append(BackupsModel::SizeRole);
    }

    if(!changedRoles.isEmpty())
    {
        if(auto model = dynamic_cast<BackupsModel*>(parent()))
        {
            auto changedIndex = model->index(model->getRow(mFolder), 0);
            model->updateSelectedAndTotalSize();
            emit model->dataChanged(changedIndex, changedIndex, changedRoles);
        }
    }
}

void BackupFolder::calculateFolderSize()
{
    if(!mFolderAttr)
    {
        mFolderAttr = new LocalFileFolderAttributes(mFolder, this);
    }

    mFolderAttr->requestSize(this,[&](qint64 size)
                            {
                                setSize(size);
                            });

}

int BackupsModel::CHECK_DIRS_TIME = 1000;

BackupsModel::BackupsModel(QObject* parent)
    : QAbstractListModel(parent)
    , mRoleNames(QAbstractItemModel::roleNames())
    , mSelectedRowsTotal(0)
    , mBackupsTotalSize(0)
    , mTotalSizeReady(false)
    , mBackupsController(new BackupsController(this))
    , mConflictsSize(0)
    , mConflictsNotificationText(QString::fromUtf8(""))
    , mCheckAllState(Qt::CheckState::Unchecked)
    , mGlobalError(BackupErrorCode::None)
{
    mRoleNames[NameRole] = "mName";
    mRoleNames[FolderRole] = "mFolder";
    mRoleNames[SizeRole] = "mSize";
    mRoleNames[SizeReadyRole] = "mSizeReady";
    mRoleNames[SelectedRole] = "mSelected";
    mRoleNames[DoneRole] = "mDone";
    mRoleNames[ErrorRole] = "mError";

    // Append mBackupFolderList with the default dirs
    populateDefaultDirectoryList();

    connect(SyncInfo::instance(), &SyncInfo::syncRemoved,
            this, &BackupsModel::onSyncRemoved);
    connect(mBackupsController.get(), &BackupsController::backupsCreationFinished,
            this, &BackupsModel::onBackupsCreationFinished);
    connect(mBackupsController.get(), &BackupsController::backupFinished,
            this, &BackupsModel::onBackupFinished);
    connect(&mCheckDirsTimer, &QTimer::timeout, this, &BackupsModel::checkDirectories);

    MegaSyncApp->qmlEngine()->rootContext()->setContextProperty(QString::fromUtf8("BackupsModel"), this);
    MegaSyncApp->qmlEngine()->rootContext()->setContextProperty(QString::fromUtf8("BackupsController"),
                                                                mBackupsController.get());

    qmlRegisterUncreatableType<BackupsModel>("BackupsModel", 1, 0, "BackupErrorCode",
                                             QString::fromUtf8("Cannot create WarningLevel in QML"));

    MegaSyncApp->qmlEngine()->addImageProvider(QLatin1String("standardicons"), new StandardIconProvider);

    mCheckDirsTimer.setInterval(CHECK_DIRS_TIME);
    mCheckDirsTimer.start();
}

BackupsModel::~BackupsModel()
{
    if(auto engine = MegaSyncApp->qmlEngine())
    {
        engine->removeImageProvider(QLatin1String("standardicons"));
    }
}

QHash<int, QByteArray> BackupsModel::roleNames() const
{
    return mRoleNames;
}

int BackupsModel::rowCount(const QModelIndex& parent) const
{
    // When implementing a table based model, rowCount() should return 0 when the parent is valid.
    return parent.isValid() ? 0 : mBackupFolderList.size();
}

bool BackupsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    bool result = hasIndex(index.row(), index.column(), index.parent()) && value.isValid();

    if (result)
    {
        BackupFolder* item = mBackupFolderList[index.row()];
        switch (role)
        {
            case NameRole:
                item->mName = value.toString();
                break;
            case FolderRole:
                item->mFolder = value.toString();
                break;
            case SizeRole:
                item->mSize = value.toInt();
                break;
            case SelectedRole:
            {
                item->mSelected = value.toBool();
                checkSelectedAll();
                break;
            }
            case DoneRole:
                item->mDone = value.toBool();
                break;
            case ErrorRole:
                item->mError = value.toInt();
                break;
            default:
                result = false;
                break;
        }

        if(result)
        {
            emit dataChanged(index, index, { role } );
        }
    }

    return result;
}

QVariant BackupsModel::data(const QModelIndex &index, int role) const
{
    QVariant field;

    if (hasIndex(index.row(), index.column(), index.parent()))
    {
        const BackupFolder* item = mBackupFolderList.at(index.row());
        switch (role)
        {
            case NameRole:
                field = item->mName;
                break;
            case FolderRole:
                field = item->mFolder;
                break;
            case SizeRole:
                field = item->mSize;
                break;
            case SizeReadyRole:
                field = item->mFolderSizeReady;
                break;
            case SelectedRole:
                field = item->mSelected;
                break;
            case DoneRole:
                field = item->mDone;
                break;
            case ErrorRole:
                field = item->mError;
                break;
            default:
                break;
        }
    }

    return field;
}

QString BackupsModel::getTotalSize() const
{
    return Utilities::getSizeStringLocalized(mBackupsTotalSize);
}

bool BackupsModel::getIsTotalSizeReady() const
{
    return mTotalSizeReady;
}

Qt::CheckState BackupsModel::getCheckAllState() const
{
    return mCheckAllState;
}

void BackupsModel::setCheckAllState(Qt::CheckState state, bool fromModel)
{
    if(!fromModel && mCheckAllState == Qt::CheckState::Unchecked && state == Qt::CheckState::PartiallyChecked)
    {
        state = Qt::CheckState::Checked;
    }

    if(mCheckAllState == state)
    {
        return;
    }

    if(mCheckAllState != Qt::CheckState::Checked && state == Qt::CheckState::Checked)
    {
        setAllSelected(true);
    }
    else if(mCheckAllState != Qt::CheckState::Unchecked && state == Qt::CheckState::Unchecked)
    {
        setAllSelected(false);
    }
    mCheckAllState = state;
    emit checkAllStateChanged();
}

BackupsController* BackupsModel::backupsController() const
{
    return mBackupsController.get();
}

bool BackupsModel::getExistConflicts() const
{
    return mConflictsSize > 0;
}

QString BackupsModel::getConflictsNotificationText() const
{
    return mConflictsNotificationText;
}

int BackupsModel::getGlobalError() const
{
    return mGlobalError;
}

bool BackupsModel::existsOnlyGlobalError() const
{
    return mExistsOnlyGlobalError;
}

void BackupsModel::insert(const QString &folder)
{
    QString inputPath(QDir::toNativeSeparators(QDir(folder).absolutePath()));
    if(selectIfExistsInsertion(inputPath))
    {
        // If the folder exists in the table then select the item and return
        return;
    }

    BackupFolder* data = new BackupFolder(inputPath, mSyncController.getSyncNameFromPath(inputPath), true, this);
    beginInsertRows(QModelIndex(), 0, 0);
    mBackupFolderList.prepend(data);
    endInsertRows();
    checkSelectedAll();
}

void BackupsModel::setAllSelected(bool selected)
{
    QList<BackupFolder*>::iterator item = mBackupFolderList.end()-1;
    while (item != mBackupFolderList.begin()-1)
    {
        (*item)->mSelected = selected;
        emit dataChanged(getModelIndex(item), getModelIndex(item), { SelectedRole } );
        item--;
    }

    updateSelectedAndTotalSize();
}

void BackupsModel::populateDefaultDirectoryList()
{
    // Default directories definition
    QVector<QStandardPaths::StandardLocation> defaultPaths =
    {
        QStandardPaths::DesktopLocation,
        QStandardPaths::DocumentsLocation,
        QStandardPaths::MusicLocation,
        QStandardPaths::PicturesLocation
    };

    // Iterate defaultPaths to add to mBackupFolderList if the path is not empty
    for (auto type : qAsConst(defaultPaths))
    {
        const auto standardPaths (QStandardPaths::standardLocations(type));
        QDir dir (QDir::cleanPath(standardPaths.first()));
        QString path (QDir::toNativeSeparators(dir.canonicalPath()));
        if(dir.exists() && dir != QDir::home() && isLocalFolderSyncable(path))
        {
            BackupFolder* folder = new BackupFolder(path, mSyncController.getSyncNameFromPath(path), false, this);
            mBackupFolderList.append(folder);
        }
        else
        {
            mega::MegaApi::log(mega::MegaApi::LOG_LEVEL_WARNING,
                               QString::fromUtf8("Default path %1 is not valid.").arg(path).toUtf8().constData());
        }
    }
}

void BackupsModel::updateSelectedAndTotalSize()
{
    mSelectedRowsTotal = 0;
    auto lastTotalSize = mBackupsTotalSize;
    mBackupsTotalSize = 0;
    QList<BackupFolder*>::iterator item = mBackupFolderList.begin();
    while (item != mBackupFolderList.end())
    {
        if((*item)->mSelected)
        {
            mSelectedRowsTotal++;

            if(!(*item)->mFolderSizeReady)
            {
                setTotalSizeReady(false);
                return;
            }

            if((*item)->folderSize > 0)
            {
                mBackupsTotalSize += (*item)->folderSize;
            }
        }
        item++;
    }

    if(mBackupsTotalSize != lastTotalSize)
    {
        emit totalSizeChanged();
    }

    if(!mSelectedRowsTotal)
    {
        emit noneSelected();
    }
    setTotalSizeReady(true);
}

void BackupsModel::checkSelectedAll()
{
    updateSelectedAndTotalSize();

    Qt::CheckState state = Qt::CheckState::PartiallyChecked;
    if(mSelectedRowsTotal == 0)
    {
        state = Qt::CheckState::Unchecked;
    }
    else if(mSelectedRowsTotal == mBackupFolderList.size())
    {
        state = Qt::CheckState::Checked;
    }
    setCheckAllState(state, true);
}

bool BackupsModel::isLocalFolderSyncable(const QString& inputPath)
{
    QString message;
    return (SyncController::isLocalFolderSyncable(inputPath, mega::MegaSync::TYPE_BACKUP, message) != SyncController::CANT_SYNC);
}

bool BackupsModel::selectIfExistsInsertion(const QString& inputPath)
{
    bool exists = false;
    int row = 0;

    while (!exists && row < rowCount())
    {
        QString existingPath(mBackupFolderList[row]->mFolder);
        if ((exists = (inputPath == existingPath)))
        {
            if(!mBackupFolderList[row]->mSelected)
            {
                setData(index(row, 0), QVariant(true), SelectedRole);
            }
        }
        row++;
    }

    return exists;
}

bool BackupsModel::folderContainsOther(const QString& folder,
                                            const QString& other) const
{
    if(folder == other)
    {
        return true;
    }
    return folder.startsWith(other) && folder[other.size()] == QDir::separator();
}

bool BackupsModel::isRelatedFolder(const QString& folder,
                                        const QString& existingPath) const
{
    return folderContainsOther(folder, existingPath) || folderContainsOther(existingPath, folder);
}

QModelIndex BackupsModel::getModelIndex(QList<BackupFolder*>::iterator item)
{
    int row = static_cast<int>(std::distance(mBackupFolderList.begin(), item));
    return QModelIndex(index(static_cast<int>(row), 0));
}

void BackupsModel::checkDuplicatedBackupNames(const QSet<QString>& candidateSet,
                                              const QStringList& candidateList)
{
    // Search the repeated strings (backup names)
    int repeatedStrings = candidateList.count() - candidateSet.count();
    int repeatedCounter = 0;
    int row = -1;
    while(repeatedCounter < repeatedStrings && ++row < rowCount())
    {
        if (mBackupFolderList[row]->mSelected && !mBackupFolderList[row]->mDone)
        {
            QString current(mBackupFolderList[row]->mName);
            int i = row;
            bool isFirst = true;
            while(repeatedCounter < repeatedStrings && ++i < rowCount())
            {
                if (mBackupFolderList[i]->mSelected
                    && !mBackupFolderList[i]->mDone
                    && mBackupFolderList[i]->mError == BackupErrorCode::None
                    && mBackupFolderList[i]->mName == current)
                {
                    if(isFirst)
                    {
                        mBackupFolderList[row]->mError = BackupErrorCode::DuplicatedName;
                        isFirst = false;
                    }
                    mBackupFolderList[i]->mError = BackupErrorCode::DuplicatedName;
                    repeatedCounter++;
                }
            }
        }
    }
}

void BackupsModel::reviewConflicts()
{
    QList<BackupFolder*>::iterator item = mBackupFolderList.begin();
    QString firstRemoteNameConflict = QString::fromUtf8("");
    QString conflictText(QString::fromUtf8(""));
    int remoteCount = 0;
    int duplicatedCount = 0;
    int syncConflictCount = 0;
    int pathRelationCount = 0;
    int unavailableCount = 0;
    int sdkCount = 0;

    while (item != mBackupFolderList.end())
    {
        if((*item)->mSelected)
        {
            switch((*item)->mError)
            {
                case DuplicatedName:
                    duplicatedCount++;
                    break;
                case ExistsRemote:
                    if(firstRemoteNameConflict.isEmpty())
                    {
                        firstRemoteNameConflict = (*item)->mName;
                    }
                    remoteCount++;
                    break;
                case SyncConflict:
                    syncConflictCount++;
                    break;
                case PathRelation:
                    pathRelationCount++;
                    break;
                case UnavailableDir:
                    unavailableCount++;
                    break;
                case SDKCreation:
                    if(sdkCount == 0 && !(*item)->sdkError.isEmpty())
                    {
                        conflictText = (*item)->sdkError;
                    }
                    sdkCount++;
                    break;
                default:
                    break;
            }
        }
        item++;
    }

    mConflictsSize = duplicatedCount
                        + remoteCount
                        + syncConflictCount
                        + pathRelationCount
                        + unavailableCount;

    mExistsOnlyGlobalError = mConflictsSize == 0;
    emit existsOnlyGlobalErrorChanged();

    if(sdkCount > 0)
    {
        setGlobalError(BackupErrorCode::SDKCreation);
        if(conflictText.isEmpty())
        {
            if(sdkCount == 1)
            {
                conflictText = tr("Folder wasn't backed up. Try again.");
            }
            else
            {
                conflictText = tr("These folders weren't backed up. Try again.");
            }
        }
        changeConflictsNotificationText(conflictText);
        return;
    }

    if(syncConflictCount > 0)
    {
        // The conflict text has been changed in existOtherRelatedFolder method
        setGlobalError(BackupErrorCode::SyncConflict);
        return;
    }

    if(duplicatedCount > 0)
    {
        conflictText = tr("You can't back up folders with the same name. "
                          "Rename them to continue with the backup. "
                          "Folder names won't change on your computer.");
        setGlobalError(BackupErrorCode::DuplicatedName);
    }
    else if(remoteCount == 1)
    {
        conflictText = tr("A folder named \"%1\" already exists in your Backups. "
                          "Rename the new folder to continue with the backup. "
                          "Folder name will not change on your computer.").arg(firstRemoteNameConflict);
        setGlobalError(BackupErrorCode::ExistsRemote);
    }
    else if(remoteCount > 1)
    {
        conflictText = tr("Some folders with the same name already exist in your Backups. "
                          "Rename the new folders to continue with the backup. "
                          "Folder names will not change on your computer.");
        setGlobalError(BackupErrorCode::ExistsRemote);
    }
    else if(pathRelationCount > 0)
    {
        conflictText = tr("Backup folders can't contain or be contained by other backup folder");
        setGlobalError(BackupErrorCode::PathRelation);
    }
    else if(unavailableCount > 0)
    {
        conflictText = tr("Folder can't be backed up as it can't be located. "
                          "It may have been moved or deleted, or you might not have access.");
        setGlobalError(BackupErrorCode::UnavailableDir);
    }
    changeConflictsNotificationText(conflictText);
}

void BackupsModel::changeConflictsNotificationText(const QString& text)
{
    if(mConflictsNotificationText.isEmpty())
    {
        mConflictsNotificationText = text;
        emit existConflictsChanged();
    }
}

void BackupsModel::checkRemoteDuplicatedBackups(const QSet<QString>& candidateSet)
{
    QSet<QString> duplicatedSet = mBackupsController->getRemoteFolders();
    duplicatedSet.intersect(candidateSet);
    if(!duplicatedSet.isEmpty())
    {
        auto backup = duplicatedSet.begin();
        while(backup != duplicatedSet.end())
        {
            int row = -1;
            bool found = false;
            while(!found && ++row < rowCount())
            {
                if ((found = mBackupFolderList[row]->mSelected && mBackupFolderList[row]->mName == *backup)
                    && !mBackupFolderList[row]->mDone
                    && mBackupFolderList[row]->mError == BackupErrorCode::None)
                {
                        mBackupFolderList[row]->mError = BackupErrorCode::ExistsRemote;
                }
            }
            backup++;
        }
    }
}

bool BackupsModel::existOtherRelatedFolder(const int currentRow)
{
    if(currentRow > mBackupFolderList.size())
    {
        return false;
    }

    bool found = false;
    QString folder(mBackupFolderList[currentRow]->mFolder);
    int conflictRow = 1;
    while(!found && conflictRow < rowCount())
    {
        if((found = mBackupFolderList[conflictRow]->mSelected
            && conflictRow!=currentRow &&isRelatedFolder(folder, mBackupFolderList[conflictRow]->mFolder)))
        {
            mBackupFolderList[currentRow]->mError = BackupErrorCode::PathRelation;
            mBackupFolderList[conflictRow]->mError = BackupErrorCode::PathRelation;
        }
        conflictRow++;
    }
    return found;
}

void BackupsModel::check()
{
    // Clean errors
    for (int row = 0; row < rowCount(); row++)
    {
        if (mBackupFolderList[row]->mSelected
            && mBackupFolderList[row]->mError != BackupErrorCode::SDKCreation)
        {
            mBackupFolderList[row]->mError = BackupErrorCode::None;
        }
    }

    mConflictsNotificationText = QString::fromUtf8("");
    mGlobalError = BackupErrorCode::None;

    QStringList candidateList;
    for (int row = 0; row < rowCount(); row++)
    {
        if (mBackupFolderList[row]->mSelected && !mBackupFolderList[row]->mDone)
        {
            candidateList.append(mBackupFolderList[row]->mName);
            QString message;
            if (mBackupFolderList[row]->mError == BackupErrorCode::None
                    && !existOtherRelatedFolder(row)
                && SyncController::isLocalFolderSyncable(mBackupFolderList[row]->mFolder, mega::MegaSync::TYPE_BACKUP, message)
                        != SyncController::CAN_SYNC)
            {
                mBackupFolderList[row]->mError = BackupErrorCode::SyncConflict;
                changeConflictsNotificationText(message);
            }
        }
    }

    QSet<QString> candidateSet = QSet<QString>::fromList(candidateList);
    checkRemoteDuplicatedBackups(candidateSet);

    if (candidateSet.count() < candidateList.count())
    {
        checkDuplicatedBackupNames(candidateSet, candidateList);
    }

    reviewConflicts();

    // Change final errors
    for (int row = 0; row < rowCount(); row++)
    {
        if (mBackupFolderList[row]->mSelected)
        {
            emit dataChanged(index(row, 0), index(row, 0), { ErrorRole } );
        }
    }

    if(mConflictsNotificationText.isEmpty())
    {
        emit existConflictsChanged();
    }

    if(mGlobalError == BackupErrorCode::None)
    {
        emit globalErrorChanged();
    }
}


int BackupsModel::getRow(const QString& folder)
{
    int row = 0;
    bool found = false;
    while(!found && row < rowCount())
    {
        found = mBackupFolderList[row]->mFolder == folder;
        if (!found)
        {
            row++;
        }
    }

    return row;
}

void BackupsModel::calculateFolderSizes()
{
    for(auto& backupFolder : mBackupFolderList)
    {
        if(backupFolder->mSelected)
        {
            backupFolder->calculateFolderSize();
        }
    }
}

int BackupsModel::rename(const QString& folder, const QString& name)
{
    int row = getRow(folder);
    QString originalName = mBackupFolderList[row]->mName;
    bool hasError = false;
    QSet<QString> candidateSet;
    candidateSet.insert(name);

    QSet<QString> duplicatedSet = mBackupsController->getRemoteFolders();
    duplicatedSet.intersect(candidateSet);

    if(!duplicatedSet.isEmpty())
    {
        mBackupFolderList[row]->mError = BackupErrorCode::ExistsRemote;
        hasError = true;
    }

    if(!hasError)
    {
        int i = -1;
        while(!hasError && ++i < rowCount())
        {
            hasError = (i != row && mBackupFolderList[row]->mSelected && name == mBackupFolderList[i]->mName);
            if (hasError)
            {
                mBackupFolderList[row]->mError = BackupErrorCode::DuplicatedName;
            }
        }
    }

    if(!hasError)
    {
        setData(index(row, 0), QVariant(name), NameRole);
        check();
    }
    else
    {
        mBackupFolderList[row]->mName = originalName;
    }

    return mBackupFolderList[row]->mError;
}

void BackupsModel::remove(const QString& folder)
{
    QList<BackupFolder*>::iterator item = mBackupFolderList.begin();
    bool found = false;
    QString name;
    while (!found && item != mBackupFolderList.end())
    {
        if((found = (*item)->mFolder == folder))
        {
            name = (*item)->mName;
            const auto row = std::distance(mBackupFolderList.begin(), item);
            beginRemoveRows(QModelIndex(), row, row);
            item = mBackupFolderList.erase(item);
            endRemoveRows();
        }
        else
        {
            item++;
        }
    }

    if(found)
    {
        check();
        checkSelectedAll();
    }
}

bool BackupsModel::existsFolder(const QString& inputPath)
{
    bool exists = false;
    QList<BackupFolder*>::iterator item = mBackupFolderList.begin();
    while (!exists && item != mBackupFolderList.end())
    {
        exists = (inputPath == (*item)->mFolder);
        if(!exists)
        {
            item++;
        }
    }
    return exists;
}

void BackupsModel::change(const QString& oldFolder, const QString& newFolder)
{
    if(oldFolder == newFolder)
    {
        return;
    }

    QList<BackupFolder*>::iterator item = mBackupFolderList.begin();
    bool found = false;
    while (!found && item != mBackupFolderList.end())
    {
        if((*item)->mSelected && (found = ((*item)->mFolder == oldFolder)))
        {
            if(existsFolder(newFolder))
            {
                remove(newFolder);
            }
            setData(index(getRow(oldFolder), 0), QVariant(mSyncController.getSyncNameFromPath(newFolder)), NameRole);
            setData(index(getRow(oldFolder), 0), QVariant(newFolder), FolderRole);

            if((*item)->mError == BackupErrorCode::SDKCreation)
            {
                (*item)->mError = BackupErrorCode::None;
            }

            checkSelectedAll();
            check();
        }
        item++;
    }
}

void BackupsModel::onSyncRemoved(std::shared_ptr<SyncSettings> syncSettings)
{
    Q_UNUSED(syncSettings);
    check();
}

void BackupsModel::clean(bool resetErrors)
{
    QList<BackupFolder*>::iterator item = mBackupFolderList.begin();
    while (item != mBackupFolderList.end())
    {
        if((*item)->mSelected)
        {
            if((*item)->mDone)
            {
                const auto row = std::distance(mBackupFolderList.begin(), item);
                beginRemoveRows(QModelIndex(), row, row);
                item = mBackupFolderList.erase(item);
                endRemoveRows();
            }
            else
            {
                if(resetErrors)
                {
                    setData(index(getRow((*item)->mFolder), 0), QVariant(BackupErrorCode::None), ErrorRole);
                }
                item++;
            }
        }
        else
        {
            item++;
        }
    }
}

void BackupsModel::setGlobalError(BackupErrorCode error)
{
    mGlobalError = error;
    emit globalErrorChanged();
}

void BackupsModel::setTotalSizeReady(bool ready)
{
    if(mTotalSizeReady != ready)
    {
        mTotalSizeReady = ready;
        emit totalSizeReadyChanged();
    }
}

void BackupsModel::onBackupsCreationFinished(bool success)
{
    if(success)
    {
        clean();
    }
    else
    {
        mConflictsNotificationText.clear();
        reviewConflicts();
    }
}

void BackupsModel::onBackupFinished(const QString& folder,
                                    bool done,
                                    const QString& sdkError)
{
    int row = getRow(folder);
    if(done)
    {
        setData(index(row, 0), QVariant(true), DoneRole);
    }
    else
    {
        mBackupFolderList[row]->sdkError = sdkError;
        setData(index(row, 0), QVariant(BackupErrorCode::SDKCreation), ErrorRole);
    }
}

bool BackupsModel::checkDirectories()
{
    bool success = true;
    bool reviewErrors = false;
    for (int row = 0; row < rowCount(); row++)
    {
        if (mBackupFolderList[row]->mSelected
                && !mBackupFolderList[row]->mDone
                && mBackupFolderList[row]->mError != SDKCreation)
        {
            if(!QDir(mBackupFolderList[row]->mFolder).exists())
            {
                setData(index(row, 0), QVariant(BackupErrorCode::UnavailableDir), ErrorRole);
                success = false;
            }
            else if(mBackupFolderList[row]->mError == BackupErrorCode::UnavailableDir)
            {
                // If actual error is UnavailableDir and it could be located again
                // Then, clean this error
                setData(index(row, 0), QVariant(BackupErrorCode::None), ErrorRole);
                reviewErrors = true;
            }
        }
    }

    if(reviewErrors)
    {
        // If one or more UnavailableDir errors have been reverted
        // Then we need to check all the conflicts again
        check();
    }
    else
    {
        // Only review the global conflict
        reviewConflicts();
    }

    return success;
}

// ************************************************************************************************
// * BackupsProxyModel
// ************************************************************************************************

BackupsProxyModel::BackupsProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , mSelectedFilterEnabled(false)
{
    setSourceModel(new BackupsModel(this));
    setDynamicSortFilter(true);
}

bool BackupsProxyModel::selectedFilterEnabled() const
{
    return mSelectedFilterEnabled;
}

void BackupsProxyModel::setSelectedFilterEnabled(bool enabled)
{
    if(mSelectedFilterEnabled == enabled) {
        return;
    }

    if(enabled)
    {
        if(auto model = dynamic_cast<BackupsModel*>(sourceModel()))
        {
            model->calculateFolderSizes();
        }
    }
    mSelectedFilterEnabled = enabled;
    emit selectedFilterEnabledChanged();

    invalidateFilter();
}

bool BackupsProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if(!mSelectedFilterEnabled) {
        return true;
    }

    const QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    return index.data(BackupsModel::BackupFolderRoles::SelectedRole).toBool();
}

void BackupsProxyModel::createBackups()
{
    if(!backupsModel()->checkDirectories())
    {
        return;
    }

    // All expected errors have been handled
    BackupsController::BackupInfoList candidateList;
    for (int row = 0; row < rowCount(); row++)
    {
        if(!index(row, 0).data(BackupsModel::BackupFolderRoles::DoneRole).toBool())
        {
            BackupsController::BackupInfo candidate;
            candidate.first = index(row, 0).data(BackupsModel::BackupFolderRoles::FolderRole).toString();
            candidate.second = index(row, 0).data(BackupsModel::BackupFolderRoles::NameRole).toString();
            candidateList.append(candidate);
        }
    }
    backupsModel()->backupsController()->addBackups(candidateList);
}

BackupsModel* BackupsProxyModel::backupsModel()
{
    return dynamic_cast<BackupsModel*>(sourceModel());
}
