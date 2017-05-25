QT       += core gui webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG += plugin link_pkgconfig
PKGCONFIG += dde-file-manager

DESTDIR = $$_PRO_FILE_PWD_/../views

OTHER_FILES += webview.json

HEADERS += \
    dfmwebview.h

SOURCES += \
    main.cpp \
    dfmwebview.cpp

target.path = /usr/lib/$${QMAKE_HOST.arch}-linux-gnu/dde-file-manager/plugins/views

INSTALLS += target
