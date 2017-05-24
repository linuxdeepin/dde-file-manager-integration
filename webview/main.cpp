/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "dfmwebview.h"

#include <dfmviewplugin.h>

DFM_USE_NAMESPACE

class DFMWebViewPlugin : public DFMViewPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DFMViewFactoryInterface_iid FILE "webview.json")

public:
    explicit DFMWebViewPlugin(QObject *parent = 0);

    DFMBaseView *create(const QString &key) Q_DECL_OVERRIDE;
};

DFMWebViewPlugin::DFMWebViewPlugin(QObject *parent)
    : DFMViewPlugin(parent)
{

}

DFMBaseView *DFMWebViewPlugin::create(const QString &key)
{
    Q_UNUSED(key)

    return new DFMWebView();
}

#include "main.moc"
