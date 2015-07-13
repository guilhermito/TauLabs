/**
 ******************************************************************************
 * @file       mesadownloader.h
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

#ifndef MESADOWNLOADER_H
#define MESADOWNLOADER_H

#define OPENGL32_LINK "http://forum.taulabs.org/public_files/opengl32"
#define OPENGL64_LINK "http://forum.taulabs.org/public_files/opengl64"

#include <QObject>

#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSysInfo>
#include <QMessageBox>
#include <QEventLoop>

#include <customsplash.h>

class MesaDownloader : public QObject
{
    Q_OBJECT
public:
    explicit MesaDownloader(CustomSplash *splash, QObject *parent = 0);
    virtual ~MesaDownloader();
    bool startDownload();

signals:
    void downloaded();
    void progress(QString);

private slots:
    void downloadProgress(qint64 downloaded,qint64 total);
    void fileDownloaded(QNetworkReply* pReply);

private:
    QNetworkAccessManager m_WebCtrl;
    QUrl m_fileUrl;
    CustomSplash *m_splash;
    bool m_downloadSuccess;


};

#endif // MESADOWNLOADER_H
