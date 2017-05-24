/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "dclipboardfilecontroller.h"

#include <dfmfilecontrollerplugin.h>

DFM_USE_NAMESPACE

class DFMClipboardControllerPlugin : public DFMFileControllerPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DFMFileControllerFactoryInterface_iid FILE "clipboard-files.json")

public:
    explicit DFMClipboardControllerPlugin(QObject *parent = 0);

    DAbstractFileController *create(const QString &key) Q_DECL_OVERRIDE;
};

DFMClipboardControllerPlugin::DFMClipboardControllerPlugin(QObject *parent)
    : DFMFileControllerPlugin(parent)
{

}

DAbstractFileController *DFMClipboardControllerPlugin::create(const QString &key)
{
    Q_UNUSED(key)

    return new DClipboardFileController();
}

#include "main.moc"
