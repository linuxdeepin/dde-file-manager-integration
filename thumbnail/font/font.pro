QT += gui-private

TARGET  = font2image
CONFIG += plugin link_pkgconfig
TEMPLATE = lib
PKGCONFIG += fontconfig

OTHER_FILES += font2image.json

SOURCES += main.cpp \
    fontimageiohandler.cpp

target.path = $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target

HEADERS += \
    fontimageiohandler.h
