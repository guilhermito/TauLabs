/**
 ******************************************************************************
 * @file       mesadownloader.cpp
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2015
 * @addtogroup app
 * @{
 * @addtogroup
 * @{
 * @brief Downloads and copies to application directory the windows mesa driver
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

#include "mesadownloader.h"
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QProcess>

MesaDownloader::MesaDownloader(CustomSplash *splash, QObject *parent) :
    QObject(parent), m_splash(splash)
{
    connect(&m_WebCtrl, SIGNAL (finished(QNetworkReply*)), this, SLOT(fileDownloaded(QNetworkReply*)));
    m_splash->show();
}

MesaDownloader::~MesaDownloader() { }

bool MesaDownloader::startDownload()
{
    QString url;
#ifdef _WIN64
    url = QString(OPENGL64_LINK);
#else
    url = QString(OPENGL32_LINK);
#endif
    QEventLoop loop;
    connect(this, SIGNAL(downloaded()), &loop, SLOT(quit()));
    QNetworkReply *m_Reply = m_WebCtrl.get(QNetworkRequest(QUrl(url)));
    connect(m_Reply, SIGNAL(downloadProgress(qint64,qint64)), SLOT(downloadProgress(qint64,qint64)));
    loop.exec();
    return m_downloadSuccess;
}

void MesaDownloader::fileDownloaded(QNetworkReply *pReply) {
    m_splash->close();
    if(pReply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(NULL, "Download failed", QString("The mesa driver download failed with %0 error, the application will now close").arg(pReply->errorString()));
        m_downloadSuccess = false;
        emit downloaded();
        return;
    }
    pReply->deleteLater();
    emit downloaded();
    QFile file(QCoreApplication::applicationDirPath() + QDir::separator() + "opengl32.dll");
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(NULL, "File write failed", "Make sure the application directory is not read only");
        m_downloadSuccess = false;
        emit downloaded();
        return;
    }
    file.write(pReply->readAll());
    file.close();
    QMessageBox::warning(NULL, "Download completed", "This application will now restart");
    QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    m_downloadSuccess = true;
    emit downloaded();
}

void MesaDownloader::downloadProgress(qint64 downloaded, qint64 total)
{
    QString text;
    if(total > 0) {
        int percentage = (int)(downloaded * 100 / total);
        text = (QString::number(percentage) + "%");
    }
    else {
        text = (QString::number(downloaded) + "bytes");
    }
    m_splash->showMessage(QString("Downloading MESA driver. Progress:%0").arg(text));
    qApp->processEvents();
}

