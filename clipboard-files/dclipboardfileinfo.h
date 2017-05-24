/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#ifndef DCLIPBOARDFILEINFO_H
#define DCLIPBOARDFILEINFO_H

#include <dabstractfileinfo.h>

class DClipboardFileInfo : public DAbstractFileInfo
{
public:
    explicit DClipboardFileInfo(const DUrl &url);

    bool isDir() const Q_DECL_OVERRIDE;

    QFileDevice::Permissions permissions() const Q_DECL_OVERRIDE;
    QVector<MenuAction> menuActionList(MenuType type = SingleFile) const Q_DECL_OVERRIDE;
    QList<int> userColumnRoles() const Q_DECL_OVERRIDE;
    QVariant userColumnDisplayName(int userColumnRole) const Q_DECL_OVERRIDE;
    QVariant userColumnData(int userColumnRole) const Q_DECL_OVERRIDE;
    int userColumnWidth(int userColumnRole, const QFontMetrics &fontMetrics) const Q_DECL_OVERRIDE;
    Qt::ItemFlags fileItemDisableFlags() const Q_DECL_OVERRIDE;
    DUrl mimeDataUrl() const Q_DECL_OVERRIDE;

    bool canRedirectionFileUrl() const Q_DECL_OVERRIDE;
    DUrl redirectedFileUrl() const Q_DECL_OVERRIDE;
};

#endif // DCLIPBOARDFILEINFO_H
