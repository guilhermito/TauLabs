/**
 ******************************************************************************
 *
 * @file       configinputwidget.cpp
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2013
 * @addtogroup GCSPlugins GCS Plugins
 * @{
 * @addtogroup ConfigPlugin Config Plugin
 * @{
 * @brief Servo input/output configuration panel for the config gadget
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

#include "configinputwidget.h"

#include "uavtalk/telemetrymanager.h"

#include <QDebug>
#include <QStringList>
#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <utils/stylehelper.h>
#include <QMessageBox>

#include <extensionsystem/pluginmanager.h>
#include <generalsettings.h>

#include "actuatorcommand.h"

// The fraction of range to place the neutral position in
#define THROTTLE_NEUTRAL_FRACTION 0.02

#define ACCESS_MIN_MOVE -3
#define ACCESS_MAX_MOVE 3
#define STICK_MIN_MOVE -8
#define STICK_MAX_MOVE 8

InputConfigurator::InputConfigurator(QObject *parent) :QObject(parent)
{
    pm=ExtensionSystem::PluginManager::instance();
    Q_ASSERT(pm);

    objManager = pm->getObject<UAVObjectManager>();

    manualCommandObj = ManualControlCommand::GetInstance(objManager);
    manualSettingsObj = ManualControlSettings::GetInstance(objManager);
    flightStatusObj = FlightStatus::GetInstance(objManager);
    receiverActivityObj=ReceiverActivity::GetInstance(objManager);
    


    // Get telemetry manager and make sure it is valid
    telMngr = pm->getObject<TelemetryManager>();
    Q_ASSERT(telMngr);
}

InputConfigurator::~InputConfigurator()
{

}

void InputConfigurator::goToWizard()
{
    // Monitor for connection loss to reset wizard safely
    connect(telMngr, SIGNAL(disconnected()), this, SLOT(wzCancel()));
}

/**
 * @brief ConfigInputWidget::fastMdata Set manual control command to fast updates. Set all others to updates slowly.
 */
void InputConfigurator::fastMdata()
{
    // Check that the metadata hasn't already been saved.
    if (!originalMetaData.empty())
        return;

    // Store original metadata
    UAVObjectUtilManager* utilMngr = pm->getObject<UAVObjectUtilManager>();
    originalMetaData = utilMngr->readAllNonSettingsMetadata();

    // Update data rates
    quint16 slowUpdate = 5000; // in [ms]
    quint16 fastUpdate =  150; // in [ms]

    // Iterate over list of UAVObjects, configuring all dynamic data metadata objects.
    QVector< QVector<UAVDataObject*> > objList = objManager->getDataObjectsVector();
    foreach (QVector<UAVDataObject*> list, objList) {
        foreach (UAVDataObject* obj, list) {
            if(!obj->isSettings()) {
                UAVObject::Metadata mdata = obj->getMetadata();
                UAVObject::SetFlightAccess(mdata, UAVObject::ACCESS_READWRITE);

                switch(obj->getObjID()){
                    case ReceiverActivity::OBJID:
                    case FlightStatus::OBJID:
                        UAVObject::SetFlightTelemetryUpdateMode(mdata, UAVObject::UPDATEMODE_ONCHANGE);
                        break;
                    case AccessoryDesired::OBJID:
                    case ManualControlCommand::OBJID:
                        UAVObject::SetFlightTelemetryUpdateMode(mdata, UAVObject::UPDATEMODE_PERIODIC);
                        mdata.flightTelemetryUpdatePeriod = fastUpdate;
                        break;
                    case ActuatorCommand::OBJID:
                        UAVObject::SetFlightAccess(mdata, UAVObject::ACCESS_READONLY);
                        UAVObject::SetFlightTelemetryUpdateMode(mdata, UAVObject::UPDATEMODE_PERIODIC);
                        mdata.flightTelemetryUpdatePeriod = slowUpdate;
                        break;
                    default:
                        UAVObject::SetFlightTelemetryUpdateMode(mdata, UAVObject::UPDATEMODE_PERIODIC);
                        mdata.flightTelemetryUpdatePeriod = slowUpdate;
                        break;
                }

                // Set the metadata
                obj->setMetadata(mdata);
            }
        }
    }

}

/**
  * Restore previous update settings for manual control data
  */
void InputConfigurator::restoreMdata()
{
    foreach (QString objName, originalMetaData.keys()) {
        UAVObject *obj = objManager->getObject(objName);
        obj->setMetadata(originalMetaData.value(objName));
    }
    originalMetaData.clear();
}

/**
 * @brief ConfigInputWidget::identifyControls Triggers whenever a new ReceiverActivity UAVO is received.
 * This function checks if the channel has already been identified, and if not assigns it to a new
 * ManualControlSettings channel.
 */
void InputConfigurator::identifyControls()
{
    static int debounce=0;

    receiverActivityData=receiverActivityObj->getData();
    if(receiverActivityData.ActiveChannel==255)
        return;

    if(channelDetected)
        return;
    else
    {
        receiverActivityData=receiverActivityObj->getData();
        currentChannel.group=receiverActivityData.ActiveGroup;
        currentChannel.number=receiverActivityData.ActiveChannel;
        if(currentChannel==lastChannel)
            ++debounce;
        lastChannel.group= currentChannel.group;
        lastChannel.number=currentChannel.number;

        // If this channel isn't already allocated, and the debounce passes the threshold, set
        // the input channel as detected.
        if(!usedChannels.contains(lastChannel) && debounce>DEBOUNCE_THRESHOLD_COUNT)
        {
            channelDetected = true;
            debounce=0;
            usedChannels.append(lastChannel);
            manualSettingsData=manualSettingsObj->getData();
            manualSettingsData.ChannelGroups[currentChannelNum]=currentChannel.group;
            manualSettingsData.ChannelNumber[currentChannelNum]=currentChannel.number;
            manualSettingsObj->setData(manualSettingsData);
        }
        else
            return;
    }

    QTimer::singleShot(CHANNEL_IDENTIFICATION_WAIT_TIME_MS, this, SLOT(wzNext()));
}

void InputConfigurator::identifyLimits()
{
    manualCommandData=manualCommandObj->getData();
    for(quint8 i=0;i<ManualControlSettings::CHANNELMAX_NUMELEM;++i)
    {
        if(manualSettingsData.ChannelMin[i] <= manualSettingsData.ChannelMax[i]) {
            // Non inverted channel
            if(manualSettingsData.ChannelMin[i]>manualCommandData.Channel[i])
                manualSettingsData.ChannelMin[i]=manualCommandData.Channel[i];
            if(manualSettingsData.ChannelMax[i]<manualCommandData.Channel[i])
                manualSettingsData.ChannelMax[i]=manualCommandData.Channel[i];
        } else {
            // Inverted channel
            if(manualSettingsData.ChannelMax[i]>manualCommandData.Channel[i])
                manualSettingsData.ChannelMax[i]=manualCommandData.Channel[i];
            if(manualSettingsData.ChannelMin[i]<manualCommandData.Channel[i])
                manualSettingsData.ChannelMin[i]=manualCommandData.Channel[i];
        }

        if (i == ManualControlSettings::CHANNELNEUTRAL_THROTTLE) {
            // Keep the throttle neutral position near the minimum value so that
            // the stick visualization keeps working consistently (it expects this
            // ratio between + and - range.
            manualSettingsData.ChannelNeutral[i] = manualSettingsData.ChannelMin[i] +
                    (manualSettingsData.ChannelMax[i] - manualSettingsData.ChannelMin[i]) * THROTTLE_NEUTRAL_FRACTION;
        } else {
            manualSettingsData.ChannelNeutral[i] =
                    (manualSettingsData.ChannelMin[i] + manualSettingsData.ChannelMax[i]) * 0.5;
        }
    }
    manualSettingsObj->setData(manualSettingsData);
}

/**
 * @brief ConfigInputWidget::detectFailsafe
 * Detect that the FlightStatus object indicates a Failsafe condition
 */
void InputConfigurator::detectFailsafe()
{
    FlightStatus::DataFields flightStatusData = flightStatusObj->getData();
    switch (failsafeDetection)
    {
    case FS_AWAITING_CONNECTION:
        if (flightStatusData.ControlSource == FlightStatus::CONTROLSOURCE_TRANSMITTER) {
            emit(QString(tr("To verify that the failsafe mode on your receiver is safe, please turn off your transmitter now.\n")));
            failsafeDetection = FS_AWAITING_FAILSAFE;    // Now wait for it to go away
        } else {
            emit(QString(tr("Unable to detect transmitter to verify failsafe. Please ensure it is turned on.\n")));
        }
        break;
    case FS_AWAITING_FAILSAFE:
        if (flightStatusData.ControlSource == FlightStatus::CONTROLSOURCE_FAILSAFE) {
            emit(QString(tr("Failsafe mode detected. Please turn on your transmitter again.\n")));
            failsafeDetection = FS_AWAITING_RECONNECT;    // Now wait for it to go away
        }
        break;
    case FS_AWAITING_RECONNECT:
        if (flightStatusData.ControlSource == FlightStatus::CONTROLSOURCE_TRANSMITTER) {
            emit(QString(tr("Congratulations. Failsafe detection appears to be working reliably.\n")));
        }
        break;
    }
}

//void InputConfigurator::invertControls()
//{
//    manualSettingsData=manualSettingsObj->getData();
//    foreach(QWidget * wd,extraWidgets)
//    {
//        QCheckBox * cb=qobject_cast<QCheckBox *>(wd);
//        if(cb)
//        {
//            int index = manualSettingsObj->getField("ChannelNumber")->getElementNames().indexOf(cb->text());
//            if((cb->isChecked() && (manualSettingsData.ChannelMax[index]>manualSettingsData.ChannelMin[index])) ||
//                    (!cb->isChecked() && (manualSettingsData.ChannelMax[index]<manualSettingsData.ChannelMin[index])))
//            {
//                qint16 aux;
//                aux=manualSettingsData.ChannelMax[index];
//                manualSettingsData.ChannelMax[index]=manualSettingsData.ChannelMin[index];
//                manualSettingsData.ChannelMin[index]=aux;
//                if (index == ManualControlSettings::CHANNELNEUTRAL_THROTTLE) {
//                    // Keep the throttle neutral position near the minimum value so that
//                    // the stick visualization keeps working consistently (it expects this
//                    // ratio between + and - range.
//                    manualSettingsData.ChannelNeutral[index] = manualSettingsData.ChannelMin[index] +
//                            (manualSettingsData.ChannelMax[index] - manualSettingsData.ChannelMin[index]) * THROTTLE_NEUTRAL_FRACTION;
//                }
//            }
//        }
//    }
//    manualSettingsObj->setData(manualSettingsData);
//}
/**
 * @brief Converts a raw Switch Channel value into a Switch position index
 * @param channelNumber channel number to convert
 * @param switchPositions total switch positions
 * @return Switch position index converted from the raw value
 */
quint8 InputConfigurator::scaleSwitchChannel(quint8 channelNumber, quint8 switchPositions)
{
    if(channelNumber > (ManualControlSettings::CHANNELMIN_NUMELEM - 1))
            return 0;
    ManualControlSettings::DataFields manualSettingsDataPriv = manualSettingsObj->getData();
    ManualControlCommand::DataFields manualCommandDataPriv = manualCommandObj->getData();

    float valueScaled;
    int chMin = manualSettingsDataPriv.ChannelMin[channelNumber];
    int chMax = manualSettingsDataPriv.ChannelMax[channelNumber];
    int chNeutral = manualSettingsDataPriv.ChannelNeutral[channelNumber];

    int value = manualCommandDataPriv.Channel[channelNumber];
    if ((chMax > chMin && value >= chNeutral) || (chMin > chMax && value <= chNeutral))
    {
        if (chMax != chNeutral)
            valueScaled = (float)(value - chNeutral) / (float)(chMax - chNeutral);
        else
            valueScaled = 0;
    }
    else
    {
        if (chMin != chNeutral)
            valueScaled = (float)(value - chNeutral) / (float)(chNeutral - chMin);
        else
            valueScaled = 0;
    }

    if (valueScaled < -1.0)
        valueScaled = -1.0;
    else
    if (valueScaled >  1.0)
        valueScaled =  1.0;

    // Convert channel value into the switch position in the range [0..N-1]
    // This uses the same optimized computation as flight code to be consistent
    quint8 pos = ((qint16)(valueScaled * 256) + 256) * switchPositions >> 9;
    if (pos >= switchPositions)
        pos = switchPositions - 1;
    return pos;
}


void InputConfigurator::updateCalibration()
{
    manualCommandData=manualCommandObj->getData();
    for(quint8 i=0;i<ManualControlSettings::CHANNELMAX_NUMELEM;++i)
    {
        if((!reverse[i] && manualSettingsData.ChannelMin[i]>manualCommandData.Channel[i]) ||
                (reverse[i] && manualSettingsData.ChannelMin[i]<manualCommandData.Channel[i]))
            manualSettingsData.ChannelMin[i]=manualCommandData.Channel[i];
        if((!reverse[i] && manualSettingsData.ChannelMax[i]<manualCommandData.Channel[i]) ||
                (reverse[i] && manualSettingsData.ChannelMax[i]>manualCommandData.Channel[i]))
            manualSettingsData.ChannelMax[i]=manualCommandData.Channel[i];
        manualSettingsData.ChannelNeutral[i] = manualCommandData.Channel[i];
    }

    manualSettingsObj->setData(manualSettingsData);
    manualSettingsObj->updated();
}

void InputConfigurator::simpleCalibration(bool enable)
{
    if (enable) {
        fastMdata();
        connect(manualCommandObj, SIGNAL(objectUnpacked(UAVObject*)), this, SLOT(updateCalibration()));
    } else {
        manualCommandData = manualCommandObj->getData();
        manualSettingsData = manualSettingsObj->getData();

        restoreMdata();

        for (unsigned int i = 0; i < ManualControlCommand::CHANNEL_NUMELEM; i++)
            manualSettingsData.ChannelNeutral[i] = manualCommandData.Channel[i];

        // Force flight mode neutral to middle
        manualSettingsData.ChannelNeutral[ManualControlSettings::CHANNELNUMBER_FLIGHTMODE] =
                (manualSettingsData.ChannelMax[ManualControlSettings::CHANNELNUMBER_FLIGHTMODE] +
                manualSettingsData.ChannelMin[ManualControlSettings::CHANNELNUMBER_FLIGHTMODE]) / 2;

        // Force throttle to be near min
        manualSettingsData.ChannelNeutral[ManualControlSettings::CHANNELNEUTRAL_THROTTLE] =
                manualSettingsData.ChannelMin[ManualControlSettings::CHANNELMIN_THROTTLE] +
                (manualSettingsData.ChannelMax[ManualControlSettings::CHANNELMAX_THROTTLE] -
                  manualSettingsData.ChannelMin[ManualControlSettings::CHANNELMIN_THROTTLE]) * THROTTLE_NEUTRAL_FRACTION;

        manualSettingsObj->setData(manualSettingsData);

        disconnect(manualCommandObj, SIGNAL(objectUnpacked(UAVObject*)), this, SLOT(updateCalibration()));
    }
}
