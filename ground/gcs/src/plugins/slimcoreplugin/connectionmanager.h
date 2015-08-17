/**
 ******************************************************************************
 *
 * @file       connectionmanagerslim.h
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2014
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup CorePlugin Core Plugin
 * @{
 * @brief The Core GCS plugin
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "QWidget"
#include "mainwindow.h"
#include "generalsettings.h"
#include <slimcoreplugin/iconnection.h>
#include <QtCore/QVector>
#include <QtCore/QIODevice>
#include <QtCore/QLinkedList>
#include <QPushButton>
#include <QPointer>

#include "core_global.h"

namespace Core {

    class IConnection;
    class IDevice;

namespace Internal {
    class MainWindow;
} // namespace Internal


class DevListItem
{
public:
    DevListItem(IConnection *c, IDevice *d) :
        connection(c), device(d) { }

    DevListItem() : connection(NULL) { }

    QString getConName() {
        if (connection == NULL || device.isNull())
            return "";
        return connection->shortName() + ": " + device->getDisplayName();
    }

    bool operator==(const DevListItem &rhs) {
        return connection == rhs.connection && device == rhs.device;
    }

    IConnection *connection;
    QPointer<IDevice> device;
};


class CORE_EXPORT ConnectionManager : public QWidget
{
    Q_OBJECT

public:
    ConnectionManager(Internal::MainWindow *mainWindow);
    virtual ~ConnectionManager();

    void init();

    QIODevice* getCurrentConnection() { return m_ioDev; }
    DevListItem getCurrentDevice() { return m_connectionDevice; }
    DevListItem findDevice(const QString &devName);

    QLinkedList<DevListItem> getAvailableDevices() { return m_devList; }

    bool isConnected() { return m_ioDev != 0; }

    bool connectDevice(DevListItem device);
    bool disconnectDevice();
    void suspendPolling();
    void resumePolling();
    bool getAutoconnect(){return true;}
protected:
    void updateConnectionList(IConnection *connection);
    void registerDevice(IConnection *conn, IDevice *device);
    void updateConnections();

signals:
    void deviceConnected(QIODevice *device);
    void deviceAboutToDisconnect();
    void deviceDisconnected();
    void availableDevicesChanged(const QLinkedList<Core::DevListItem> devices);

public slots:
    void telemetryConnected();
    void telemetryDisconnected();

private slots:
    void objectAdded(QObject *obj);
    void aboutToRemoveObject(QObject *obj);

    void devChanged(IConnection *connection);

    void onConnectionDestroyed(QObject *obj);
    void connectionsCallBack(); //used to call devChange after all the plugins are loaded

protected:
    QLinkedList<DevListItem> m_devList;
    QList<IConnection*> m_connectionsList;

    //currently connected connection plugin
    DevListItem m_connectionDevice;

    //currently connected QIODevice
    QIODevice *m_ioDev;

private:
    bool connectDevice();
    bool polling;
    Internal::MainWindow *m_mainWindow;
    QList <IConnection *> connectionBackup;

};

} //namespace Core

#endif // CONNECTIONMANAGER_H
