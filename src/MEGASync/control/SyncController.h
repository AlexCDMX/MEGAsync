#pragma once

#include "model/SyncSettings.h"
#include "model/SyncModel.h"
#include "QTMegaRequestListener.h"

#include "megaapi.h"

#include <QString>
#include <QDir>

/**
 * @brief Sync Controller class
 *
 * Interface object used to control Syncs and report back on results using Qt Signals.
 * Uses SyncModel.h class as the data model.
 *
 */
class SyncController: public QObject, public mega::MegaRequestListener
{
    Q_OBJECT

public:

    enum Syncability
    {
       CAN_SYNC = 0,
       WARN_SYNC,
       CANT_SYNC,
    };

    SyncController(QObject* parent = nullptr);
    ~SyncController();

    void addBackup(const QString& localFolder, const QString& syncName = QString());
    void addSync(const QString &localFolder, const mega::MegaHandle &remoteHandle,
                 const QString& syncName = QString(), mega::MegaSync::SyncType type = mega::MegaSync::TYPE_TWOWAY);
    void removeSync(std::shared_ptr<SyncSetting> syncSetting, const mega::MegaHandle& remoteHandle = mega::INVALID_HANDLE);
    void enableSync(std::shared_ptr<SyncSetting> syncSetting);
    void disableSync(std::shared_ptr<SyncSetting> syncSetting);

    void setMyBackupsDirName();
    void getMyBackupsHandle();
    QString getMyBackupsLocalizedPath();

    // Local folder checks
    static QString getIsLocalFolderAlreadySyncedMsg(const QString& path, const mega::MegaSync::SyncType& syncType);
    static Syncability isLocalFolderAlreadySynced(const QString& path, const mega::MegaSync::SyncType& syncType, QString& message);
    static QString getIsLocalFolderAllowedForSyncMsg(const QString& path, const mega::MegaSync::SyncType& syncType);
    static Syncability isLocalFolderAllowedForSync(const QString& path, const mega::MegaSync::SyncType& syncType, QString& message);
    static QString getAreLocalFolderAccessRightsOkMsg(const QString& path, const mega::MegaSync::SyncType& syncType);
    static Syncability areLocalFolderAccessRightsOk(const QString& path, const mega::MegaSync::SyncType& syncType, QString& message);
    static Syncability isLocalFolderSyncable(const QString& path, const mega::MegaSync::SyncType& syncType, QString& message);

    // Remote folder checks
    static QString getIsRemoteFolderAlreadySyncedMsg(std::shared_ptr<mega::MegaNode> node);
    static Syncability isRemoteFolderAlreadySynced(std::shared_ptr<mega::MegaNode> node, QString& message);
    static QString getIsRemoteFolderAllowedForSyncMsg(std::shared_ptr<mega::MegaNode> node);
    static Syncability isRemoteFolderAllowedForSync(std::shared_ptr<mega::MegaNode> node, QString& message);
    static QString getAreRemoteFolderAccessRightsOkMsg(std::shared_ptr<mega::MegaNode> node);
    static Syncability areRemoteFolderAccessRightsOk(std::shared_ptr<mega::MegaNode> node, QString& message);
    static Syncability isRemoteFolderSyncable(std::shared_ptr<mega::MegaNode> node, QString& message);

    static QString getSyncNameFromPath(const QString& path);

    static const char* DEFAULT_BACKUPS_ROOT_DIRNAME;

signals:
    void syncAddStatus(int errorCode, QString errorMsg, QString name);
    void syncRemoveError(std::shared_ptr<SyncSetting> sync);
    void syncEnableError(std::shared_ptr<SyncSetting> sync);
    void syncDisableError(std::shared_ptr<SyncSetting> sync);

    void setMyBackupsStatus(int errorCode, QString errorMsg);
    void myBackupsHandle(mega::MegaHandle handle);

protected:
    // override from MegaRequestListener
    void onRequestFinish(mega::MegaApi* api, mega::MegaRequest* req, mega::MegaError* e) override;

private:
    void setMyBackupsHandle(mega::MegaHandle handle);
    QString getSyncAPIErrorMsg(int megaError);
    QString getSyncTypeString(const mega::MegaSync::SyncType& syncType);

    mega::MegaApi* mApi;
    mega::QTMegaRequestListener* mDelegateListener;
    SyncModel* mSyncModel;
    mega::MegaHandle mMyBackupsHandle;
};
