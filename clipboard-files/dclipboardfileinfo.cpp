/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include "dclipboardfileinfo.h"

#include <dfileservices.h>
#include <dfilesystemmodel.h>

DClipboardFileInfo::DClipboardFileInfo(const DUrl &url)
    : DAbstractFileInfo(url)
{
    if (url.hasFragment()) {
        setProxy(DFileService::instance()->createFileInfo(Q_NULLPTR, DUrl(url.fragment())));
    }
}

bool DClipboardFileInfo::isDir() const
{
    if (!fileUrl().hasFragment())
        return true;

    return DAbstractFileInfo::isDir();
}

QFileDevice::Permissions DClipboardFileInfo::permissions() const
{
    QFileDevice::Permissions p = DAbstractFileInfo::permissions();

    p = p & ~(QFileDevice::WriteGroup | QFileDevice::WriteOwner | QFileDevice::WriteUser | QFileDevice::WriteOther);

    return p;
}

QVector<MenuAction> DClipboardFileInfo::menuActionList(DAbstractFileInfo::MenuType type) const
{
    QVector<MenuAction> actions;

    if (type == SpaceArea)
        return actions;

    actions << MenuAction::Open << MenuAction::Separator;

    if (type == SingleFile) {
        actions << MenuAction::OpenFileLocation;

        if (isDir()) {
            actions << MenuAction::OpenInTerminal
                    << MenuAction::OpenInNewTab
                    << MenuAction::OpenInNewWindow;
        }
    }

    actions << MenuAction::Separator
            << MenuAction::Property;

    return actions;
}

QList<int> DClipboardFileInfo::userColumnRoles() const
{
    static QList<int> userColumnRoles = QList<int>() << DFileSystemModel::FileUserRole + 1
                                                     << DFileSystemModel::FileSizeRole
                                                     << DFileSystemModel::FileMimeTypeRole;

    return userColumnRoles;
}

QVariant DClipboardFileInfo::userColumnDisplayName(int userColumnRole) const
{
    if (userColumnRole == DFileSystemModel::FileUserRole + 1)
        return QObject::tr("Path", "SearchFileInfo");

    return DAbstractFileInfo::userColumnDisplayName(userColumnRole);
}

QVariant DClipboardFileInfo::userColumnData(int userColumnRole) const
{
    if (userColumnRole == DFileSystemModel::FileUserRole + 1) {
        const DUrl &fileUrl = DUrl(this->fileUrl().fragment());

        if (fileUrl.isLocalFile()) {
            return absolutePath();
        } else {
            DUrl newUrl = fileUrl;

            newUrl.setPath(QFileInfo(newUrl.path()).path());

            return newUrl.toString();
        }
    }

    return DAbstractFileInfo::userColumnData(userColumnRole);
}

int DClipboardFileInfo::userColumnWidth(int userColumnRole, const QFontMetrics &fontMetrics) const
{
    if (userColumnRole == DFileSystemModel::FileUserRole + 1)
        return -1;

    return DAbstractFileInfo::userColumnWidth(userColumnRole, fontMetrics);
}

Qt::ItemFlags DClipboardFileInfo::fileItemDisableFlags() const
{
    return Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
}

DUrl DClipboardFileInfo::mimeDataUrl() const
{
    return DUrl(fileUrl().fragment());
}

bool DClipboardFileInfo::canRedirectionFileUrl() const
{
    return fileUrl().hasFragment();
}

DUrl DClipboardFileInfo::redirectedFileUrl() const
{
    return mimeDataUrl();
}
