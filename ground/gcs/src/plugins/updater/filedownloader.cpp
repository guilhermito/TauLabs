/**
 ******************************************************************************
 * @file
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2014
 * @addtogroup [Group]
 * @{
 * @addtogroup
 * @{
 * @brief [Brief]
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

#include "filedownloader.h"

FileDownloader::FileDownloader(QObject *parent) :
    QObject(parent)
{
    connect(&m_WebCtrl, SIGNAL(finished(QNetworkReply*)),
            SLOT(fileDownloaded(QNetworkReply*)));


}

FileDownloader::~FileDownloader()
{

}

void FileDownloader::fileDownloaded(QNetworkReply* pReply)
{
    bool success = true;
    if(pReply->error() != QNetworkReply::NoError)
        success = false;
    else
        m_DownloadedData = pReply->readAll();
    //emit a signal
    pReply->deleteLater();
    emit downloaded(success);
}

QByteArray FileDownloader::downloadedData() const
{
    return m_DownloadedData;
}

void FileDownloader::start(QUrl url)
{
    request = QNetworkRequest(url);
    QNetworkReply *reply = m_WebCtrl.get(request);
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SIGNAL(downloadProgress(qint64, qint64)));
}
