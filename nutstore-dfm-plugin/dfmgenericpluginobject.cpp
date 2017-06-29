/**
 * Copyright (C) 2017 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include "dfmgenericpluginobject.h"

#include <dabstractfilewatcher.h>

#include <QTcpSocket>
#include <QUdpSocket>
#include <QDebug>

#define COMMAND_GET_NS_ROOTS "get_ns_roots"
#define COMMAND_QUERY_STAT "query_stat"
#define COMMAND_REFRESH_STAT "refresh_stat"
#define COMMAND_REFRESH_NS_ROOT "refresh_ns_root"

class NSCommand
{
public:
    NSCommand();
    NSCommand(const QString &name);

    inline QString name() const
    { return m_name;}
    inline QStringList keys() const
    { return m_data.keys();}
    inline bool contains(const QString &key) const
    { return m_data.contains(key);}
    inline QString value(const QString &key, const QString &defaultValue = QString()) const
    { return m_data.value(key, defaultValue);}
    inline QStringList values(const QString &key) const
    { return m_data.values(key);}

    inline void setName(const QString &name)
    { m_name = name;}
    inline void addValue(const QString &key, const QString &value)
    { m_data.insertMulti(key, value);}
    inline void removeValue(const QString &key, const QString &value)
    { m_data.remove(key, value);}
    inline void removeValue(const QString &key)
    { m_data.remove(key);}

    QByteArray toData() const;
    static NSCommand fromData(const QByteArray &data);

private:
    QString m_name;
    QMultiMap<QString, QString> m_data;
};

NSCommand::NSCommand()
    : NSCommand(QString())
{

}

NSCommand::NSCommand(const QString &name)
    : m_name(name)
{

}

QByteArray NSCommand::toData() const
{
    QByteArray data;

    data.append(m_name.toLocal8Bit()).append('\n');

    for (const QString &key : m_data.keys()) {
        data.append(key.toLocal8Bit());

        for (const QString &value : m_data.values(key)) {
            data.append('\t').append(value.toLocal8Bit());
        }

        data.append('\n');
    }

    data.append("done\n");

    return data;
}

NSCommand NSCommand::fromData(const QByteArray &data)
{
    const QByteArrayList &list = data.split('\n');

    if (list.isEmpty())
        return NSCommand();

    NSCommand command(QString::fromLocal8Bit(list.first()));

    for (int i = 1; i < list.count(); ++i) {
        const QString &line = QString::fromLocal8Bit(list.at(i));

        if (line == "done")
            break;

        const QStringList &key_values_list = line.split("\t");

        if (key_values_list.isEmpty())
            continue;

        const QString &key = key_values_list.first();

        for (int j = 1; j < key_values_list.count(); ++j)
            command.addValue(key, key_values_list.at(j));
    }

    return command;
}

DFMGenericPluginObject::DFMGenericPluginObject(QObject *parent)
    : QObject(parent)
{
    client = new QTcpSocket(this);
    server = new QUdpSocket(this);

    client->connectToHost(QHostAddress::LocalHost, 19080, QIODevice::ReadWrite);

    connect(client, &QTcpSocket::connected, this, &DFMGenericPluginObject::updateNSRootPathList);
    connect(client, &QTcpSocket::readyRead, this, &DFMGenericPluginObject::onClientReadReady);
    connect(client, static_cast<void(QTcpSocket::*)(QAbstractSocket::SocketError)>(&QTcpSocket::error), this, [this] {
        qWarning() << "The localhost:19080 tcp socket error:" << client->errorString();

        // reset
        if (!lastQueryStatFile.isEmpty())
            lastQueryStatFile = QString();
    });

    if (!server->bind(QHostAddress::LocalHost, 19081)) {
        qWarning() << "Bind the localhost:19081 failed, error message:" << server->errorString();
        server->deleteLater();
        server = Q_NULLPTR;
    } else {
        connect(server, &QUdpSocket::readyRead, this, &DFMGenericPluginObject::onServerReadReady);
    }
}

QList<QIcon> DFMGenericPluginObject::fileAdditionalIcon(const DAbstractFileInfoPointer &fileInfo)
{
    QList<QIcon> list;
    const QString &file_path = fileInfo->redirectedFileUrl().toLocalFile();

    if (file_path.isEmpty())
        return list;

    NSFileStat file_stat = nsFileStat.value(file_path, Invalid);

    switch (file_stat) {
    case Invalid:
        if (nsRootPaths.isEmpty())
            break;

        if (isNSRootChildFile(file_path))
            addNSFileToUpdateQueue(file_path);

        break;
    case Uptodate:
        list << QIcon::fromTheme("emblem-nutstore-uptodate");
        break;
    case Syncing:
        list << QIcon::fromTheme("emblem-nutstore-syncing");
        break;
    case Locked:
        list << QIcon::fromTheme("emblem-nutstore-locked");
        break;
    case Readonly:
        list << QIcon::fromTheme("emblem-nutstore-readonly");
        break;
    case Unknow:
        break;
    }

    return list;
}

bool DFMGenericPluginObject::updateNSRootPathList()
{
    if (client->state() != QTcpSocket::ConnectedState)
        return false;

    NSCommand command(COMMAND_GET_NS_ROOTS);
    const QByteArray &message = command.toData();
    qint64 size = client->write(message);

    if (size != message.size())
        return false;

    return client->flush();
}

bool DFMGenericPluginObject::updateNSFileStat(const QString &filePath)
{
    if (client->state() != QTcpSocket::ConnectedState)
        return false;

    NSCommand command(COMMAND_QUERY_STAT);

    command.addValue("path", filePath);

    const QByteArray &message = command.toData();
    qint64 size = client->write(message);

    if (size != message.size())
        return false;

    // record
    lastQueryStatFile = filePath;

    return client->flush();
}

void DFMGenericPluginObject::addNSFileToUpdateQueue(const QString &filePath)
{
    if (nsUpdateFileStatQueue.isEmpty())
        updateNSFileStat(filePath);

    nsUpdateFileStatQueue.append(filePath);
}

bool DFMGenericPluginObject::isNSRootChildFile(const QString &filePath) const
{
    for (const QString &root_path : nsRootPaths)
        if (filePath.startsWith(root_path))
            return true;

    return false;
}

void DFMGenericPluginObject::notifyFileChanged(const QString &filePath)
{
    const DUrl &url = DUrl::fromLocalFile(filePath);
    // notify file view to refresh
    DAbstractFileWatcher::ghostSignal(url.parentUrl(), &DAbstractFileWatcher::fileAttributeChanged, url);
}

void DFMGenericPluginObject::onClientReadReady()
{
    const QByteArray &data = client->readAll();
    const NSCommand &command = NSCommand::fromData(data);

    if (command.name() == "rsp") {
        const QString &stat_value = command.value("stat");

        if (stat_value.isEmpty()) {
            if (!command.contains("path"))
                return;

            // save old paths
            QStringList to_update_paths = nsRootPaths;

            nsRootPaths = command.values("path");

            // add new paths
            to_update_paths << nsRootPaths;

            for (const QString &path : to_update_paths) {
                notifyFileChanged(path);
            }

            for (const QString &file_path : nsFileStat.keys()) {
                if (!isNSRootChildFile(file_path)) {
                    NSFileStat stat = nsFileStat.take(file_path);

                    if (stat != Invalid && stat != Unknow) {
                        notifyFileChanged(file_path);
                    }
                }
            }

            return;
        }

        if (lastQueryStatFile.isEmpty()) {
            if (!nsUpdateFileStatQueue.isEmpty()) {
                updateNSFileStat(nsUpdateFileStatQueue.dequeue());
            }

            return;
        }

        bool ok = false;
        int file_stat = stat_value.toInt(&ok);

        NSFileStat old_stat = nsFileStat.value(lastQueryStatFile, Unknow);
        NSFileStat new_stat = Unknow;

        if (ok && file_stat >= Uptodate  && file_stat <= Readonly)
            new_stat = (NSFileStat)file_stat;

        if (old_stat != new_stat) {
            nsFileStat[lastQueryStatFile] = new_stat;
            notifyFileChanged(lastQueryStatFile);
        }

        // reset
        if (!lastQueryStatFile.isEmpty())
            lastQueryStatFile = QString();

        if (!nsUpdateFileStatQueue.isEmpty()) {
            updateNSFileStat(nsUpdateFileStatQueue.dequeue());
        }
    }
}

void DFMGenericPluginObject::onServerReadReady()
{
    char data[2048] = {};
    qint64 size = server->readDatagram(data, 2047);

    if (size <= 0) {
        return;
    }

    const NSCommand &command = NSCommand::fromData(QByteArray(data, size));

    if (command.name() == COMMAND_REFRESH_STAT) {
        const QStringList &paths = command.values("path");

        for (const QString &path : paths)
            addNSFileToUpdateQueue(path);
    } else if (command.name() == COMMAND_REFRESH_NS_ROOT) {
        updateNSRootPathList();
    }
}
