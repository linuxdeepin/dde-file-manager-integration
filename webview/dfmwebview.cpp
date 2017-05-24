/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include "dfmwebview.h"

#include <dfmevent.h>
#include <dfmeventdispatcher.h>

#include <QWebHistory>

DFMWebView::DFMWebView(QWidget *parent)
    : QWebView(parent)
{
    connect(this, &DFMWebView::urlChanged, this, &DFMWebView::notifyUrlChanged);

    DFMEventDispatcher::instance()->installEventFilter(this);
}

QWidget *DFMWebView::widget() const
{
    return const_cast<DFMWebView*>(this);
}

DUrl DFMWebView::rootUrl() const
{
    return url();
}

bool DFMWebView::setRootUrl(const DUrl &url)
{
    setUrl(url);

    return true;
}

void DFMWebView::notifyUrlChanged()
{
    DFMBaseView::notifyUrlChanged();
}

bool DFMWebView::fmEventFilter(const QSharedPointer<DFMEvent> &event, DFMAbstractEventHandler *target, QVariant *resultData)
{
    Q_UNUSED(resultData)

    if (target && target->object() == window()) {
        switch ((int)event->type()) {
        case DFMEvent::Back:
            if (!history()->canGoBack())
                break;

            back();

            return true;
        case DFMEvent::Forward:
            if (!history()->canGoForward())
                break;

            forward();

            return true;
        }
    }

    return false;
}
