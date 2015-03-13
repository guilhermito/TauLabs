/**
 ******************************************************************************
 *
 * @file       updaterfactory.cpp
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

#include "updaterfactory.h"
#include <QtPlugin>

// for menu item
#include "gcsversioninfo.h"
#include <coreconstants.h>
#include <actionmanager/actionmanager.h>
#include <icore.h>
#include <QUrl>
#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkReply>
// for XML object
#include <QFile>
#include <QProcess>
#include <QDate>

UpdaterFactory::~UpdaterFactory()
{
    // Do nothing
}

void UpdaterFactory::onUpdateTimerTimeout()
{
    manager->get(QNetworkRequest(QUrl("http://www.files.jbtecnologia.com/tbsupdates.xml")));
}

void UpdaterFactory::onXmlArrive(QNetworkReply *reply)
{
    qDebug()<<"onXmlArrive";
    qDebug()<<"PLATTFORM"<<currentPlattform;
    updateTimer.stop();
    bool boardsBackuped;
    if(reply->error() == QNetworkReply::NoError)
        currentInfo = processXml(reply->readAll());
    else
        qDebug() << "Update Factory xml fetch error:" << reply->errorString();
    if(checkIfCurrentSoftwareNeedsUpdate(currentInfo) && !dontShowSoftwareUpdate) {
        QEventLoop loop;
        dialog->setup(TBSSplashDialog::TYPE_QUESTION, TBSSplashDialog::BUTTONS_OK | TBSSplashDialog::BUTTONS_CANCEL, "New software version found, do you wish to upgrade now?", false,TBSSplashDialog::BUTTONS_NO_BUTTONS, 0, this, true);
        dialog->show();
        connect(dialog, SIGNAL(cancelClicked(QObject*)), &loop, SLOT(quit()));
        connect(dialog, SIGNAL(okClicked(QObject*)), &loop, SLOT(quit()));
        loop.exec();
        dialog->close();
        if(dialog->getLastButtonClicked() & TBSSplashDialog::DONT_SHOW_AGAIN) {
            qDebug()<<"DONTSHOWAFAIN";
            dontShowSoftwareUpdate = true;
        }
        if(dialog->getLastButtonClicked() & TBSSplashDialog::BUTTONS_CANCEL) {
            updateTimer.start();
            return;
        }
        connect(&fileDownloader, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)), Qt::UniqueConnection);
        connect(&fileDownloader, SIGNAL(downloaded(bool)), this, SLOT(downloadEnded(bool)), Qt::UniqueConnection);
        switch (currentSoftwareType) {
        case SOFT_GCS:
            currentStatus = STATUS_DOWNLOADING_FULL_GCS;
            break;
        case SOFT_TBS_AGENT:
            currentStatus = STATUS_DOWNLOADING_AGENT;
            break;
        default:
            break;
        }
        currentStatus = STATUS_DOWNLOADING_AGENT;
        dialog->setup(TBSSplashDialog::TYPE_WAIT, 0, "Starting Download", false, TBSSplashDialog::BUTTONS_NO_BUTTONS, 0, this, false);
        fileDownloader.start(QUrl(currentInfo.common.value(currentSoftwareType).link.toString() + "agent2.txt"));

        dialog->show();
        updateTimer.stop();
        return;
        qDebug()<<"HASH"<<currentInfo.uavoHash;
        if(currentInfo.uavoHash != QString(Core::Constants::UAVOSHA1_STR)) {
            qDebug()<<"NEEDS BACKUP" <<QSysInfo::WordSize;
            boardsBackuped = backupUVOSettingsFromBoards();
        }
        //updateThis(boardBackuped);
       // else
       if(!dontShowSoftwareUpdate) {


        }
    }
    else if(telMngr->isConnected()) {

    }
    updateTimer.start();
}

UpdaterFactory::updateInfo UpdaterFactory::processXml(QByteArray xmlArray)
{
    updateInfo info;
    QDomDocument doc("Last_Versions");
    doc.setContent(xmlArray);
    QDomElement root = doc.documentElement();
    QDomElement version = root.firstChildElement("current_version_data");
    info.uavoHash = version.attribute("uavo_hash");
    QDomNodeList boards = root.elementsByTagName("Hardware"); 
    for(int x = 0; x < boards.count(); ++x) {
        info.embeded.insert(boards.at(x).toElement().attribute("hw_id").toLong(), domElementToStruct(boards.at(x).toElement()));
    }
    QDomElement common = root.firstChildElement(currentPlattform);
    QDomElement agent = common.firstChildElement("agent");
    if(!agent.isNull())
    {
        info.common.insert(SOFT_TBS_AGENT, domElementToStruct(agent));
    }
    QDomElement gcs = common.firstChildElement("gcs");
    if(!gcs.isNull())
    {
        info.common.insert(SOFT_GCS, domElementToStruct(gcs));
    }
    QDomElement updater = common.firstChildElement("updater");
    if(!updater.isNull())
    {
        info.common.insert(SOFT_UPDATER, domElementToStruct(updater));
    }
    return info;
}

void UpdaterFactory::downloadProgress(qint64 current, qint64 total)
{
    qDebug()<< current<<total << (current * 100)/total;
    switch (currentStatus) {
    case STATUS_DOWNLOADING_AGENT:
        dialog->updateText(QString("Downloading TBS agent\nProgress %0%").arg(((current * 100) / total)));
        break;
    case STATUS_DOWNLOADING_FULL_GCS:
        dialog->updateText(QString("Downloading Tau Labs GCS\nProgress %0%").arg(((current * 100) / total)));
        break;
    case STATUS_DOWNLOADING_FW:
        dialog->updateText(QString("Downloading Firmware\nProgress %0%").arg(((current * 100) / total)));
        break;
    case STATUS_DOWNLOADING_UPDATER:
        dialog->updateText(QString("Downloading updater binary\nProgress %0%").arg(((current * 100) / total)));
        break;
    case STATUS_DOWNLOADING_INFO:
        dialog->updateText(QString("Downloading information file\nProgress %0%").arg(((current * 100) / total)));
        break;
    default:
        break;
    }
}

void UpdaterFactory::downloadEnded(bool success)
{
    if(!success)
    {
        splashError("File failed to download!");
        currentStatus = STATUS_IDLE;
        updateTimer.start();
        return;
    }
    qDebug()<< "downloadEnded";
    QFile file;
    switch (currentStatus) {
    case STATUS_DOWNLOADING_AGENT:
        currentStatus = STATUS_DOWNLOADING_UPDATER;
        fileDownloader.start(QUrl(currentInfo.common.value(SOFT_UPDATER).link.toString()+"updater"));
        break;
    case STATUS_DOWNLOADING_FULL_GCS:
        break;
    case STATUS_DOWNLOADING_FW:
        break;
    case STATUS_DOWNLOADING_UPDATER:
        qDebug() << "TEMP" << QDir::tempPath();
        file.setFileName(QDir::tempPath() + QDir::separator() + "updater");
        if(!file.open(QIODevice::WriteOnly)) {
            splashError("Could not save downloaded file. Aborting");
            currentStatus = STATUS_IDLE;
            return;
        }
       // file.setPermissions(QFile::ReadGroup | QFile::ReadOwner | QFile::ReadOther | QFile::ExeOther | QFile::ExeGroup);
        file.write(fileDownloader.downloadedData());
        file.setPermissions(QFile::WriteOwner | QFile::ExeOwner | QFile::ExeUser | QFile::WriteGroup | QFile::WriteOther | QFile::ReadGroup | QFile::ReadOwner | QFile::ReadOther |QFile::ExeOther | QFile::ExeGroup);
        dialog->close();
        file.close();
        break;
    case STATUS_DOWNLOADING_INFO:
        break;
    default:
        break;
    }
}

UpdaterFactory::UpdaterFactory(QObject *parent):QObject(parent),dontShowSoftwareUpdate(false), currentStatus(STATUS_IDLE)
{
#ifdef SLIM_GCS
    setCurrentSoftwareType(UpdaterFactory::SOFT_TBS_AGENT);
#endif
#ifdef FULL_GCS
    setCurrentSoftwareType(UpdaterFactory::SOFT_GCS);
#endif
    mainwindow = (Core::Internal::MainWindow*)Core::ICore::instance()->mainWindow();
    dialog = mainwindow->tbsDialog();
    manager = new QNetworkAccessManager(this);
    pm=ExtensionSystem::PluginManager::instance();
    Q_ASSERT(pm);
    // Get telemetry manager and make sure it is valid
    telMngr = pm->getObject<TelemetryManager>();
    utilMngr = pm->getObject<UAVObjectUtilManager>();
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onXmlArrive(QNetworkReply*)) );
    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(onUpdateTimerTimeout()));
    updateTimer.setInterval(1000);
    updateTimer.start();
#ifdef Q_OS_LINUX
    currentPlattform = "linux";
#elif defined(Q_OS_WIN)
    currentPlattform = "windows";
#elif defined(Q_OS_OSX)
    currentPlattform = "osx";
#endif
    currentPlattform.append(QString::number(QSysInfo::WordSize));
  //  test();
}

QString UpdaterFactory::test()
{
    QString gcsDate = QString(Core::Constants::GCS_REVISION_STR).split(QRegExp("\\s")).at(1);
    QString gcsHash = QString(Core::Constants::GCS_REVISION_STR).split(QRegExp("\\s")).at(0);
    qDebug() << gcsDate;
    qDebug() << "DATE" << Core::Constants::t << Core::Constants::GCS_REVISION_STR;
    QDate thisDate = QDate::fromString(QString(Core::Constants::t), "MMM dd yyyy");
    QString thisUAVOHash = QString(Core::Constants::UAVOSHA1_STR);
    qDebug() << thisDate << thisUAVOHash << gcsHash;
    QProcess *process = new QProcess(this);
    QFile file("/home/jose/Code/guiTau/tbsupdates.xml");
    if(!file.open(QIODevice::ReadWrite))
        qDebug() << "error opening file";
    // create an XML root
    QDomDocument doc("Last_Versions");
    QDomElement root = doc.createElement("last_versions");
    doc.appendChild(root);

    // add hardware, firmware and GCS version info
    QDomElement versionInfo = doc.createElement("current_version_data");
    versionInfo.setAttribute("uavo_hash", thisUAVOHash);
    root.appendChild(versionInfo);

    QDomElement hardware = doc.createElement("Hardware");
    hardware.setAttribute("hw_id", "SAMPLE_ID");
    root.appendChild(hardware);

    QDomElement fw = doc.createElement("firmware");
    fw.setAttribute("date", gcsDate);
    fw.setAttribute("hash", "HASH_STR");
    fw.setAttribute("name", "NAME_STR");
    fw.setAttribute("link", "NAME_STR");
    hardware.appendChild(fw);

    QDomElement settings = doc.createElement("settings");
    settings.setAttribute("date", gcsDate);
    settings.setAttribute("hash", "HASH_STR");
    settings.setAttribute("name", "NAME_STR");
    settings.setAttribute("link", "NAME_STR");
    hardware.appendChild(settings);

    QDomElement linux32 = doc.createElement("linux32");
    QDomElement gcs = doc.createElement("gcs");
    gcs.setAttribute("date", gcsDate);
    gcs.setAttribute("hash", "HASH_STR");
    gcs.setAttribute("name", "NAME_STR");
    gcs.setAttribute("link", "NAME_STR");
    linux32.appendChild(gcs);

    QDomElement agent = doc.createElement("agent");
    agent.setAttribute("date", gcsDate);
    agent.setAttribute("hash", "HASH_STR");
    agent.setAttribute("name", "NAME_STR");
    agent.setAttribute("link", "NAME_STR");
    linux32.appendChild(agent);

    QDomElement updater = doc.createElement("updater");
    updater.setAttribute("date", gcsDate);
    updater.setAttribute("hash", "HASH_STR");
    updater.setAttribute("name", "NAME_STR");
    updater.setAttribute("link", "NAME_STR");
    linux32.appendChild(updater);

    QDomElement androidgcs = doc.createElement("androidgcs");
    androidgcs.setAttribute("date", gcsDate);
    androidgcs.setAttribute("hash", "HASH_STR");
    androidgcs.setAttribute("name", "NAME_STR");
    androidgcs.setAttribute("link", "NAME_STR");
    root.appendChild(androidgcs);
    root.appendChild(linux32);

    file.write(doc.toString(4).toLatin1());
    file.close();
    process->start("sh /home/jose/Code/guiTau/upload.sh");
    return doc.toString(4);
}

UpdaterFactory::softData UpdaterFactory::domElementToStruct(QDomElement element)
{
    softData data;
    QString dateString = element.attribute("date");
    data.date = QDate::fromString(dateString, "yyyyMMdd");
    data.hash = element.attribute("hash");
    data.link = QUrl(element.attribute("link"));
    data.name = element.attribute("name");
    qDebug() << dateString <<data.date;
    return data;
}

bool UpdaterFactory::backupUVOSettingsFromBoards()
{
    return true;
}

void UpdaterFactory::splashError(QString text)
{
    QEventLoop loop;
    connect(dialog, SIGNAL(okClicked(QObject*)), &loop, SLOT(quit()));
    dialog->setup(TBSSplashDialog::TYPE_ERROR, TBSSplashDialog::BUTTONS_OK,text , false, TBSSplashDialog::BUTTONS_OK, 5, this, false);
    loop.exec();
    dialog->close();
}

UpdaterFactory::softType UpdaterFactory::getCurrentSoftwareType() const
{
    return currentSoftwareType;
}

void UpdaterFactory::setCurrentSoftwareType(const softType &value)
{
    currentSoftwareType = value;
}

bool UpdaterFactory::checkIfCurrentSoftwareNeedsUpdate(UpdaterFactory::updateInfo info)
{
    QDate thisDate = QDate::fromString(QString(Core::Constants::t), "MMM dd yyyy");
    qDebug()<<thisDate << info.common.value(currentSoftwareType).date;
    if(thisDate < info.common.value(currentSoftwareType).date) {
        qDebug() << "NEEDS UPDATE";
        return true;
    }
    else
        return false;
}

bool UpdaterFactory::checkIfFirmwareNeedsUpdate(UpdaterFactory::updateInfo info)
{
    if(!telMngr->isConnected())
        return false;
    deviceDescriptorStruct device;
    if(utilMngr->getBoardDescriptionStruct(device)) {
        qDebug()<<device.gitDate << device.boardID();
        return info.embeded.value(device.boardID()).date > QDate::fromString(device.gitDate); //TODO
    }
    else
        return false;
}

