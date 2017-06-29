/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#ifndef DFMGENERICPLUGINOBJECT_H
#define DFMGENERICPLUGINOBJECT_H

#include <QObject>
#include <QIcon>
#include <QQueue>

#include <dabstractfileinfo.h>

QT_BEGIN_NAMESPACE
class QTcpSocket;
class QUdpSocket;
QT_END_NAMESPACE

class DFMGenericPluginObject : public QObject
{
    Q_OBJECT

public:
    explicit DFMGenericPluginObject(QObject *parent = 0);

public slots:
    QList<QIcon> fileAdditionalIcon(const DAbstractFileInfoPointer &fileInfo);

private:
    enum NSFileStat {
        Invalid = -1,
        Uptodate = 0,
        Syncing = 1,
        Locked = 2,
        Readonly = 3,
        Unknow = 4
    };

    bool updateNSRootPathList();
    bool updateNSFileStat(const QString &filePath);

    void addNSFileToUpdateQueue(const QString &filePath);

    bool isNSRootChildFile(const QString &filePath) const;
    void notifyFileChanged(const QString &filePath);

    void onClientReadReady();
    void onServerReadReady();

    QStringList nsRootPaths;
    QMap<QString, NSFileStat> nsFileStat;

    QTcpSocket *client;
    QUdpSocket *server;

    QQueue<QString> nsUpdateFileStatQueue;
    QString lastQueryStatFile;
};

#endif // DFMGENERICPLUGINOBJECT_H
