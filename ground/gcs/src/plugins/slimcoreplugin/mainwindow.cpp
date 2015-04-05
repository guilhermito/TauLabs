/**
 ******************************************************************************
 *
 * @file       mainwindow.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 *             Parts by Nokia Corporation (qt-info@nokia.com) Copyright (C) 2009.
 * @author     Tau Labs, http://taulabs.org Copyright (C) 2012-2013
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup CorePlugin Core Plugin
 * @{
 * @brief Provides the GCS Main Window
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

#include "mainwindow.h"

#include "connectionmanager.h"
#include "boardmanager.h"
#include "coreimpl.h"
#include "coreconstants.h"
#include "plugindialog.h"
#include "authorsdialog.h"
#include "ioutputpane.h"
#include "icorelistener.h"
#include <QStyleFactory>
#include "threadmanager.h"
#include "uniqueidmanager.h"
#include "variablemanager.h"
#include "versiondialog.h"

#include <extensionsystem/pluginmanager.h>
#include <utils/hostosinfo.h>
#include <utils/pathchooser.h>
#include <utils/stylehelper.h>
#include <utils/xmlconfig.h>

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QSettings>
#include <QtCore/QtPlugin>
#include <QtCore/QUrl>

#include <QApplication>
#include <QCloseEvent>
#include <QMenu>
#include <QPixmap>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QPainter>
#include <QBitmap>
#include <QPushButton>
using namespace Core;
using namespace Core::Internal;
namespace Core
{
namespace Internal
{
MainWindow::MainWindow() :
    EventFilteringMainWindow(),
    m_coreImpl(new CoreImpl(this)),
    m_uniqueIDManager(new UniqueIDManager()),
    // keep this in sync with main() in app/main.cpp
    m_threadManager(new ThreadManager(this)),
    m_connectionManager(0),
    m_boardManager(0),
    m_versionDialog(0),
    m_authorsDialog(0),
    m_MainLayout (this),
    m_TitleBar   (this),
    m_SizeGrip   (this),
    m_Cache      (NULL),
    childWidget  (new MainWidget(this))
{
    setWindowTitle(QLatin1String(Core::Constants::GCS_NAME));
    if (!Utils::HostOsInfo::isMacHost())
        QApplication::setWindowIcon(QIcon(Core::Constants::ICON_TAULABS));
    QCoreApplication::setApplicationName(QLatin1String(Core::Constants::GCS_NAME));
    QCoreApplication::setApplicationVersion(QLatin1String(Core::Constants::GCS_VERSION_LONG));
    QCoreApplication::setOrganizationName(QLatin1String(Core::Constants::GCS_AUTHOR));
    QCoreApplication::setOrganizationDomain(QLatin1String("taulabs.org"));
    QSettings::setDefaultFormat(XmlConfig::XmlSettingsFormat);

    m_connectionManager = new ConnectionManager(this);

    m_boardManager = new BoardManager();
    connect(this       , SIGNAL(WindowTitleChanged()),
            &m_TitleBar, SLOT  (UpdateWindowTitle ()));

    resize(1000, 700);

    setWindowTitle(tr("TBS Tau Labs Agent"));

    setWindowFlags(Qt::FramelessWindowHint);

  #if QT_VERSION >= 0x040500
    #ifdef Q_WS_X11
      if(x11Info().isCompositingManagerRunning()) setAttribute(Qt::WA_TranslucentBackground);
    #else
      setAttribute(Qt::WA_TranslucentBackground);
    #endif
  #endif

    m_SizeGrip.setStyleSheet("image: none");

    m_MainLayout.setMargin (0); // No space between window's element and the window's border
    m_MainLayout.setSpacing(0); // No space between window's element

    setLayout(&m_MainLayout);
    m_MainLayout.addWidget(&m_TitleBar, 0, 0, 1, 1);
    m_MainLayout.addWidget(childWidget, 1, 0, 1,1);
    QLabel *dummy = new QLabel(this);
    dummy->setMaximumWidth(10);
    m_MainLayout.addWidget(dummy,2,0,1,1);
    m_MainLayout.setRowStretch(0, 1); // Put the title bar at the top of the window
    m_tbsDialog = new TBSSplashDialog();
}

MainWindow::~MainWindow()
{
    if (m_connectionManager)
	{
		m_connectionManager->disconnectDevice();
		m_connectionManager->suspendPolling();
	}

	hide();

    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    if (m_uavGadgetManagers.count() > 0) {
        foreach (UAVGadgetManager *mode, m_uavGadgetManagers)
        {
            delete mode;
        }
    }    delete m_uniqueIDManager;
    m_uniqueIDManager = 0;

    pm->removeObject(m_coreImpl);
    delete m_coreImpl;
    m_coreImpl = 0;
    delete m_Cache;
    if(m_tbsDialog)
        m_tbsDialog->deleteLater();
}

bool MainWindow::init(QString *errorMessage)
{
    Q_UNUSED(errorMessage)

    ExtensionSystem::PluginManager *pm = ExtensionSystem::PluginManager::instance();
    pm->addObject(m_coreImpl);
    m_connectionManager->init();
    m_boardManager->init();
    return true;
}

void MainWindow::extensionsInitialized()
{
    emit splashMessages(tr("Preparing to open core"));
    emit m_coreImpl->coreAboutToOpen();
    show();
    emit m_coreImpl->coreOpened();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    const QList<ICoreListener *> listeners =
        ExtensionSystem::PluginManager::instance()->getObjects<ICoreListener>();
    foreach (ICoreListener *listener, listeners) {
        if (!listener->coreAboutToClose()) {
            event->ignore();
            return;
        }
    }

    emit m_coreImpl->coreAboutToClose();

    event->accept();
}

bool MainWindow::showOptionsDialog(const QString &category,
                                   const QString &page,
                                   QWidget *parent)
{
    emit m_coreImpl->optionsDialogRequested();
    if (!parent)
        parent = this;
    //SettingsDialog dlg(parent, category, page);
   // return dlg.execDialog();
}

void MainWindow::showHelp()
{
    QDesktopServices::openUrl( QUrl(Constants::GCS_HELP, QUrl::StrictMode) );
}

UniqueIDManager *MainWindow::uniqueIDManager() const
{
    return m_uniqueIDManager;
}

ThreadManager *MainWindow::threadManager() const
{
     return m_threadManager;
}

ConnectionManager *MainWindow::connectionManager() const
{
    return m_connectionManager;
}

BoardManager *MainWindow::boardManager() const
{
    return m_boardManager;
}

UAVGadgetInstanceManager *MainWindow::uavGadgetInstanceManager() const
{
    return m_uavGadgetInstanceManager;
}

void MainWindow::changeEvent(QEvent *e)
{
    //QMainWindow::changeEvent(e);
    if (e->type() == QEvent::ActivationChange) {
        if (isActiveWindow()) {
            emit windowActivated();
        }
    } else if (e->type() == QEvent::WindowStateChange) {

    }
}

void MainWindow::shutdown()
{

}

inline int takeLeastPriorityUavGadgetManager(const QList<Core::UAVGadgetManager*> m_uavGadgetManagers) {
    int index = 0;
    return index;
}

void MainWindow::aboutTauLabsGCS()
{
    if (!m_versionDialog) {
        m_versionDialog = new VersionDialog(this);
        connect(m_versionDialog, SIGNAL(finished(int)),
                this, SLOT(destroyVersionDialog()));
    }
    m_versionDialog->show();
}

void MainWindow::destroyVersionDialog()
{
    if (m_versionDialog) {
        m_versionDialog->deleteLater();
        m_versionDialog = 0;
    }
}

void MainWindow::aboutTauLabsAuthors()
{
    if (!m_authorsDialog) {
        m_authorsDialog = new AuthorsDialog(this);
        connect(m_authorsDialog, SIGNAL(finished(int)),
                this, SLOT(destroyAuthorsDialog()));
    }
    m_authorsDialog->show();
}

void MainWindow::destroyAuthorsDialog()
{
    if (m_authorsDialog) {
        m_authorsDialog->deleteLater();
        m_authorsDialog = 0;
    }
}

// Display a warning with an additional button to open
// the debugger settings dialog if settingsId is nonempty.

bool MainWindow::showWarningWithOptions(const QString &title,
                                        const QString &text,
                                        const QString &details,
                                        const QString &settingsCategory,
                                        const QString &settingsId,
                                        QWidget *parent)
{
    Q_UNUSED(title);
    Q_UNUSED(text);
    Q_UNUSED(details);
    Q_UNUSED(settingsCategory);
    Q_UNUSED(settingsId);
    Q_UNUSED(parent);
    return false;
}
void MainWindow::setWindowTitle(const QString &title)
{
  QWidget::setWindowTitle(title);

  emit WindowTitleChanged();
}

void MainWindow::showEvent(QShowEvent *event)
{
  Q_UNUSED(event);

  CenterOnScreen();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  if(m_Cache != NULL)
  {
    QPainter painter(this);

    painter.drawPixmap(0, 0, *m_Cache);

#if QT_VERSION >= 0x040500
    if(!testAttribute(Qt::WA_TranslucentBackground)) setMask(m_Cache->mask());
#else
    setMask(m_Cache->mask());
#endif
  }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  Q_UNUSED(event);

  delete m_Cache;

  m_Cache = new QPixmap(size());

  m_Cache->fill(Qt::transparent);

  QPainter painter(m_Cache);
  QColor darkBlue;
  darkBlue.setNamedColor("0f0f0f");
  QColor lightBlue(177, 177, 203);

  /********** Window's background **********/
  QPolygon background;

  background << QPoint(           0,            16)
               << QPoint(          16,             0)
               << QPoint(width() -  1,             0)
               << QPoint(width() -  1, height() - 17)
               << QPoint(width() - 17, height() - 1)
               << QPoint(          0, height() -  1);

  painter.setPen  (QPen  (darkBlue));
  painter.setBrush(QBrush(darkBlue));

  painter.drawPolygon(background);
  /*****************************************/

  /********** Window's frame **********/
  QPolygon frame;

  frame << QPoint(           4,            20)
        << QPoint(          20,             4)
        << QPoint(width() -  4,             4)
        << QPoint(width() -  4, height() - 21)
        << QPoint(width() - 20, height() - 5)
        << QPoint(          4, height() -  5);

  painter.setPen  (QPen(lightBlue));
  painter.setBrush(Qt::NoBrush);

  painter.drawPolygon(frame);
  /*****************************************/

  painter.end();

  m_SizeGrip.move  (width() - 18, height() - 18);
  m_SizeGrip.resize(100, 100);
}

void MainWindow::CenterOnScreen()
{
  QDesktopWidget screen;

  QRect screenGeom = screen.screenGeometry(this);

  int screenCenterX = screenGeom.center().x();
  int screenCenterY = screenGeom.center().y();

  move(screenCenterX - width () / 2,
       screenCenterY - height() / 2);
}
}
}
