TEMPLATE = app

QT += qml quick widgets network quickcontrols2

CONFIG += c++11

SOURCES += main.cpp \
    oauth2.cpp \
    oauthmastodon.cpp \
    qmastodonnetbase.cpp \
    qmastodonpoststatus.cpp \
    binding.cpp


RESOURCES += qml.qrc

OTHER_FILES = content/*.qml

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    oauth2.h \
    oauthmastodon.h \
    qmastodonnetbase.h \
    qmastodonpoststatus.h \
    binding.h

DISTFILES += \
    content/AuthWindow.qml

