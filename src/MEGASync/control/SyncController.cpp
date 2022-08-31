#include "SyncController.h"
#include "MegaApplication.h"
#include "Platform.h"

#include <QStorageInfo>
#include <QTemporaryFile>

using namespace mega;

const char* SyncController::DEFAULT_BACKUPS_ROOT_DIRNAME = "My backups";

SyncController::SyncController(QObject* parent)
    : QObject(parent),
      mApi(MegaSyncApp->getMegaApi()),
      mDelegateListener (new QTMegaRequestListener(mApi, this)),
      mSyncModel(SyncModel::instance()),
      mMyBackupsHandle(INVALID_HANDLE)
{
    // The controller shouldn't ever be instantiated before we have an API and a SyncModel available
    assert(mApi);
    assert(mSyncModel);
}

SyncController::~SyncController()
{
    delete mDelegateListener;
}

void SyncController::addBackup(const QString& localFolder, const QString& syncName)
{
    addSync(QDir::toNativeSeparators(localFolder), mega::INVALID_HANDLE,
            syncName.isEmpty() ? getSyncNameFromPath(localFolder) : syncName,
            mega::MegaSync::TYPE_BACKUP);
}

void SyncController::addSync(const QString& localFolder, const MegaHandle& remoteHandle,
                             const QString& syncName, MegaSync::SyncType type)
{
    MegaApi::log(MegaApi::LOG_LEVEL_INFO, QString::fromUtf8("Adding sync (%1) \"%2\" for path \"%3\"")
                 .arg(getSyncTypeString(type), syncName, localFolder).toUtf8().constData());

    mApi->syncFolder(type, localFolder.toUtf8().constData(),
                     syncName.isEmpty() ? nullptr : syncName.toUtf8().constData(),
                     remoteHandle, nullptr, mDelegateListener);
}

void SyncController::removeSync(std::shared_ptr<SyncSetting> syncSetting, const MegaHandle& remoteHandle)
{
    if (!syncSetting)
    {
        MegaApi::log(MegaApi::LOG_LEVEL_ERROR, QString::fromUtf8("Removing invalid sync").toUtf8().constData());
        return;
    }

    MegaApi::log(MegaApi::LOG_LEVEL_INFO, QString::fromUtf8("Removing sync (%1) \"%2\"")
                 .arg(getSyncTypeString(syncSetting->getType()), syncSetting->name()).toUtf8().constData());

    mApi->removeSync(syncSetting->backupId(), remoteHandle, mDelegateListener);
    // FIXME: There is a bug in SyncModel class handling that persists the saved Backup entry after SDK delete
}

void SyncController::enableSync(std::shared_ptr<SyncSetting> syncSetting)
{
    if (!syncSetting)
    {
        MegaApi::log(MegaApi::LOG_LEVEL_ERROR, QString::fromUtf8("Trying to enable null sync").toUtf8().constData());
        return;
    }

    MegaApi::log(MegaApi::LOG_LEVEL_INFO, QString::fromUtf8("Enabling sync (%1) \"%2\" to \"%3\"")
                 .arg(getSyncTypeString(syncSetting->getType()), syncSetting->getLocalFolder(), syncSetting->getMegaFolder())
                 .toUtf8().constData());

    mApi->enableSync(syncSetting->backupId(), mDelegateListener);
}

void SyncController::disableSync(std::shared_ptr<SyncSetting> syncSetting)
{
    if (!syncSetting)
    {
        MegaApi::log(MegaApi::LOG_LEVEL_ERROR, QString::fromUtf8("Trying to disable invalid sync").toUtf8().constData());
        return;
    }

    MegaApi::log(MegaApi::LOG_LEVEL_INFO, QString::fromUtf8("Disabling sync (%1) \"%2\" to \"%3\"")
                 .arg(getSyncTypeString(syncSetting->getType()), syncSetting->getLocalFolder(), syncSetting->getMegaFolder())
                 .toUtf8().constData());

    mApi->disableSync(syncSetting->backupId(), mDelegateListener);
}

// Checks if a path belongs is in an existing sync or backup tree; and if the selected
// folder has a sync or backup in its tree.
QString SyncController::getIsLocalFolderAlreadySyncedMsg(const QString& path, const MegaSync::SyncType& syncType)
{
    QString inputPath (QDir::toNativeSeparators(QDir(path).absolutePath()));
    QString message;

    // Gather all synced or backed-up dirs
    QMap<QString, MegaSync::SyncType> localFolders = SyncModel::instance()->getLocalFoldersAndTypeMap();

    // Check if the path is already synced or part of a sync
    foreach (auto& existingPath, localFolders.keys())
    {
        if (inputPath == existingPath)
        {
            if (syncType == MegaSync::SyncType::TYPE_BACKUP)
            {
                message = localFolders.value(existingPath) == MegaSync::SyncType::TYPE_TWOWAY ?
                            tr("You can't backup this folder as it's already synced.")
                          : tr("Folder is already backed up. Select a different one.");
            }
            else
            {
                message = localFolders.value(existingPath) == MegaSync::SyncType::TYPE_TWOWAY ?
                            tr("You can't sync this folder as it's already synced.")
                          : tr("You can't sync this folder as it's already backed up.");
            }
        }
        else if (inputPath.startsWith(existingPath)
                 && inputPath[existingPath.size() - QDir(existingPath).isRoot()] == QDir::separator())
        {
            if (syncType == MegaSync::SyncType::TYPE_BACKUP)
            {
                message = localFolders.value(existingPath) == MegaSync::SyncType::TYPE_TWOWAY ?
                            tr("You can't backup this folder as it's already inside a synced folder.")
                          : tr("You can't backup this folder as it's already inside a backed up folder.");
            }
            else
            {
                message = localFolders.value(existingPath) == MegaSync::SyncType::TYPE_TWOWAY ?
                            tr("You can't sync folders that are inside synced folders.")
                          : tr("You can't sync folders that are inside backed up folders.");
            }
        }
        else if (existingPath.startsWith(inputPath)
                 && existingPath[inputPath.size() - QDir(inputPath).isRoot()] == QDir::separator())
        {
            if (syncType == MegaSync::SyncType::TYPE_BACKUP)
            {
                message = localFolders.value(existingPath) == MegaSync::SyncType::TYPE_TWOWAY ?
                            tr("You can't backup this folder as it contains synced folders.")
                          : tr("You can't backup this folder as it contains backed up folders.");
            }
            else
            {
                message = localFolders.value(existingPath) == MegaSync::SyncType::TYPE_TWOWAY ?
                            tr("You can't sync folders that contain synced folders.")
                          : tr("You can't sync folders that contain backed up folders.");
            }
        }
    }
    return message;
}

SyncController::Syncability SyncController::isLocalFolderAlreadySynced(const QString& path, const MegaSync::SyncType &syncType, QString& message)
{
    message = getIsLocalFolderAlreadySyncedMsg(path, syncType);
    return (message.isEmpty() ? Syncability::CAN_SYNC : Syncability::CANT_SYNC);
}

QString SyncController::getIsLocalFolderAllowedForSyncMsg(const QString& path, const MegaSync::SyncType& syncType)
{
    QString inputPath (path);
    QString message;

#ifdef WIN32
    if (inputPath.startsWith(QString::fromLatin1("\\\\?\\")))
    {
        inputPath = inputPath.mid(4);
    }
#endif

    // Use canonicalPath() to resolve links
    inputPath = QDir(inputPath).canonicalPath();

    if (inputPath == QDir::rootPath())
    {
        if (syncType == MegaSync::SyncType::TYPE_BACKUP)
        {
            message = tr("You are trying to backup an extremely large folder.\n"
                         "To prevent the backup of entire boot volumes,"
                         " which is inefficient and dangerous,\n"
                         "we ask you to start with a smaller folder"
                         " and add more data while MEGAsync is running.");
        }
        else
        {
            message = tr("You are trying to sync an extremely large folder.\n"
                         "To prevent the syncing of entire boot volumes,"
                         " which is inefficient and dangerous,\n"
                         "we ask you to start with a smaller folder"
                         " and add more data while MEGAsync is running.");
        }
    }
    return message;
}

SyncController::Syncability SyncController::isLocalFolderAllowedForSync(const QString& path, const MegaSync::SyncType &syncType, QString& message)
{
    message = getIsLocalFolderAllowedForSyncMsg(path, syncType);
    return (message.isEmpty() ? Syncability::CAN_SYNC : Syncability::CANT_SYNC);
}

QString SyncController::getAreLocalFolderAccessRightsOkMsg(const QString& path, const mega::MegaSync::SyncType& syncType)
{
    QString message;

    // We only check rw rights for two-way syncs
    if (syncType == MegaSync::TYPE_TWOWAY)
    {
        QTemporaryFile test (path + QDir::separator());
        if (!QDir(path).mkpath(QString::fromLatin1(".")) && !test.open())
        {
            message = tr("You don't have write permissions in this local folder.")
                    + QChar::fromLatin1('\n')
                    + tr("MEGAsync won't be able to download anything here.");
        }
    }
    return message;
}

SyncController::Syncability SyncController::areLocalFolderAccessRightsOk(const QString& path, const mega::MegaSync::SyncType& syncType, QString& message)
{
    message = getAreLocalFolderAccessRightsOkMsg(path, syncType);
    return (message.isEmpty() ? Syncability::CAN_SYNC : Syncability::WARN_SYNC);
}

// Returns wether the path is syncable.
// The message to display to the user is stored in <message>.
// The first error encountered is returned.
// Errors trump warnings
// In case of several warnings, only the last one is returned.
SyncController::Syncability SyncController::isLocalFolderSyncable(const QString& path, const mega::MegaSync::SyncType& syncType, QString& message)
{
    Syncability syncability (Syncability::CAN_SYNC);

    // First check if the path is allowed
    syncability = isLocalFolderAllowedForSync(path, syncType, message);

    // The check if it is not synced already
    if (syncability != Syncability::CANT_SYNC)
    {
        syncability = std::max(isLocalFolderAlreadySynced(path, syncType, message), syncability);
    }

    // Then check that we have rw rights for this path
    if (syncability != Syncability::CANT_SYNC)
    {
        syncability = std::max(areLocalFolderAccessRightsOk(path, syncType, message), syncability);
    }

    return (syncability);
}

// Checks if a path belongs is in an existing sync or backup tree; and if the selected
// folder has a sync in its tree.
// This method only considers two-way syncs.
QString SyncController::getIsRemoteFolderAlreadySyncedMsg(std::shared_ptr<mega::MegaNode> node)
{
    QString message;
    QChar pathSep (QLatin1Char('/'));
    auto api (MegaSyncApp->getMegaApi());

    auto path (QString::fromUtf8(api->getNodePath(node.get())));

    if (!path.isEmpty())
    {
        // Gather all synced or backed-up dirs
        const auto remoteFolders = SyncModel::instance()->getMegaFolders(mega::MegaSync::TYPE_TWOWAY);

        // Check if the path is already synced or part of a sync
        auto existingPathIt (remoteFolders.cbegin());
        while (message.isEmpty() && existingPathIt != remoteFolders.cend())
        {
            if (path == *existingPathIt)
            {
                message = tr("The selected MEGA folder is already synced");
            }
            else if (path.startsWith(*existingPathIt)
                     && path[existingPathIt->size() - existingPathIt->endsWith(pathSep)] == pathSep)
            {
                message = tr("Folder contents already synced");
            }
            else if (existingPathIt->startsWith(path)
                     && (*existingPathIt)[path.size() - path.endsWith(pathSep)] == pathSep)
            {
                message = tr("Folder already synced");
            }
            existingPathIt++;
        }
    }
    else
    {
        message = tr("Invalid remote path");
    }
    return message;
}

// This method only considers two-way syncs.
SyncController::Syncability SyncController::isRemoteFolderAlreadySynced(std::shared_ptr<mega::MegaNode> node, QString& message)
{
    message = getIsRemoteFolderAlreadySyncedMsg(node);
    return (message.isEmpty() ? Syncability::CAN_SYNC : Syncability::CANT_SYNC);
}

// This method only considers two-way syncs.
QString SyncController::getIsRemoteFolderAllowedForSyncMsg(std::shared_ptr<mega::MegaNode> node)
{
    QString message;
    auto api (MegaSyncApp->getMegaApi());

    if (!(node && node->isFolder()
                && !api->isInRubbish(node.get())
                && !api->isInVault(node.get())))
    {
        message = tr("Invalid remote path");
    }

    return message;
}

// This method only considers two-way syncs.
SyncController::Syncability SyncController::isRemoteFolderAllowedForSync(std::shared_ptr<mega::MegaNode> node, QString& message)
{
    message = getIsRemoteFolderAllowedForSyncMsg(node);
    return (message.isEmpty() ? Syncability::CAN_SYNC : Syncability::CANT_SYNC);
}

// This method only considers two-way syncs.
QString SyncController::getAreRemoteFolderAccessRightsOkMsg(std::shared_ptr<mega::MegaNode> node)
{
    QString message;
    auto api (MegaSyncApp->getMegaApi());

    if (node)
    {
        if (api->isInShare(node.get())
                && (api->getAccess(node.get()) < mega::MegaShare::ACCESS_FULL))
        {
            message = tr("You don't have enough permissions for this remote folder");
        }
    }
    else
    {
        message = tr("Invalid remote path");
    }
    return message;
}

// This method only considers two-way syncs.
SyncController::Syncability SyncController::areRemoteFolderAccessRightsOk(std::shared_ptr<mega::MegaNode> node, QString& message)
{
    message = getAreRemoteFolderAccessRightsOkMsg(node);
    return (message.isEmpty() ? Syncability::CAN_SYNC : Syncability::CANT_SYNC);
}

// Returns wether the path is syncable.
// The message to display to the user is stored in <message>.
// The first error encountered is returned.
// Errors trump warnings
// In case of several warnings, only the last one is returned.
// This method only considers two-way syncs.
SyncController::Syncability SyncController::isRemoteFolderSyncable(std::shared_ptr<mega::MegaNode> node, QString& message)
{
    Syncability syncability (Syncability::CAN_SYNC);

    // First check if the path is allowed
    syncability = isRemoteFolderAllowedForSync(node, message);

    // The check if it is not synced already
    if (syncability != Syncability::CANT_SYNC)
    {
        syncability = std::max(isRemoteFolderAlreadySynced(node, message), syncability);
    }

    // Then check that we have rw rights for this path
    if (syncability != Syncability::CANT_SYNC)
    {
        syncability = std::max(areRemoteFolderAccessRightsOk(node, message), syncability);
    }

    return (syncability);
}

QString SyncController::getSyncNameFromPath(const QString& path)
{
    QDir dir (path);
    QString syncName;

    // Handle fs root case
    if (dir.isRoot())
    {
        // Cleanup the path (in Windows: get "F:" from "F:\")
        QString cleanPath (QDir::toNativeSeparators(dir.absolutePath()).remove(QDir::separator()));
        // Try to get the volume label
        QStorageInfo storage(dir);
        QString label (QString::fromUtf8(storage.subvolume()));
        if (label.isEmpty())
        {
            label = storage.name();
        }
        // If we have no label, fallback to the cleaned path
        syncName = label.isEmpty() ? cleanPath
                                   : QString::fromUtf8("%1 (%2)").arg(label, cleanPath);
    }
    else
    {
        // Take the folder name as sync name
        syncName = dir.dirName();
    }

    return syncName;
}

QString SyncController::getSyncAPIErrorMsg(int megaError)
{
    switch (megaError)
    {
        case MegaError::API_EARGS:
            return tr("Unable to create backup as selected folder is not valid. Try again.");
        break;
        case MegaError::API_EACCESS:
            return tr("Unable to create backup. Try again and if issue continues, contact [A]Support[/A].");
        break;
        case MegaError::API_EINCOMPLETE:
            return tr("Unable to create backup as the device you're backing up from doesn't have a name. "
                     "Give your device a name and then try again. If issue continues, contact [A]Support[/A].");
        case MegaError::API_EINTERNAL:
        // Fallthrough
        case MegaError::API_ENOENT:
        // Fallthrough
        case MegaError::API_EEXIST:
            return tr("Unable to create backup. For further information, contact [A]Support[/A].");
        default:
            break;
    }
    return QString();
}

QString SyncController::getSyncTypeString(const mega::MegaSync::SyncType& syncType)
{
    QString typeString;
    switch (syncType)
    {
        case MegaSync::SyncType::TYPE_TWOWAY:
        {
            typeString = QLatin1String("Two-way");
            break;
        }
        case MegaSync::SyncType::TYPE_BACKUP:
        {
            typeString = QLatin1String("Backup");
            break;
        }
        case MegaSync::SyncType::TYPE_UP:
        {
            typeString = QLatin1String("One-way: up");
            break;
        }
        case MegaSync::SyncType::TYPE_DOWN:
        {
            typeString = QLatin1String("One-way: down");
            break;
        }
        case MegaSync::SyncType::TYPE_UNKNOWN:
        default:
        {
            typeString = QLatin1String("Unknown");
            break;
        }
    }
    return typeString;
}

void SyncController::setMyBackupsDirName()
{
    QString name = QApplication::translate("MegaNodeNames", SyncController::DEFAULT_BACKUPS_ROOT_DIRNAME);
    mApi->setMyBackupsFolder(name.toUtf8().constData(), mDelegateListener);
}

void SyncController::getMyBackupsHandle()
{
    if(mMyBackupsHandle == INVALID_HANDLE)
        mApi->getUserAttribute(MegaApi::USER_ATTR_MY_BACKUPS_FOLDER, mDelegateListener);
    else
        emit myBackupsHandle(mMyBackupsHandle);
}

void SyncController::setMyBackupsHandle(MegaHandle handle)
{
    mMyBackupsHandle = handle;
    emit myBackupsHandle(mMyBackupsHandle);
}

// The path looks like "/My backups" (but translated), without the "/Backups" root
// Note: if the node exists, its name is ignored and we always display the localized version,
// as per requirements.
QString SyncController::getMyBackupsLocalizedPath()
{
    return QLatin1Char('/') + QApplication::translate("MegaNodeNames", SyncController::DEFAULT_BACKUPS_ROOT_DIRNAME);
}

void SyncController::onRequestFinish(MegaApi *api, MegaRequest *req, MegaError *e)
{
    int errorCode (e->getErrorCode());

    switch(req->getType())
    {
    case MegaRequest::TYPE_ADD_SYNC:
    {
        int syncErrorCode (req->getNumDetails());
        QString errorMsg;

        bool error = false;

        if (syncErrorCode != MegaSync::NO_SYNC_ERROR)
        {
            errorMsg = QCoreApplication::translate("MegaSyncError", MegaSync::getMegaSyncErrorCode(syncErrorCode));
            error = true;
        }
        else if (errorCode != MegaError::API_OK)
        {
            errorMsg = getSyncAPIErrorMsg(errorCode);
            if(errorMsg.isEmpty())
                errorMsg = QCoreApplication::translate("MegaError", e->getErrorString());
            error = true;
        }

        if(error)
        {
            std::shared_ptr<MegaNode> remoteNode(api->getNodeByHandle(req->getNodeHandle()));
            QString logMsg = QString::fromUtf8("Error adding sync (%1) \"%2\" for \"%3\" to \"%4\" (request error): %5").arg(
                             getSyncTypeString(static_cast<MegaSync::SyncType>(req->getParamType())),
                             QString::fromUtf8(req->getName()),
                             QString::fromUtf8(req->getFile()),
                             QString::fromUtf8(api->getNodePath(remoteNode.get())),
                             errorMsg);
            MegaApi::log(MegaApi::LOG_LEVEL_ERROR, logMsg.toUtf8().constData());
        }
        emit syncAddStatus(errorCode, errorMsg, QString::fromUtf8(req->getFile()));
        break;
    }
    case MegaRequest::TYPE_REMOVE_SYNC:
    {
        if (errorCode != MegaError::API_OK)
        {
            QString errorMsg = getSyncAPIErrorMsg(errorCode);
            if(errorMsg.isEmpty())
                errorMsg = QCoreApplication::translate("MegaError", e->getErrorString());

            std::shared_ptr<SyncSetting> sync = mSyncModel->getSyncSettingByTag(req->getParentHandle());
            QString logMsg = QString::fromUtf8("Error removing sync (%1) (request error): %2").arg(
                                 getSyncTypeString(sync ? sync->getType() : MegaSync::SyncType::TYPE_UNKNOWN),
                                 errorMsg);
            MegaApi::log(MegaApi::LOG_LEVEL_ERROR, logMsg.toUtf8().constData());
            if(sync)
                emit syncRemoveError(sync);
        }
        break;
    }
    case MegaRequest::TYPE_DISABLE_SYNC:
    {
        if (errorCode == MegaError::API_OK)
            break;

        int syncErrorCode (req->getNumDetails());
        std::shared_ptr<SyncSetting> sync = mSyncModel->getSyncSettingByTag(req->getParentHandle());

        if (sync && (syncErrorCode != MegaSync::NO_SYNC_ERROR))
        {
            QString errorMsg = QString::fromUtf8("Error disabling sync (%1) \"%2\" for \"%3\" to \"%4\": %5").arg(
                        getSyncTypeString(sync->getType()),
                        sync->name(),
                        sync->getLocalFolder(),
                        sync->getMegaFolder(),
                        QCoreApplication::translate("MegaSyncError", MegaSync::getMegaSyncErrorCode(syncErrorCode)));

            MegaApi::log(MegaApi::LOG_LEVEL_ERROR, errorMsg.toUtf8().constData());
            emit syncDisableError(sync);
        }
        else
        {
            QString errorMsg = getSyncAPIErrorMsg(errorCode);
            if(errorMsg.isEmpty())
                errorMsg = QCoreApplication::translate("MegaError", e->getErrorString());

            QString logMsg = QString::fromUtf8("Error disabling sync (%1) (request error): %2").arg(
                                 getSyncTypeString(sync ? sync->getType() : MegaSync::SyncType::TYPE_UNKNOWN),
                                 errorMsg);
            MegaApi::log(MegaApi::LOG_LEVEL_ERROR, logMsg.toUtf8().constData());
        }
        break;
    }
    case MegaRequest::TYPE_ENABLE_SYNC:
    {
        if (errorCode == MegaError::API_OK)
            break;

        int syncErrorCode (req->getNumDetails());
        std::shared_ptr<SyncSetting> sync = mSyncModel->getSyncSettingByTag(req->getParentHandle());

        if (sync && (syncErrorCode != MegaSync::NO_SYNC_ERROR))
        {
            QString errorMsg = QString::fromUtf8("Error enabling sync (%1) \"%2\" for \"%3\" to \"%4\": %5").arg(
                        getSyncTypeString(sync->getType()),
                        sync->name(),
                        sync->getLocalFolder(),
                        sync->getMegaFolder(),
                        QCoreApplication::translate("MegaSyncError", MegaSync::getMegaSyncErrorCode(syncErrorCode)));

            MegaApi::log(MegaApi::LOG_LEVEL_ERROR, errorMsg.toUtf8().constData());
            emit syncEnableError(sync);
        }
        else
        {
            QString errorMsg = getSyncAPIErrorMsg(errorCode);
            if(errorMsg.isEmpty())
                errorMsg = QCoreApplication::translate("MegaError", e->getErrorString());

            QString logMsg = QString::fromUtf8("Error enabling sync (%1) (request reason): %2").arg(
                                 getSyncTypeString(sync ? sync->getType() : MegaSync::SyncType::TYPE_UNKNOWN),
                                 errorMsg);
            MegaApi::log(MegaApi::LOG_LEVEL_ERROR, logMsg.toUtf8().constData());
        }

        // TODO: Evaluate if I'm needed
        if (!req->getNumDetails() && sync)
        {
            mSyncModel->removeUnattendedDisabledSync(sync->backupId(), sync->getType());
        }
        break;
    }
    case MegaRequest::TYPE_MOVE:
    {
        if (e->getErrorCode() != MegaError::API_OK)
        {
            QString errorMsg = getSyncAPIErrorMsg(errorCode);
            if(errorMsg.isEmpty())
                errorMsg = QCoreApplication::translate("MegaError", e->getErrorString());
            MegaApi::log(MegaApi::LOG_LEVEL_ERROR, QString::fromUtf8("Error trashing MEGA folder (request error): %1")
                         .arg(errorMsg).toUtf8().constData());
        }
        break;
    }
    case MegaRequest::TYPE_SET_MY_BACKUPS:
    {
        QString errorMsg;
        if (errorCode == MegaError::API_OK)
        {
            setMyBackupsHandle(req->getNodeHandle());
            MegaApi::log(MegaApi::LOG_LEVEL_INFO, "MyBackups folder set successfully");
        }
        else
        {
            errorMsg = QString::fromUtf8(e->getErrorString());
            QString logMsg (QString::fromUtf8("Error setting MyBackups folder: \"%1\"").arg(errorMsg));
            MegaApi::log(MegaApi::LOG_LEVEL_ERROR, logMsg.toUtf8().constData());
            errorMsg = QCoreApplication::translate("MegaError", errorMsg.toUtf8().constData());
        }
        emit setMyBackupsStatus(errorCode, errorMsg);
        break;
    }
    case MegaRequest::TYPE_GET_ATTR_USER:
    {
        int subCommand (req->getParamType());
        if (subCommand == MegaApi::USER_ATTR_MY_BACKUPS_FOLDER)
        {
            MegaHandle handle = INVALID_HANDLE;
            if (errorCode == MegaError::API_OK)
            {
                handle = req->getNodeHandle();
                MegaApi::log(MegaApi::LOG_LEVEL_INFO, "Got MyBackups folder from remote");
            }
            else
            {
                MegaApi::log(MegaApi::LOG_LEVEL_ERROR,
                             QString::fromUtf8("Error getting MyBackups folder: \"%1\"")
                             .arg(QString::fromUtf8(e->getErrorString()))
                             .toUtf8().constData());
            }
            setMyBackupsHandle(handle);
        }
        break;
    }
    }
}
