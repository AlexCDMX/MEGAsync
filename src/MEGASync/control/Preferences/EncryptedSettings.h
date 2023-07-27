#ifndef ENCRYPTEDSETTINGS_H
#define ENCRYPTEDSETTINGS_H

#include <QSettings>
#include <QVariant>
#include <QStringList>
#include <QCryptographicHash>

class EncryptedSettings : protected QSettings
{
    Q_OBJECT

public:
    explicit EncryptedSettings(QString file);

    void setValue(const QString & key, const QVariant & value);
    QVariant value(const QString & key, const QVariant & defaultValue = QVariant());
    void beginGroup(const QString & prefix);
    void beginGroup(int numGroup);
    void endGroup();
    int numChildGroups();
    bool containsGroup(QString groupName);
    bool isGroupEmpty();
    void remove(const QString & key);
    void clear();
    void sync();

    void deferSyncs(bool b);  // this must receive balanced calls with true and false, as it maintains a count (to support threads).
    bool needsDeferredSync();

protected:
    QByteArray XOR(const QByteArray &key, const QByteArray& data) const;
    QString encrypt(const QString key, const QString value) const;
    QString decrypt(const QString key, const QString value) const;
    QString hash(const QString key) const;
    QByteArray encryptionKey;
    int mDeferSyncEnableCount = 0;
    bool mSyncDeferred = false;
};

#endif // ENCRYPTEDSETTINGS_H
