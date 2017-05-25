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
#include <QAction>
#include <QWebHitTestResult>
#include <QMenu>

class DFMWebViewPrivate
{
public:
    static DFMWebView *lastCreateWebView;
};

DFMWebView *DFMWebViewPrivate::lastCreateWebView = Q_NULLPTR;

DFMWebView::DFMWebView(QWidget *parent)
    : QWebView(parent)
{
    DFMWebViewPrivate::lastCreateWebView = this;

    page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);

    connect(this, &DFMWebView::urlChanged, this, &DFMWebView::notifyUrlChanged);
    connect(this, &DFMWebView::linkClicked, this, &DFMWebView::setUrl);

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

void DFMWebView::openLinkInNewTab(const DUrl &url)
{
    DFMEventDispatcher::instance()->processEvent<DFMOpenNewTabEvent>(this, url);
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

QWebView *DFMWebView::createWindow(QWebPage::WebWindowType type)
{
    if (type != QWebPage::WebBrowserWindow)
        return Q_NULLPTR;

    DFMWebView *lastCreateWebView = DFMWebViewPrivate::lastCreateWebView;

    DFMEventDispatcher::instance()->processEvent<DFMOpenNewWindowEvent>(this, QList<DUrl>() << DUrl("http://"));

    if (lastCreateWebView == DFMWebViewPrivate::lastCreateWebView)
        return Q_NULLPTR;

    return DFMWebViewPrivate::lastCreateWebView;
}

void DFMWebView::contextMenuEvent(QContextMenuEvent *event)
{
    const DUrl &url = page()->mainFrame()->hitTestContent(event->pos()).linkUrl();

    if (url.isEmpty()) {
        return QWebView::contextMenuEvent(event);
    }

    QMenu menu(this);
    menu.addAction(pageAction(QWebPage::OpenLinkInNewWindow));

    connect(menu.addAction(tr("Open in New Tab")), &QAction::triggered, this, [this, url] {
        openLinkInNewTab(url);
    });

    menu.addSeparator();
    menu.addAction(pageAction(QWebPage::DownloadLinkToDisk));
    // Add link to bookmarks...
    menu.addSeparator();
    menu.addAction(pageAction(QWebPage::CopyLinkToClipboard));
    if (page()->settings()->testAttribute(QWebSettings::DeveloperExtrasEnabled))
        menu.addAction(pageAction(QWebPage::InspectElement));
    menu.exec(mapToGlobal(event->pos()));
}
