/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#ifndef DFMWEBVIEWPLUGIN_H
#define DFMWEBVIEWPLUGIN_H

#include <dfmbaseview.h>
#include <dfmabstracteventhandler.h>

#include <QWebView>

DFM_USE_NAMESPACE

class DFMWebView : public QWebView, public DFMBaseView, public DFMAbstractEventHandler
{
    Q_OBJECT
public:
    explicit DFMWebView(QWidget *parent = 0);

    QWidget *widget() const Q_DECL_OVERRIDE;
    DUrl rootUrl() const Q_DECL_OVERRIDE;
    bool setRootUrl(const DUrl &url) Q_DECL_OVERRIDE;

private slots:
    void notifyUrlChanged();

private:
    bool fmEventFilter(const QSharedPointer<DFMEvent> &event, DFMAbstractEventHandler *target = 0, QVariant *resultData = 0) Q_DECL_OVERRIDE;
};

#endif // DFMWEBVIEWPLUGIN_H
