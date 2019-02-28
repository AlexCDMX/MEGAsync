CONFIG -= qt
MEGASDK_BASE_PATH = $$PWD/../MEGASync/mega

CONFIG(debug, debug|release) {
    CONFIG -= debug release
    CONFIG += debug
}
CONFIG(release, debug|release) {
    CONFIG -= debug release
    CONFIG += release
}

TARGET = MEGAupdater
TEMPLATE = app

HEADERS += UpdateTask.h \
    Preferences.h \
    MacUtils.h

SOURCES += MegaUpdater.cpp \
    UpdateTask.cpp

INCLUDEPATH += $$MEGASDK_BASE_PATH/bindings/qt/3rdparty/include

macx {    
    OBJECTIVE_SOURCES +=  MacUtils.mm
    DEFINES += _DARWIN_FEATURE_64_BIT_INODE USE_OPENSSL CRYPTOPP_DISABLE_ASM
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
    LIBS += -L$$MEGASDK_BASE_PATH/bindings/qt/3rdparty/libs/
    LIBS += -framework Cocoa -framework SystemConfiguration -framework CoreFoundation -framework Foundation -framework Security
    QMAKE_CXXFLAGS += -g
    LIBS += -lcryptopp
}

win32 {
    contains(CONFIG, BUILDX64) {
       release {
            LIBS += -L"$$MEGASDK_BASE_PATH/bindings/qt/3rdparty/libs/x64"
        }
        else {
            LIBS += -L"$$MEGASDK_BASE_PATH/bindings/qt/3rdparty/libs/x64d"
        }
    }

    !contains(CONFIG, BUILDX64) {
        release {
            LIBS += -L"$$MEGASDK_BASE_PATH/bindings/qt/3rdparty/libs/x32"
        }
        else {
            LIBS += -L"$$MEGASDK_BASE_PATH/bindings/qt/3rdparty/libs/x32d"
        }
    }

    DEFINES += UNICODE _UNICODE NTDDI_VERSION=0x05010000 _WIN32_WINNT=0x0501
    LIBS += -lurlmon -lShlwapi -lShell32 -lAdvapi32 -lcryptoppmt

    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

    QMAKE_CXXFLAGS_RELEASE += -MT
    QMAKE_CXXFLAGS_DEBUG += -MTd

    QMAKE_CXXFLAGS_RELEASE -= -MD
    QMAKE_CXXFLAGS_DEBUG -= -MDd

    RC_FILE = icon.rc
    QMAKE_LFLAGS += /LARGEADDRESSAWARE
    QMAKE_LFLAGS_WINDOWS += /SUBSYSTEM:WINDOWS,5.01
    QMAKE_LFLAGS_CONSOLE += /SUBSYSTEM:CONSOLE,5.01
    DEFINES += PSAPI_VERSION=1
}

unix:!macx {
    error("This tool (MEGAupdater) is only compatible with Windows and macOS. On Linux, MEGA apps can be updated using the official repository.")
}
