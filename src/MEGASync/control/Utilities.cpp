#include "Utilities.h"
#include "control/Preferences.h"

#include <QApplication>
#include <QImageReader>
#include <QDirIterator>
#include <QDesktopServices>
#include <QTextStream>
#include <QDateTime>
#include <iostream>
#include "MegaApplication.h"

#ifndef WIN32
#include "megaapi.h"
#include <utime.h>
#endif

using namespace std;
using namespace mega;

QHash<QString, QString> Utilities::extensionIcons;
QHash<QString, QString> Utilities::languageNames;

void Utilities::initializeExtensions()
{
    extensionIcons[QString::fromAscii("3ds")] = extensionIcons[QString::fromAscii("3dm")]  = extensionIcons[QString::fromAscii("max")] =
                            extensionIcons[QString::fromAscii("obj")]  = QString::fromAscii("3D.png");

    extensionIcons[QString::fromAscii("aep")] = extensionIcons[QString::fromAscii("aet")]  = QString::fromAscii("aftereffects.png");

    extensionIcons[QString::fromAscii("mp3")] = extensionIcons[QString::fromAscii("wav")]  = extensionIcons[QString::fromAscii("3ga")]  =
                            extensionIcons[QString::fromAscii("aif")]  = extensionIcons[QString::fromAscii("aiff")] =
                            extensionIcons[QString::fromAscii("flac")] = extensionIcons[QString::fromAscii("iff")]  =
                            extensionIcons[QString::fromAscii("m4a")]  = extensionIcons[QString::fromAscii("wma")]  =  QString::fromAscii("audio.png");

    extensionIcons[QString::fromAscii("dxf")] = extensionIcons[QString::fromAscii("dwg")] =  QString::fromAscii("cad.png");


    extensionIcons[QString::fromAscii("zip")] = extensionIcons[QString::fromAscii("rar")] = extensionIcons[QString::fromAscii("tgz")]  =
                            extensionIcons[QString::fromAscii("gz")]  = extensionIcons[QString::fromAscii("bz2")]  =
                            extensionIcons[QString::fromAscii("tbz")] = extensionIcons[QString::fromAscii("tar")]  =
                            extensionIcons[QString::fromAscii("7z")]  = extensionIcons[QString::fromAscii("sitx")] =  QString::fromAscii("compressed.png");

    extensionIcons[QString::fromAscii("sql")] = extensionIcons[QString::fromAscii("accdb")] = extensionIcons[QString::fromAscii("db")]  =
                            extensionIcons[QString::fromAscii("dbf")]  = extensionIcons[QString::fromAscii("mdb")]  =
                            extensionIcons[QString::fromAscii("pdb")] = QString::fromAscii("web_lang.png");

    extensionIcons[QString::fromAscii("folder")] = QString::fromAscii("folder.png");

    extensionIcons[QString::fromAscii("xls")] = extensionIcons[QString::fromAscii("xlsx")] = extensionIcons[QString::fromAscii("xlt")]  =
                            extensionIcons[QString::fromAscii("xltm")]  = QString::fromAscii("excel.png");

    extensionIcons[QString::fromAscii("exe")] = extensionIcons[QString::fromAscii("com")] = extensionIcons[QString::fromAscii("bin")]  =
                            extensionIcons[QString::fromAscii("apk")]  = extensionIcons[QString::fromAscii("app")]  =
                             extensionIcons[QString::fromAscii("msi")]  = extensionIcons[QString::fromAscii("cmd")]  =
                            extensionIcons[QString::fromAscii("gadget")] = QString::fromAscii("executable.png");

    extensionIcons[QString::fromAscii("fnt")] = extensionIcons[QString::fromAscii("otf")] = extensionIcons[QString::fromAscii("ttf")]  =
                            extensionIcons[QString::fromAscii("fon")]  = QString::fromAscii("font.png");

    extensionIcons[QString::fromAscii("gif")] = extensionIcons[QString::fromAscii("tiff")]  = extensionIcons[QString::fromAscii("tif")]  =
                            extensionIcons[QString::fromAscii("bmp")]  = extensionIcons[QString::fromAscii("png")] =
                            extensionIcons[QString::fromAscii("tga")]  = QString::fromAscii("image.png");

    extensionIcons[QString::fromAscii("ai")] = extensionIcons[QString::fromAscii("ait")] = QString::fromAscii("illustrator.png");
    extensionIcons[QString::fromAscii("jpg")] = extensionIcons[QString::fromAscii("jpeg")] = QString::fromAscii("image.png");
    extensionIcons[QString::fromAscii("indd")] = QString::fromAscii("indesign.png");

    extensionIcons[QString::fromAscii("jar")] = extensionIcons[QString::fromAscii("java")]  = extensionIcons[QString::fromAscii("class")]  = QString::fromAscii("web_data.png");

    extensionIcons[QString::fromAscii("pdf")] = QString::fromAscii("pdf.png");
    extensionIcons[QString::fromAscii("abr")] = extensionIcons[QString::fromAscii("psb")]  = extensionIcons[QString::fromAscii("psd")]  =
                            QString::fromAscii("photoshop.png");

    extensionIcons[QString::fromAscii("pps")] = extensionIcons[QString::fromAscii("ppt")]  = extensionIcons[QString::fromAscii("pptx")] = QString::fromAscii("powerpoint.png");

    extensionIcons[QString::fromAscii("prproj")] = extensionIcons[QString::fromAscii("ppj")]  = QString::fromAscii("premiere.png");

    extensionIcons[QString::fromAscii("3fr")] = extensionIcons[QString::fromAscii("arw")]  = extensionIcons[QString::fromAscii("bay")]  =
                            extensionIcons[QString::fromAscii("cr2")]  = extensionIcons[QString::fromAscii("dcr")] =
                            extensionIcons[QString::fromAscii("dng")] = extensionIcons[QString::fromAscii("fff")]  =
                            extensionIcons[QString::fromAscii("mef")] = extensionIcons[QString::fromAscii("mrw")]  =
                            extensionIcons[QString::fromAscii("nef")] = extensionIcons[QString::fromAscii("pef")]  =
                            extensionIcons[QString::fromAscii("rw2")] = extensionIcons[QString::fromAscii("srf")]  =
                            extensionIcons[QString::fromAscii("orf")] = extensionIcons[QString::fromAscii("rwl")]  =
                            QString::fromAscii("raw.png");

    extensionIcons[QString::fromAscii("ods")]  = extensionIcons[QString::fromAscii("ots")]  =
                            extensionIcons[QString::fromAscii("gsheet")]  = extensionIcons[QString::fromAscii("nb")] =
                            extensionIcons[QString::fromAscii("xlr")] = QString::fromAscii("spreadsheet.png");

    extensionIcons[QString::fromAscii("torrent")] = QString::fromAscii("torrent.png");
    extensionIcons[QString::fromAscii("dmg")] = QString::fromAscii("dmg.png");

    extensionIcons[QString::fromAscii("txt")] = extensionIcons[QString::fromAscii("rtf")]  = extensionIcons[QString::fromAscii("ans")]  =
                            extensionIcons[QString::fromAscii("ascii")]  = extensionIcons[QString::fromAscii("log")] =
                            extensionIcons[QString::fromAscii("odt")] = extensionIcons[QString::fromAscii("wpd")]  =
                            QString::fromAscii("text.png");

    extensionIcons[QString::fromAscii("svgz")]  = extensionIcons[QString::fromAscii("svg")]  =
                            extensionIcons[QString::fromAscii("cdr")]  = extensionIcons[QString::fromAscii("eps")] =
                            QString::fromAscii("vector.png");

     extensionIcons[QString::fromAscii("mkv")]  = extensionIcons[QString::fromAscii("webm")]  =
                            extensionIcons[QString::fromAscii("avi")]  = extensionIcons[QString::fromAscii("mp4")] =
                            extensionIcons[QString::fromAscii("m4v")] = extensionIcons[QString::fromAscii("mpg")]  =
                            extensionIcons[QString::fromAscii("mpeg")] = extensionIcons[QString::fromAscii("mov")]  =
                            extensionIcons[QString::fromAscii("3g2")] = extensionIcons[QString::fromAscii("3gp")]  =
                            extensionIcons[QString::fromAscii("asf")] = extensionIcons[QString::fromAscii("wmv")]  =
                            extensionIcons[QString::fromAscii("flv")] = extensionIcons[QString::fromAscii("vob")] =
                            QString::fromAscii("video.png");

     extensionIcons[QString::fromAscii("html")]  = extensionIcons[QString::fromAscii("xml")] = extensionIcons[QString::fromAscii("shtml")]  =
                            extensionIcons[QString::fromAscii("dhtml")] = extensionIcons[QString::fromAscii("js")] =
                            extensionIcons[QString::fromAscii("css")]  = QString::fromAscii("web_data.png");

     extensionIcons[QString::fromAscii("php")]  = extensionIcons[QString::fromAscii("php3")]  =
                            extensionIcons[QString::fromAscii("php4")]  = extensionIcons[QString::fromAscii("php5")] =
                            extensionIcons[QString::fromAscii("phtml")] = extensionIcons[QString::fromAscii("inc")]  =
                            extensionIcons[QString::fromAscii("asp")] = extensionIcons[QString::fromAscii("pl")]  =
                            extensionIcons[QString::fromAscii("cgi")] = extensionIcons[QString::fromAscii("py")]  =
                            QString::fromAscii("web_lang.png");

     extensionIcons[QString::fromAscii("doc")]  = extensionIcons[QString::fromAscii("docx")] = extensionIcons[QString::fromAscii("dotx")]  =
                            extensionIcons[QString::fromAscii("wps")] = QString::fromAscii("word.png");

     extensionIcons[QString::fromAscii("odt")]  = extensionIcons[QString::fromAscii("ods")] = extensionIcons[QString::fromAscii("odp")]  =
                            extensionIcons[QString::fromAscii("odb")] = extensionIcons[QString::fromAscii("odg")] = QString::fromAscii("openoffice.png");

     extensionIcons[QString::fromAscii("sketch")] = QString::fromAscii("sketch.png");
     extensionIcons[QString::fromAscii("xd")] = QString::fromAscii("experiencedesign.png");
     extensionIcons[QString::fromAscii("pages")] = QString::fromAscii("pages.png");
     extensionIcons[QString::fromAscii("numbers")] = QString::fromAscii("numbers.png");
     extensionIcons[QString::fromAscii("key")] = QString::fromAscii("keynote.png");
}

void Utilities::getFolderSize(QString folderPath, long long *size)
{
    if (!folderPath.size())
    {
        return;
    }

    QDir dir(folderPath);
    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);
    for (int i = 0; i < entries.size(); i++)
    {
        QFileInfo info = entries[i];
        if (info.isFile())
        {
            (*size) += info.size();
        }
        else if (info.isDir())
        {
            getFolderSize(info.absoluteFilePath(), size);
        }
    }
}

qreal Utilities::getDevicePixelRatio()
{
#if QT_VERSION >= 0x050000
    return qApp->testAttribute(Qt::AA_UseHighDpiPixmaps) ? qApp->devicePixelRatio() : 1.0;
#else
    return 1.0;
#endif
}

QString Utilities::getExtensionPixmapName(QString fileName, QString prefix)
{
    if (extensionIcons.isEmpty())
    {
        initializeExtensions();
    }

    QFileInfo f(fileName);
    if (extensionIcons.contains(f.suffix().toLower()))
    {
        return QString(extensionIcons[f.suffix().toLower()]).insert(0, prefix);
    }
    else
    {
        return prefix + QString::fromAscii("generic.png");
    }
}

QString Utilities::languageCodeToString(QString code)
{
    if (languageNames.isEmpty())
    {
        languageNames[QString::fromAscii("ar")] = QString::fromUtf8("العربية");
        languageNames[QString::fromAscii("de")] = QString::fromUtf8("Deutsch");
        languageNames[QString::fromAscii("en")] = QString::fromUtf8("English");
        languageNames[QString::fromAscii("es")] = QString::fromUtf8("Español");
        languageNames[QString::fromAscii("fr")] = QString::fromUtf8("Français");
        languageNames[QString::fromAscii("id")] = QString::fromUtf8("Bahasa Indonesia");
        languageNames[QString::fromAscii("it")] = QString::fromUtf8("Italiano");
        languageNames[QString::fromAscii("ja")] = QString::fromUtf8("日本語");
        languageNames[QString::fromAscii("ko")] = QString::fromUtf8("한국어");
        languageNames[QString::fromAscii("nl")] = QString::fromUtf8("Nederlands");
        languageNames[QString::fromAscii("pl")] = QString::fromUtf8("Polski");
        languageNames[QString::fromAscii("pt_BR")] = QString::fromUtf8("Português Brasil");
        languageNames[QString::fromAscii("pt")] = QString::fromUtf8("Português");
        languageNames[QString::fromAscii("ro")] = QString::fromUtf8("Română");
        languageNames[QString::fromAscii("ru")] = QString::fromUtf8("Pусский");
        languageNames[QString::fromAscii("th")] = QString::fromUtf8("ภาษาไทย");
        languageNames[QString::fromAscii("tl")] = QString::fromUtf8("Tagalog");
        languageNames[QString::fromAscii("uk")] = QString::fromUtf8("Українська");
        languageNames[QString::fromAscii("vi")] = QString::fromUtf8("Tiếng Việt");
        languageNames[QString::fromAscii("zh_CN")] = QString::fromUtf8("简体中文");
        languageNames[QString::fromAscii("zh_TW")] = QString::fromUtf8("中文繁體");


        // Currently unsupported
        // languageNames[QString::fromAscii("mi")] = QString::fromUtf8("Māori");
        // languageNames[QString::fromAscii("ca")] = QString::fromUtf8("Català");
        // languageNames[QString::fromAscii("eu")] = QString::fromUtf8("Euskara");
        // languageNames[QString::fromAscii("af")] = QString::fromUtf8("Afrikaans");
        // languageNames[QString::fromAscii("no")] = QString::fromUtf8("Norsk");
        // languageNames[QString::fromAscii("bs")] = QString::fromUtf8("Bosanski");
        // languageNames[QString::fromAscii("da")] = QString::fromUtf8("Dansk");
        // languageNames[QString::fromAscii("el")] = QString::fromUtf8("ελληνικά");
        // languageNames[QString::fromAscii("lt")] = QString::fromUtf8("Lietuvos");
        // languageNames[QString::fromAscii("lv")] = QString::fromUtf8("Latviešu");
        // languageNames[QString::fromAscii("mk")] = QString::fromUtf8("македонски");
        // languageNames[QString::fromAscii("hi")] = QString::fromUtf8("हिंदी");
        // languageNames[QString::fromAscii("ms")] = QString::fromUtf8("Bahasa Malaysia");
        // languageNames[QString::fromAscii("cy")] = QString::fromUtf8("Cymraeg");
        // languageNames[QString::fromAscii("ee")] = QString::fromUtf8("Eesti");
        // languageNames[QString::fromAscii("fa")] = QString::fromUtf8("فارسی");
        // languageNames[QString::fromAscii("hr")] = QString::fromUtf8("Hrvatski");
        // languageNames[QString::fromAscii("ka")] = QString::fromUtf8("ქართული");
        // languageNames[QString::fromAscii("cs")] = QString::fromUtf8("Čeština");
        // languageNames[QString::fromAscii("sk")] = QString::fromUtf8("Slovenský");
        // languageNames[QString::fromAscii("sl")] = QString::fromUtf8("Slovenščina");
        // languageNames[QString::fromAscii("hu")] = QString::fromUtf8("Magyar");
        // languageNames[QString::fromAscii("fi")] = QString::fromUtf8("Suomi");
        // languageNames[QString::fromAscii("sr")] = QString::fromUtf8("српски");
        // languageNames[QString::fromAscii("sv")] = QString::fromUtf8("Svenska");
        // languageNames[QString::fromAscii("bg")] = QString::fromUtf8("български");
        // languageNames[QString::fromAscii("he")] = QString::fromUtf8("עברית");
        // languageNames[QString::fromAscii("tr")] = QString::fromUtf8("Türkçe");

    }
    return languageNames.value(code);
}


struct IconCache
{
    std::map<QString, QIcon> mIcons;

    QIcon& getDirect(QString resourceName)
    {
        auto i = mIcons.find(resourceName);
        if (i == mIcons.end())
        {
            auto pair = mIcons.emplace(resourceName, QIcon());
            i = pair.first;
            i->second.addFile(resourceName, QSize(), QIcon::Normal, QIcon::Off);
        }
        return i->second;
    }

    QIcon& getByExtension(const QString &fileName, const QString &prefix)
    {
        return getDirect(Utilities::getExtensionPixmapName(fileName, prefix));
    }
};

IconCache gIconCache;

QString Utilities::getExtensionPixmapNameSmall(QString fileName)
{
    return getExtensionPixmapName(fileName, QString::fromAscii(":/images/small_"));
}

QString Utilities::getExtensionPixmapNameMedium(QString fileName)
{
    return getExtensionPixmapName(fileName, QString::fromAscii(":/images/drag_"));
}

QIcon Utilities::getCachedPixmap(QString fileName)
{
    return gIconCache.getDirect(fileName);
}

QIcon Utilities::getExtensionPixmapSmall(QString fileName)
{
    return gIconCache.getDirect(getExtensionPixmapNameSmall(fileName));
}

QIcon Utilities::getExtensionPixmapMedium(QString fileName)
{
    return gIconCache.getDirect(getExtensionPixmapNameMedium(fileName));
}

QString Utilities::getAvatarPath(QString email)
{
    if (!email.size())
    {
        return QString();
    }

    QString avatarsPath = QString::fromUtf8("%1/avatars/%2.jpg")
            .arg(Preferences::instance()->getDataPath()).arg(email);
    return QDir::toNativeSeparators(avatarsPath);
}

bool Utilities::removeRecursively(QString path)
{
    if (!path.size())
    {
        return false;
    }

    QDir dir(path);
    bool success = false;
    QString qpath = QDir::toNativeSeparators(dir.absolutePath());
    MegaApi::removeRecursively(qpath.toUtf8().constData());
    success = dir.rmdir(dir.absolutePath());
    return success;
}

void Utilities::copyRecursively(QString srcPath, QString dstPath)
{
    if (!srcPath.size() || !dstPath.size())
    {
        return;
    }

    QFileInfo source(srcPath);
    if (!source.exists())
    {
        return;
    }

    if (srcPath == dstPath)
    {
        return;
    }

    QFile dst(dstPath);
    if (dst.exists())
    {
        return;
    }

    if (source.isFile())
    {
        QFile src(srcPath);
        src.copy(dstPath);
#ifndef _WIN32
       QFileInfo info(src);
       time_t t = info.lastModified().toTime_t();
       struct utimbuf times = { t, t };
       utime(dstPath.toUtf8().constData(), &times);
#endif
    }
    else if (source.isDir())
    {
        QDir dstDir(dstPath);
        dstDir.mkpath(QString::fromAscii("."));
        QDirIterator di(srcPath, QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot);
        while (di.hasNext())
        {
            di.next();
            if (!di.fileInfo().isSymLink())
            {
                copyRecursively(di.filePath(), dstPath + QDir::separator() + di.fileName());
            }
        }
    }
}

bool Utilities::verifySyncedFolderLimits(QString path)
{
#ifdef WIN32
    if (path.startsWith(QString::fromAscii("\\\\?\\")))
    {
        path = path.mid(4);
    }
#endif

    QString rootPath = QDir::toNativeSeparators(QDir::rootPath());
    if (rootPath == path)
    {
        return false;
    }
    return true;
}
QString Utilities::getTimeString(long long secs, bool secondPrecision)
{
    int seconds = (int) secs % 60;
    int minutes = (int) ((secs / 60) % 60);
    int hours   = (int) (secs / (60 * 60)) % 24;
    int days = (int)(secs / (60 * 60 * 24));

    int items = 0;
    QString time;

    if (days)
    {
        items++;
        time.append(QString::fromUtf8(" %1 <span style=\"color:#777777; text-decoration:none;\">d</span>").arg(days));
    }

    if (items || hours)
    {
        items++;
        time.append(QString::fromUtf8(" %1 <span style=\"color:#777777; text-decoration:none;\">h</span>").arg(hours));
    }

    if (items == 2)
    {
        time = time.trimmed();
        return time;
    }

    if (items || minutes)
    {
        items++;
        time.append(QString::fromUtf8(" %1 <span style=\"color:#777777; text-decoration:none;\">m</span>").arg(minutes));
    }

    if (items == 2)
    {
        time = time.trimmed();
        return time;
    }

    if (secondPrecision)
    {
        time.append(QString::fromUtf8(" %1 <span style=\"color:#777777; text-decoration:none;\">s</span>").arg(seconds));
    }
    time = time.trimmed();
    return time;
}

QString Utilities::getFinishedTimeString(long long secs)
{
    if (secs < 2)
    {
        return QCoreApplication::translate("Utilities", "just now");
    }
    else if (secs < 60)
    {
        return QCoreApplication::translate("Utilities", "%1 seconds ago").arg(secs);
    }
    else if (secs < 3600)
    {
        int minutes = secs/60;
        if (minutes == 1)
        {
            return QCoreApplication::translate("Utilities", "1 minute ago");
        }
        else
        {
            return QCoreApplication::translate("Utilities", "%1 minutes ago").arg(minutes);
        }
    }
    else if (secs < 86400)
    {
        int hours = secs/3600;
        if (hours == 1)
        {
            return QCoreApplication::translate("Utilities", "1 hour ago");
        }
        else
        {
            return QCoreApplication::translate("Utilities", "%1 hours ago").arg(hours);
        }
    }
    else if (secs < 2592000)
    {
        int days = secs/86400;
        if (days == 1)
        {
            return QCoreApplication::translate("Utilities", "1 day ago");
        }
        else
        {
            return QCoreApplication::translate("Utilities", "%1 days ago").arg(days);
        }
    }
    else if (secs < 31536000)
    {
        int months = secs/2592000;
        if (months == 1)
        {
            return QCoreApplication::translate("Utilities", "1 month ago");
        }
        else
        {
            return QCoreApplication::translate("Utilities", "%1 months ago").arg(months);
        }
    }
    else
    {
        int years = secs/31536000;
        if (years == 1)
        {
            return QCoreApplication::translate("Utilities", "1 year ago");
        }
        else
        {
            return QCoreApplication::translate("Utilities", "%1 years ago").arg(years);
        }
    }
}

QString Utilities::getSizeString(unsigned long long bytes)
{
    unsigned long long KB = 1024;
    unsigned long long MB = 1024 * KB;
    unsigned long long GB = 1024 * MB;
    unsigned long long TB = 1024 * GB;

    QString language = ((MegaApplication*)qApp)->getCurrentLanguageCode();
    QLocale locale(language);
    if (bytes >= TB)
    {
        return locale.toString( ((int)((10 * bytes) / TB))/10.0) + QString::fromAscii(" ") + QCoreApplication::translate("Utilities", "TB");
    }

    if (bytes >= GB)
    {
        return locale.toString( ((int)((10 * bytes) / GB))/10.0) + QString::fromAscii(" ") + QCoreApplication::translate("Utilities", "GB");
    }

    if (bytes >= MB)
    {
        return locale.toString( ((int)((10 * bytes) / MB))/10.0) + QString::fromAscii(" ") + QCoreApplication::translate("Utilities", "MB");
    }

    if (bytes >= KB)
    {
        return locale.toString( ((int)((10 * bytes) / KB))/10.0) + QString::fromAscii(" ") + QCoreApplication::translate("Utilities", "KB");
    }

    return locale.toString(bytes) + QString::fromAscii(" bytes");
}

QString Utilities::extractJSONString(QString json, QString name)
{
    QString pattern = name + QString::fromUtf8("\":\"");
    int pos = json.indexOf(pattern);
    if (pos < 0)
    {
        return QString();
    }

    int end = json.indexOf(QString::fromUtf8("\""), pos + pattern.size());
    if (end < 0)
    {
        return QString();
    }

    return json.mid(pos + pattern.size(), end - pos - pattern.size());
}

long long Utilities::extractJSONNumber(QString json, QString name)
{
    QString pattern = name + QString::fromUtf8("\":");
    int pos = json.indexOf(pattern);
    if (pos < 0)
    {
        return 0;
    }

    int end = pos + pattern.size();
    int count = 0;
    while (json[end].isDigit())
    {
        end++;
        count++;
    }

    return json.mid(pos + pattern.size(), count).toLongLong();
}

QString Utilities::getDefaultBasePath()
{
#ifdef WIN32
    #if QT_VERSION < 0x050000
        QString defaultPath = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
        if (!defaultPath.isEmpty())
        {
            return defaultPath;
        }

        defaultPath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
        if (!defaultPath.isEmpty())
        {
            return defaultPath;
        }

        defaultPath = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
        if (!defaultPath.isEmpty())
        {
            return defaultPath;
        }
    #else
        QStringList defaultPaths = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
        if (defaultPaths.size())
        {
            return defaultPaths.at(0);
        }

        defaultPaths = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
        if (defaultPaths.size())
        {
            return defaultPaths.at(0);
        }

        defaultPaths = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
        if (defaultPaths.size())
        {
            return defaultPaths.at(0);
        }

        defaultPaths = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
        if (defaultPaths.size())
        {
            return defaultPaths.at(0);
        }
    #endif
#else
    #if QT_VERSION < 0x050000
        QString defaultPath = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
        if (!defaultPath.isEmpty())
        {
            return defaultPath;
        }

        defaultPath = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
        if (!defaultPath.isEmpty())
        {
            return defaultPath;
        }

        defaultPath = QDesktopServices::storageLocation(QDesktopServices::DesktopLocation);
        if (!defaultPath.isEmpty())
        {
            return defaultPath;
        }
    #else
        QStringList defaultPaths = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
        if (defaultPaths.size())
        {
            return defaultPaths.at(0);
        }

        defaultPaths = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
        if (defaultPaths.size())
        {
            return defaultPaths.at(0);
        }

        defaultPaths = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
        if (defaultPaths.size())
        {
            return defaultPaths.at(0);
        }

        defaultPaths = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation);
        if (defaultPaths.size())
        {
            return defaultPaths.at(0);
        }
    #endif
#endif

    QDir dataDir = QDir(Preferences::instance()->getDataPath());
    QString rootPath = QDir::toNativeSeparators(dataDir.rootPath());
    if (rootPath.size() && rootPath.at(rootPath.size() - 1) == QDir::separator())
    {
        rootPath.resize(rootPath.size() - 1);
        return rootPath;
    }
    return QString();
}

