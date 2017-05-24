/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include "dclipboardfilecontroller.h"
#include "dclipboardfileinfo.h"

#include <dfileservices.h>
#include <dfmevent.h>
#include <dfmglobal.h>
#include <private/dabstractfilewatcher_p.h>

class DClipboardFileWatcher : public DAbstractFileWatcher
{
    Q_OBJECT
public:
    explicit DClipboardFileWatcher(QObject *parent = 0);

public slots:
    void onClipboardDataChanged();
};

class DClipboardFileWatcherPrivate : public DAbstractFileWatcherPrivate
{
public:
    using DAbstractFileWatcherPrivate::DAbstractFileWatcherPrivate;

    bool start() Q_DECL_OVERRIDE
    {
        Q_Q(DClipboardFileWatcher);

        return QObject::connect(DFMGlobal::instance(), &DFMGlobal::clipboardDataChanged, q, &DClipboardFileWatcher::onClipboardDataChanged);
    }

    bool stop() Q_DECL_OVERRIDE
    {
        Q_Q(DClipboardFileWatcher);

        return QObject::disconnect(DFMGlobal::instance(), SIGNAL(clipboardDataChanged()), q, SLOT(onClipboardDataChanged()));
    }

    Q_DECLARE_PUBLIC(DClipboardFileWatcher)
};

DClipboardFileWatcher::DClipboardFileWatcher(QObject *parent)
    : DAbstractFileWatcher(*new DClipboardFileWatcherPrivate(this), DUrl("clipboard:///"), parent)
{

}

void DClipboardFileWatcher::onClipboardDataChanged()
{
    emit subfileCreated(fileUrl());
}

DClipboardFileController::DClipboardFileController(QObject *parent)
    : DAbstractFileController(parent)
{

}

bool DClipboardFileController::openFile(const QSharedPointer<DFMOpenFileEvent> &event) const
{
    return DFileService::instance()->openFile(event->sender(), DUrl(event->url().fragment()));
}

bool DClipboardFileController::openFileLocation(const QSharedPointer<DFMOpenFileLocation> &event) const
{
    return DFileService::instance()->openFileLocation(event->sender(), DUrl(event->url().fragment()));
}

const QList<DAbstractFileInfoPointer> DClipboardFileController::getChildren(const QSharedPointer<DFMGetChildrensEvent> &event) const
{
    Q_UNUSED(event)

    QList<DAbstractFileInfoPointer> list;

    for (const DUrl &url : DFMGlobal::instance()->clipboardFileUrlList()) {
        DUrl newUrl("clipboard:///");

        newUrl.setFragment(url.toString());

        list << DAbstractFileInfoPointer(new DClipboardFileInfo(newUrl));
    }

    return list;
}

const DAbstractFileInfoPointer DClipboardFileController::createFileInfo(const QSharedPointer<DFMCreateFileInfoEvnet> &event) const
{
    Q_UNUSED(event)

    return DAbstractFileInfoPointer(new DClipboardFileInfo(event->url()));
}

bool DClipboardFileController::openInTerminal(const QSharedPointer<DFMOpenInTerminalEvent> &event) const
{
    return DFileService::instance()->openInTerminal(event->sender(), DUrl(event->url().fragment()));
}

DAbstractFileWatcher *DClipboardFileController::createFileWatcher(const QSharedPointer<DFMCreateFileWatcherEvent> &event) const
{
    if (event->url() != DUrl("clipboard:///"))
        return 0;

    return new DClipboardFileWatcher();
}

#include "dclipboardfilecontroller.moc"
