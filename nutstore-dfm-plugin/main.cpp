/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <dfmgenericplugin.h>

#include "dfmgenericpluginobject.h"

DFM_USE_NAMESPACE

class PluginMain : public DFMGenericPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DFMGenericFactoryInterface_iid FILE "generic.json")
public:
    QObject *create(const QString &key)
    {
        if (key != "fileinfo/additionalIcon")
            return Q_NULLPTR;

        return new DFMGenericPluginObject();
    }
};

#include "main.moc"
