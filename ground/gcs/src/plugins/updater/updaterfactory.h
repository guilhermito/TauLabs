/**
 ******************************************************************************
 *
 * @file       uavsettingsimportexportfactory.h
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2013-2014
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup Updater Plugin
 * @{
 * @brief Updater Plugin Factory
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
#ifndef UPDATERFACTORY_H
#define UPDATERFACTORY_H
#include "updater_global.h"
#include <QObject>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QHash>
#include <QUrl>
#include "uavobjectutil/uavobjectutilmanager.h"
#include <QDomDocument>
#include "uavtalk/telemetrymanager.h"
#include "extensionsystem/pluginmanager.h"
#include "icore.h"
#include "mainwindow.h"
#include "slimgcs/tbssplashdialog.h"
#include "filedownloader.h"

class UPDATER_EXPORT UpdaterFactory : public QObject
{
    Q_OBJECT

public:
    UpdaterFactory(QObject *parent = 0);
    ~UpdaterFactory();
    enum status {STATUS_IDLE, STATUS_DOWNLOADING_UPDATER, STATUS_DOWNLOADING_INFO, STATUS_DOWNLOADING_AGENT, STATUS_DOWNLOADING_FULL_GCS, STATUS_DOWNLOADING_FW, STATUS_DOWNLOAD_FAIL};
    enum softType {SOFT_GCS, SOFT_TBS_AGENT, SOFT_UPDATER};
    struct softData {
        QString name;
        QUrl link;
        QDate date;
        QString hash;
    };

    struct updateInfo {
        QString uavoHash;
        QHash<softType, softData> common;
        QHash<qint32, softData> embeded;
    };
    softType getCurrentSoftwareType() const;
    void setCurrentSoftwareType(const softType &value);
    bool checkIfCurrentSoftwareNeedsUpdate(updateInfo currentInfo);
    bool checkIfFirmwareNeedsUpdate(updateInfo currentInfo);
private:
    QString downloadText;
    QTimer updateTimer;
    QNetworkAccessManager * manager;
    QString test();
    softData domElementToStruct(QDomElement element);
    softType currentSoftwareType;
    ExtensionSystem::PluginManager *pm;
    TelemetryManager *telMngr;
    UAVObjectUtilManager* utilMngr;
    Core::Internal::MainWindow *mainwindow;
    TBSSplashDialog *dialog;
    bool backupUVOSettingsFromBoards();
    bool dontShowSoftwareUpdate;
    status currentStatus;
    FileDownloader fileDownloader;
    QString currentPlattform;
    updateInfo currentInfo;
    void splashError(QString text);
private slots:
    void onUpdateTimerTimeout();
    void onXmlArrive(QNetworkReply *);
    updateInfo processXml(QByteArray);
    void downloadProgress(qint64, qint64);
    void downloadEnded(bool);
signals:

};

#endif // UPDATERFACTORY_H
