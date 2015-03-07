/**
 ******************************************************************************
 *
 * @file       configgadgetfactory.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup ConfigPlugin Config Plugin
 * @{
 * @brief The Configuration Gadget used to update settings in the firmware
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
#ifndef CONFIGGADGETFACTORY_H
#define CONFIGGADGETFACTORY_H

#include "config_global.h"
#include "extensionsystem/pluginmanager.h"
#include "slimcoreplugin/mainwindow.h"
#include "slimcoreplugin/slimgcs/mainwidget.h"
#include "slimcoreplugin/slimgcs/tlsplashdialog.h"
#include "slimcoreplugin/icore.h"
#include "hwcolibri.h"
#include "iboardtype.h"
#include "manualcontrolsettings.h"
#include "manualcontrolcommand.h"
#include "firmwareiapobj.h"
#include "slimcoreplugin/connectionmanager.h"
#include "configinputwidget.h"
#include "actuatorcommand.h"
#include "receiveractivity.h"

#define DEAD_MIN 0
#define DEAD_MAX 1

namespace Core {
class IUAVGadget;
class IUAVGadgetFactory;
}

using namespace Core;

class CONFIG_EXPORT ConfigGadgetFactory:  public QObject
{
   Q_OBJECT
public:

    ConfigGadgetFactory(QObject *parent = 0);
    ~ConfigGadgetFactory();
    float scaleChannel(int value, int max, int min, int neutral);
public slots:

private slots:
    void boardConnected();
    void boardDisconnected();
    void onReceiverTypeChanged(int);
    void onDialogOkClicked(QObject*);
    void onDialogCancelClicked(QObject*);
    void onTimeout();
    void onRebootMessage(QString text);
    void onManualControlSettingsChanged();
    void onManualControlCommandChanged();
    void onHardwareObjectChanged();
    void onSelectedFlightModeChangedUI(int, int);
    void onConfigStartRequested();
    void onConfigCancelRequested();
    void onConfigNextRequested();
    void onReceiverActivityUpdated();
    void onChannelDetected(int);
    void onDeadBandTimerTimeout();
private:
    struct channelsStruct
    {
        bool operator ==(const channelsStruct& rhs) const
        {
            return((group==rhs.group) &&(number==rhs.number));
        }
        int group;
        int number;
    }lastChannel;
    enum status {STATUS_IDLE, STATUS_DIALOG_REBOOT, STATUS_REBOOTING, STATUS_RC_CONFIG};
    enum rc_config_steps {  RC_WELCOME,
                            RC_DETECT_ROLL_CHANNEL,
                            RC_DETECT_ROLL_MAXVALUE,
                            RC_DETECT_ROLL_MINVALUE,
                            RC_DETECT_ROLL_NEUTRALVALUE,
                            RC_DETECT_PITCH_CHANNEL,
                            RC_DETECT_PITCH_MAXVALUE,
                            RC_DETECT_PITCH_MINVALUE,
                            RC_DETECT_PITCH_NEUTRALVALUE,
                            RC_DETECT_THROTTLE_CHANNEL,
                            RC_DETECT_THROTTLE_MAXVALUE,
                            RC_DETECT_THROTTLE_MINVALUE,
                            RC_DETECT_THROTTLE_NEUTRALVALUE,
                            RC_DETECT_YAW_CHANNEL,
                            RC_DETECT_YAW_MAXVALUE,
                            RC_DETECT_YAW_MINVALUE,
                            RC_DETECT_YAW_NEUTRALVALUE,
                            RC_DETECT_FLIGHTMODE_CHANNEL,
                            RC_DETECT_FLIGHTMODE_MAXVALUE,
                            RC_DETECT_FLIGHTMODE_MINVALUE,
                            RC_DETECT_FLIGHTMODE_NEUTRALVALUE,
                            RC_DEADBAND_QUESTION,
                            RC_DEADBAND_CALCULATE,
                            RC_FINISH,
                            RC_ABORTED,
                         };
    status currentStatus;
    ExtensionSystem::PluginManager *pm;
    Core::Internal::MainWindow *mainwindow;
    MainWidget *mainWidget;
    void setupInputTypes();
    ManualControlSettings *m_manualControlSettings;
    ManualControlCommand *m_manualControlCommand;
    TLSplashDialog *dialog;
    bool boardReboot();
    TelemetryManager *telMngr;
    FirmwareIAPObj *firmwareIap;
    ConnectionManager *conMngr;
    QTimer timeout;
    UAVObjectUtilManager *utilMngr;
    void delay(int ms);
    QList<int> channelOrder;
    UAVObject *hwObject;
    QHash<int, MainWidget::channels> intToChannel;
    InputConfigurator *inputConfiguration;
    void fastMdata();
    QMap<QString, UAVObject::Metadata> originalMetaData;
    void restoreMdata();
    rc_config_steps currentRcStep;
    void setRcStep(rc_config_steps);
    UAVObjectManager *objManager;
    ReceiverActivity *receiverActivityObj;
    ReceiverActivity::DataFields receiverActivityData;
    channelsStruct currentChannel;
    bool channelDetected;
    QTimer deadBand;
    float deadBandValue;
    float min_max_deadband[4][2];
signals:
    void rebootMessage(QString);
    void channelDetectedSignal(int);
};

#endif // CONFIGGADGETFACTORY_H
