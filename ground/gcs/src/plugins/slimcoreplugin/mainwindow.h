/**
 ******************************************************************************
 *
 * @file       mainwindow.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 *             Parts by Nokia Corporation (qt-info@nokia.com) Copyright (C) 2009.
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2013
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core_global.h"

#include "eventfilteringmainwindow.h"

#include <QtCore/QMap>
#include <QSettings>

#include <QWidget>
#include <QGridLayout>
#include <QSizeGrip>
#include <QPixmap>
#include "slimgcs/mainwidget.h"

#include "slimgcs/windowtitlebar.h"
#include "slimgcs/tbssplashdialog.h"

QT_BEGIN_NAMESPACE
class QSettings;
class QShortcut;
class QToolButton;
class MyTabWidget;
QT_END_NAMESPACE

namespace Core {

class ActionManager;
class BaseMode;
class BaseView;
class IConfigurablePlugin;
class IContext;
class IMode;
class IWizard;
class ConnectionManager;
class BoardManager;
class MessageManager;
class MimeDatabase;
class ModeManager;
class RightPaneWidget;
class SettingsDatabase;
class UniqueIDManager;
class VariableManager;
class ThreadManager;
class ViewManagerInterface;
class UAVGadgetManager;
class UAVGadgetInstanceManager;
class GlobalMessaging;

namespace Internal {

//class ActionManagerPrivate;
class CoreImpl;
class GeneralSettings;
//class ShortcutSettings;
//class WorkspaceSettings;
class VersionDialog;
class AuthorsDialog;

class CORE_EXPORT MainWindow : public EventFilteringMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    bool init(QString *errorMessage);
    void extensionsInitialized();
    void shutdown();

    void addContextObject(IContext *contex){}
    void removeContextObject(IContext *contex){}
    void readSettings(QSettings* qs = 0, bool workspaceDiffOnly = false){}
    void saveSettings(QSettings* qs = 0){}
    void readSettings(IConfigurablePlugin* plugin, QSettings* qs = 0){}
    void saveSettings(IConfigurablePlugin* plugin, QSettings* qs = 0){}
    void deleteSettings(){}

    TBSSplashDialog * tbsDialog(){return m_tbsDialog;}

    Core::ActionManager *actionManager() const{return NULL;}
    Core::UniqueIDManager *uniqueIDManager() const;
    Core::MessageManager *messageManager() const{return NULL;}
    Core::GlobalMessaging *globalMessaging() const{return NULL;}
    UAVGadgetInstanceManager *uavGadgetInstanceManager() const;
    Core::ConnectionManager *connectionManager() const;
    Core::BoardManager *boardManager() const;
    Core::VariableManager *variableManager() const{return NULL;}
    Core::ThreadManager *threadManager() const;
    Core::ModeManager *modeManager() const {return NULL;}
    Core::MimeDatabase *mimeDatabase() const {return NULL;}
    QSettings *settings(QSettings::Scope scope) const {return NULL;}
    inline SettingsDatabase *settingsDatabase() const { return NULL; }
    IContext * currentContextObject() const {return NULL;}
    void addAdditionalContext(int context) {}
    void removeAdditionalContext(int context) {}
    bool hasContext(int context) const {return true;}
    void updateContext(){}
    MainWidget * mainWidget(){return childWidget;}
signals:
    void windowActivated();
    void splashMessages(QString);
public slots:
    bool showOptionsDialog(const QString &category = QString(),
                           const QString &page = QString(),
                           QWidget *parent = 0);

    bool showWarningWithOptions(const QString &title, const QString &text,
                                const QString &details = QString(),
                                const QString &settingsCategory = QString(),
                                const QString &settingsId = QString(),
                                QWidget *parent = 0);

protected:
    virtual void changeEvent(QEvent *e);
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void aboutTauLabsGCS();
    void aboutTauLabsAuthors();
    void destroyVersionDialog();
    void destroyAuthorsDialog();
    void showHelp();

private:
    void updateContextObject(IContext *context);
    CoreImpl *m_coreImpl;
    UniqueIDManager *m_uniqueIDManager;
    ThreadManager *m_threadManager;
    QList<UAVGadgetManager*> m_uavGadgetManagers;
    UAVGadgetInstanceManager *m_uavGadgetInstanceManager;
    ConnectionManager *m_connectionManager;
    BoardManager *m_boardManager;
    VersionDialog *m_versionDialog;
    AuthorsDialog *m_authorsDialog;

    void setWindowTitle(const QString &title);
    TBSSplashDialog *m_tbsDialog;
  signals:
    void WindowTitleChanged();

  protected:
    void showEvent  (QShowEvent   *event);
    void paintEvent (QPaintEvent  *event);
    void resizeEvent(QResizeEvent *event);

  private:
    QGridLayout     m_MainLayout ;
    WindowTitleBar  m_TitleBar   ;
    QSizeGrip       m_SizeGrip   ;
    QPixmap        *m_Cache      ;
    MainWidget     *childWidget  ;

    void CenterOnScreen();
};

} // namespace Internal
} // namespace Core

#endif // MAINWINDOW_H
