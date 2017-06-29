QT += core gui network

CONFIG += c++11 link_pkgconfig
PKGCONFIG += dde-file-manager

DESTDIR = $$_PRO_FILE_PWD_/../generics

TARGET = nutstore-dfm-plugin
CONFIG += plugin

TEMPLATE = lib

SOURCES += main.cpp \
    dfmgenericpluginobject.cpp

HEADERS += \
    dfmgenericpluginobject.h

DISTFILES += generic.json

target.path = /usr/lib/$${QMAKE_HOST.arch}-linux-gnu/dde-file-manager/plugins/generics

INSTALLS += target
