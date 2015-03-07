/**
 ******************************************************************************
 *
 * @file       configgadgetfactory.cpp
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
#include "configgadgetfactory.h"

static const quint16 slowUpdate = 5000; // in [ms]
static const quint16 fastUpdate =  150; // in [ms]
static const int DEBOUNCE_THRESHOLD_COUNT = 1;
// The fraction of range to place the neutral position in
#define THROTTLE_NEUTRAL_FRACTION 0.02

ConfigGadgetFactory::ConfigGadgetFactory(QObject *parent) :
    QObject(parent), currentStatus(STATUS_IDLE), inputConfiguration(new InputConfigurator(this))
{
    pm=ExtensionSystem::PluginManager::instance();
    Q_ASSERT(pm);

//    objManager = pm->getObject<UAVObjectManager>();

//    manualCommandObj = ManualControlCommand::GetInstance(objManager);
//    manualSettingsObj = ManualControlSettings::GetInstance(objManager);
//    flightStatusObj = FlightStatus::GetInstance(objManager);
//    receiverActivityObj=ReceiverActivity::GetInstance(objManager);
    channelOrder <<     ManualControlSettings::CHANNELGROUPS_THROTTLE <<
                        ManualControlSettings::CHANNELGROUPS_ROLL <<
                        ManualControlSettings::CHANNELGROUPS_PITCH <<
                        ManualControlSettings::CHANNELGROUPS_YAW <<
                        ManualControlSettings::CHANNELGROUPS_FLIGHTMODE <<
                        ManualControlSettings::CHANNELGROUPS_ACCESSORY0 <<
                        ManualControlSettings::CHANNELGROUPS_ACCESSORY1 <<
                        ManualControlSettings::CHANNELGROUPS_ACCESSORY2 <<
                        ManualControlSettings::CHANNELGROUPS_ARMING;


    // Get telemetry manager and make sure it is valid
    telMngr = pm->getObject<TelemetryManager>();
    conMngr = Core::ICore::instance()->connectionManager();
    objManager = pm->getObject<UAVObjectManager>();
    utilMngr = pm->getObject<UAVObjectUtilManager>();
    m_manualControlSettings = ManualControlSettings::GetInstance(objManager);
    m_manualControlCommand = ManualControlCommand::GetInstance(objManager);
    receiverActivityObj = ReceiverActivity::GetInstance(objManager);
    firmwareIap = FirmwareIAPObj::GetInstance(objManager);
    Q_ASSERT(telMngr);
    connect(telMngr, SIGNAL(connected()), this, SLOT(boardConnected()));
    connect(telMngr, SIGNAL(disconnected()), this, SLOT(boardDisconnected()));
    mainwindow = (Core::Internal::MainWindow*)Core::ICore::instance()->mainWindow();
    mainWidget = mainwindow->mainWidget();
    dialog = mainwindow->tlDialog();
    intToChannel.insert(ManualControlSettings::CHANNELMAX_ROLL, MainWidget::CHANNEL_ROLL);
    intToChannel.insert(ManualControlSettings::CHANNELMAX_PITCH, MainWidget::CHANNEL_PITCH);
    intToChannel.insert(ManualControlSettings::CHANNELMAX_YAW, MainWidget::CHANNEL_YAW);
    intToChannel.insert(ManualControlSettings::CHANNELMAX_THROTTLE, MainWidget::CHANNEL_THROTTLE);

    setupInputTypes();
    connect(mainWidget, SIGNAL(receiverTypeChanged(int)), this, SLOT(onReceiverTypeChanged(int)));
    connect(mainWidget, SIGNAL(flightModeChanged(int,int)), this, SLOT(onSelectedFlightModeChangedUI(int, int)));
    connect(mainWidget, SIGNAL(configStartRequested()), this, SLOT(onConfigStartRequested()));
    connect(mainWidget, SIGNAL(configCancelRequested()), this, SLOT(onConfigCancelRequested()));
    connect(mainWidget, SIGNAL(configNextRequested()), this, SLOT(onConfigNextRequested()));
    connect(dialog, SIGNAL(okClicked(QObject*)), this, SLOT(onDialogOkClicked(QObject*)));
    connect(dialog, SIGNAL(cancelClicked(QObject*)), this, SLOT(onDialogCancelClicked(QObject*)));
    connect(&timeout, SIGNAL(timeout()), this, SLOT(onTimeout()));
    connect(m_manualControlSettings, SIGNAL(objectUpdated(UAVObject*)), this, SLOT(onManualControlSettingsChanged()));
    connect(m_manualControlCommand, SIGNAL(objectUpdated(UAVObject*)), this, SLOT(onManualControlCommandChanged()));
    connect(this, SIGNAL(channelDetectedSignal(int)), this, SLOT(onChannelDetected(int)));
    onManualControlSettingsChanged();

    setRcStep(RC_WELCOME);
    mainWidget->setStepText("");
    mainWidget->setDone(MainWidget::CHANNEL_ALL, false);
    deadBand.setSingleShot(true);
    deadBand.setInterval(5000);
    connect(&deadBand, SIGNAL(timeout()), this, SLOT(onDeadBandTimerTimeout()));
}

ConfigGadgetFactory::~ConfigGadgetFactory()
{
}

void ConfigGadgetFactory::boardConnected()
{
    mainWidget->setConnected(true);
    if(currentStatus == STATUS_REBOOTING) {
        dialog->close();
        currentStatus = STATUS_IDLE;
        timeout.stop();
    }
    Core::IBoardType *board = utilMngr->getBoardType();
    qDebug()<<"boardConnected board pointer:"<<board;
    if(board) {
        hwObject = utilMngr->getObjectManager()->getObject(board->getHwUAVO());
        connect(hwObject, SIGNAL(objectUpdated(UAVObject*)), this, SLOT(onHardwareObjectChanged()), Qt::UniqueConnection);
    }
    onHardwareObjectChanged();
    mainWidget->setStepText("Board connected");
    mainWidget->enableAll(true);
    UAVObject::Metadata mdata = m_manualControlCommand->getMetadata();
    UAVObject::SetFlightTelemetryUpdateMode(mdata, UAVObject::UPDATEMODE_PERIODIC);
    mdata.flightTelemetryUpdatePeriod = fastUpdate;
    m_manualControlCommand->setMetadata(mdata);
    conMngr->resumePolling();
}

void ConfigGadgetFactory::boardDisconnected()
{
    mainWidget->setConnected(false);
    mainWidget->setStepText("Board not connected");
    mainWidget->enableAll(false);

}

void ConfigGadgetFactory::onReceiverTypeChanged(int value)
{
    qDebug()<<"onReceiverTypeChanged";
    Core::IBoardType *board = utilMngr->getBoardType();
    if(!board)
        return;
    hwObject = utilMngr->getObjectManager()->getObject(board->getHwUAVO());
    qDebug()<<"HWUAVObject = "<< hwObject->getName();
    disconnect(hwObject, SIGNAL(objectUpdated(UAVObject*)), this, SLOT(onHardwareObjectChanged()));
    IBoardType::InputType type = (IBoardType::InputType) value;
    qDebug()<<"type "<<type;
    board->setInputOnPort(type);
    delay(500);
    hwObject->updated();
    utilMngr->saveObjectToFlash(hwObject);
    board->getInputOnPort(0);
    delay(500);
    dialog->setup(TLSplashDialog::TYPE_QUESTION, TLSplashDialog::BUTTONS_OK | TLSplashDialog::BUTTONS_CANCEL, "Reboot board to apply new settings?", false, TLSplashDialog::BUTTONS_OK, 5, this);
    dialog->show();
    currentStatus = STATUS_DIALOG_REBOOT;
}

void ConfigGadgetFactory::onDialogOkClicked(QObject *sender)
{
    if(sender != this)
        return;
    dialog->close();
    if(currentStatus == STATUS_DIALOG_REBOOT) {
        currentStatus = STATUS_REBOOTING;
        timeout.start(20000);
        onRebootMessage("Starting board reboot");
        boardReboot();
    }
    else if(currentStatus == STATUS_RC_CONFIG) {
        currentRcStep = RC_DEADBAND_CALCULATE;
        qDebug()<<"OK PRESSED ON DEADBAND";
        setRcStep(currentRcStep);
    }
}

void ConfigGadgetFactory::onDialogCancelClicked(QObject *sender)
{
    if(sender != this)
        return;
    dialog->close();
    if(currentStatus == STATUS_DIALOG_REBOOT) {
        currentStatus = STATUS_IDLE;
    }
    else if(currentStatus == STATUS_RC_CONFIG) {
        currentRcStep = RC_FINISH;
        setRcStep(currentRcStep);
    }
}

void ConfigGadgetFactory::onTimeout()
{
    if(currentStatus == STATUS_REBOOTING) {
        currentStatus = STATUS_IDLE;
        dialog->setup(TLSplashDialog::TYPE_ERROR, TLSplashDialog::BUTTONS_OK, "Board failed to reboot, try to remove power and USB and reinsert USB", false, TLSplashDialog::BUTTONS_OK, 5, this);
    }
}

void ConfigGadgetFactory::onRebootMessage(QString text)
{
    dialog->setup(TLSplashDialog::TYPE_WAIT, TLSplashDialog::BUTTONS_NO_BUTTONS, text, true, TLSplashDialog::BUTTONS_NO_BUTTONS, 0, this);
    dialog->show();
}

void ConfigGadgetFactory::onManualControlSettingsChanged()
{
    int i;
    i = m_manualControlSettings->getChannelNumber(ManualControlSettings::CHANNELGROUPS_ROLL);
    mainWidget->setChannelNumberRoll(i);
    i = m_manualControlSettings->getChannelNumber(ManualControlSettings::CHANNELGROUPS_PITCH);
    mainWidget->setChannelNumberPitch(i);
    i = m_manualControlSettings->getChannelNumber(ManualControlSettings::CHANNELGROUPS_YAW);
    mainWidget->setChannelNumberYaw(i);
    i = m_manualControlSettings->getChannelNumber(ManualControlSettings::CHANNELGROUPS_THROTTLE);
    mainWidget->setChannelNumberThrottle(i);
    i = m_manualControlSettings->getChannelNumber(ManualControlSettings::CHANNELGROUPS_FLIGHTMODE);
    mainWidget->setChannelNumberFlightMode(i);
    for(int i = 0; i < 5; ++i)
    {
        mainWidget->setCurrentFlightMode(i, m_manualControlSettings->getFlightModePosition(i));
    }
    for(quint8 i = 0; i < (ManualControlSettings::CHANNELMAX_NUMELEM); ++i)
    {
        if(m_manualControlSettings->getChannelMax(i) < m_manualControlSettings->getChannelMin(i))
            mainWidget->setChannelInverted(intToChannel.value(i, MainWidget::CHANNEL_NONE), true);
        else
            mainWidget->setChannelInverted(intToChannel.value(i, MainWidget::CHANNEL_NONE), false);
    }
    mainWidget->setNumberOfFlightModes(m_manualControlSettings->getFlightModeNumber());
}

void ConfigGadgetFactory::onManualControlCommandChanged()
{
    float currentChannelValue[4];
    if(currentRcStep == RC_DEADBAND_CALCULATE)
    {
        if(deadBandValue == 0)
        {
            min_max_deadband[0][0] = m_manualControlCommand->getRoll();
            min_max_deadband[0][1] = m_manualControlCommand->getRoll();
            min_max_deadband[1][0] = m_manualControlCommand->getPitch();
            min_max_deadband[1][1] = m_manualControlCommand->getPitch();
            min_max_deadband[2][0] = m_manualControlCommand->getYaw();
            min_max_deadband[2][1] = m_manualControlCommand->getYaw();
            min_max_deadband[3][0] = m_manualControlCommand->getThrottle();
            min_max_deadband[3][1] = m_manualControlCommand->getThrottle();
        }

        {
            currentChannelValue[0] = m_manualControlCommand->getRoll();
            currentChannelValue[1] = m_manualControlCommand->getPitch();
            currentChannelValue[2] = m_manualControlCommand->getYaw();
            currentChannelValue[3] = m_manualControlCommand->getThrottle();
            for(int i = 0; i < 4; ++i)
            {
                if(currentChannelValue[i] > min_max_deadband[i][DEAD_MAX])
                    min_max_deadband[i][DEAD_MAX] = currentChannelValue[i];
                if(currentChannelValue[i] < min_max_deadband[i][DEAD_MIN])
                    min_max_deadband[i][DEAD_MIN] = currentChannelValue[i];
            }
            float current = 0;
            for(int i = 0; i < 4; ++i)
            {
                current = ((min_max_deadband[i][DEAD_MAX] - min_max_deadband[i][DEAD_MIN]) / 2);
                if(current > deadBandValue)
                    deadBandValue = current;
            }
        }
    }
    mainWidget->blockSignals(true);
    quint16 rawValue = m_manualControlCommand->getChannel_FlightMode();
    qint16 max = m_manualControlSettings->getChannelMax_FlightMode();
    qint16 min = m_manualControlSettings->getChannelMin_FlightMode();
    qint16 neutral = m_manualControlSettings->getChannelNeutral_FlightMode();
    float flight_mode_value = scaleChannel(rawValue, max, min, neutral);
    quint8 flight_mode_number = m_manualControlSettings->getFlightModeNumber();
    quint8 pos = ((qint16)(flight_mode_value * 256) + 256) * flight_mode_number >> 9;
    if (pos >= flight_mode_number)
        pos = flight_mode_number - 1;
    if(pos < 5)
        mainWidget->setActiveFlighMode(pos);
    mainWidget->blockSignals(false);
    mainWidget->setChannelValue(MainWidget::CHANNEL_ROLL, (m_manualControlCommand->getRoll() * 100.0f + 100.0f) / 2);
    mainWidget->setChannelValue(MainWidget::CHANNEL_PITCH, (m_manualControlCommand->getPitch() * 100.0f + 100.0f) / 2);
    mainWidget->setChannelValue(MainWidget::CHANNEL_YAW, (m_manualControlCommand->getYaw() * 100.0f + 100.0f) / 2);
    mainWidget->setChannelValue(MainWidget::CHANNEL_THROTTLE, (m_manualControlCommand->getThrottle() * 100.0f + 100.0f) / 2);
    mainWidget->setChannelValue(MainWidget::CHANNEL_FLIGHT_MODE, (flight_mode_value * 100.0f + 100.0f) / 2);


    if(currentStatus == STATUS_RC_CONFIG)
    {
        switch (currentRcStep) {
        case RC_DETECT_ROLL_MAXVALUE:
            m_manualControlSettings->setChannelMax_Roll(m_manualControlCommand->getChannel_Roll());
            m_manualControlSettings->updated();
        case RC_DETECT_ROLL_MINVALUE:
            m_manualControlSettings->setChannelMin_Roll(m_manualControlCommand->getChannel_Roll());
            m_manualControlSettings->updated();
        case RC_DETECT_ROLL_NEUTRALVALUE:
            m_manualControlSettings->setChannelNeutral_Roll(m_manualControlCommand->getChannel_Roll());
            m_manualControlSettings->updated();
            break;
        case RC_DETECT_PITCH_MAXVALUE:
            m_manualControlSettings->setChannelMax_Pitch(m_manualControlCommand->getChannel_Pitch());
            m_manualControlSettings->updated();
        case RC_DETECT_PITCH_MINVALUE:
            m_manualControlSettings->setChannelMin_Pitch(m_manualControlCommand->getChannel_Pitch());
            m_manualControlSettings->updated();
        case RC_DETECT_PITCH_NEUTRALVALUE:
            m_manualControlSettings->setChannelNeutral_Pitch(m_manualControlCommand->getChannel_Pitch());
            m_manualControlSettings->updated();
            break;
        case RC_DETECT_YAW_MAXVALUE:
            m_manualControlSettings->setChannelMax_Yaw(m_manualControlCommand->getChannel_Yaw());
            m_manualControlSettings->updated();
        case RC_DETECT_YAW_MINVALUE:
            m_manualControlSettings->setChannelMin_Yaw(m_manualControlCommand->getChannel_Yaw());
            m_manualControlSettings->updated();
        case RC_DETECT_YAW_NEUTRALVALUE:
            m_manualControlSettings->setChannelNeutral_Yaw(m_manualControlCommand->getChannel_Yaw());
            m_manualControlSettings->updated();
            break;
        case RC_DETECT_THROTTLE_MAXVALUE:
            m_manualControlSettings->setChannelMax_Throttle(m_manualControlCommand->getChannel_Throttle());
            m_manualControlSettings->updated();
        case RC_DETECT_THROTTLE_MINVALUE:
            m_manualControlSettings->setChannelMin_Throttle(m_manualControlCommand->getChannel_Throttle());
            m_manualControlSettings->updated();
        case RC_DETECT_THROTTLE_NEUTRALVALUE://FIXME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            m_manualControlSettings->setChannelNeutral_Throttle(m_manualControlCommand->getChannel_Throttle());
            m_manualControlSettings->updated();
            break;
        case RC_DETECT_FLIGHTMODE_MAXVALUE:
            m_manualControlSettings->setChannelMax_FlightMode(m_manualControlCommand->getChannel_FlightMode());
            m_manualControlSettings->updated();
        case RC_DETECT_FLIGHTMODE_MINVALUE:
            m_manualControlSettings->setChannelMin_FlightMode(m_manualControlCommand->getChannel_FlightMode());
            m_manualControlSettings->updated();
        case RC_DETECT_FLIGHTMODE_NEUTRALVALUE:
            m_manualControlSettings->setChannelNeutral_FlightMode(m_manualControlCommand->getChannel_FlightMode());
            m_manualControlSettings->updated();
            break;
        default:
            break;
        }
    }
}

void ConfigGadgetFactory::onHardwareObjectChanged()
{
    qDebug()<<"onHardwareObjectChanged()";
    Core::IBoardType *board = utilMngr->getBoardType();
    qDebug()<<"onHardwareObjectChanged board pointer:"<<board;
    if(!board)
        return;
    mainWidget->blockSignals(true);
    qDebug()<<"input type "<< board->getInputOnPort();
    mainWidget->setCurrentReceiverType(board->getInputOnPort());
    mainWidget->blockSignals(false);
}

void ConfigGadgetFactory::onSelectedFlightModeChangedUI(int flighModeNumber, int flighModeSelected)
{
    m_manualControlSettings->setFlightModePosition(flighModeNumber, flighModeSelected);
    m_manualControlSettings->updated();
    utilMngr->saveObjectToFlash(m_manualControlSettings);
}

void ConfigGadgetFactory::onConfigStartRequested()
{
    if(!telMngr->isConnected())
        mainWidget->onConfigCancel();
    currentStatus = STATUS_RC_CONFIG;
    currentRcStep = RC_WELCOME;
    fastMdata();
    setRcStep(currentRcStep);
}

void ConfigGadgetFactory::onConfigCancelRequested()
{
    currentStatus = STATUS_IDLE;
    currentRcStep = RC_ABORTED;
    setRcStep(currentRcStep);
    restoreMdata();
    mainWidget->setActive(MainWidget::CHANNEL_ALL, false);
}

void ConfigGadgetFactory::onConfigNextRequested()
{
    if(currentRcStep == RC_DEADBAND_CALCULATE)
    {
        deadBand.start(5000);
        mainWidget->disableNext(true);
        deadBandValue = 0;//TODO ADD MUTEX
        dialog->setup(TLSplashDialog::TYPE_WAIT, TLSplashDialog::BUTTONS_NO_BUTTONS, "Please wait while deadband is being calculated", true, TLSplashDialog::BUTTONS_NO_BUTTONS, 0, this);
        dialog->show();
        return;
    }
    int i = (int)currentRcStep + 1;
    currentRcStep = (rc_config_steps)i;
    setRcStep(currentRcStep);
}

void ConfigGadgetFactory::onReceiverActivityUpdated()
{
    static int debounce=0;
    ManualControlSettings::DataFields manualSettingsData;
    receiverActivityData = receiverActivityObj->getData();
    int currentChannelNum;
    if(receiverActivityData.ActiveChannel == 255) {
        switch (currentRcStep) {
        case RC_DETECT_FLIGHTMODE_CHANNEL:
            mainWidget->setChannelNumberFlightMode(-1);
            break;
        case RC_DETECT_PITCH_CHANNEL:
            mainWidget->setChannelNumberPitch(-1);
            break;
        case RC_DETECT_ROLL_CHANNEL:
            mainWidget->setChannelNumberRoll(-1);
            break;
        case RC_DETECT_THROTTLE_CHANNEL:
            mainWidget->setChannelNumberThrottle(-1);
            break;
        case RC_DETECT_YAW_CHANNEL:
            mainWidget->setChannelNumberYaw(-1);
            break;
        default:
            break;
        }
        return;
    }
    currentChannel.group = receiverActivityData.ActiveGroup;
    currentChannel.number = receiverActivityData.ActiveChannel;
    if(currentChannel == lastChannel)
        ++debounce;
    lastChannel.group = currentChannel.group;
    lastChannel.number =currentChannel.number;

    // If this channel isn't already allocated, and the debounce passes the threshold, set
    // the input channel as detected.
    if(debounce > DEBOUNCE_THRESHOLD_COUNT)
    {
        debounce = 0;
        manualSettingsData = m_manualControlSettings->getData();
        switch (currentRcStep) {
        case RC_DETECT_FLIGHTMODE_CHANNEL:
            currentChannelNum = ManualControlSettings::CHANNELMAX_FLIGHTMODE;
            break;
        case RC_DETECT_PITCH_CHANNEL:
            currentChannelNum = ManualControlSettings::CHANNELMAX_PITCH;
            break;
        case RC_DETECT_ROLL_CHANNEL:
            currentChannelNum = ManualControlSettings::CHANNELMAX_ROLL;
            break;
        case RC_DETECT_THROTTLE_CHANNEL:
            currentChannelNum = ManualControlSettings::CHANNELMAX_THROTTLE;
            break;
        case RC_DETECT_YAW_CHANNEL:
            currentChannelNum = ManualControlSettings::CHANNELMAX_YAW;
            break;
        default:
            break;
        }
        manualSettingsData.ChannelGroups[currentChannelNum] = currentChannel.group;
        manualSettingsData.ChannelNumber[currentChannelNum] = currentChannel.number;
        m_manualControlSettings->setData(manualSettingsData);
    }
    else
        return;
    emit channelDetectedSignal(currentChannel.number);
}

void ConfigGadgetFactory::onChannelDetected(int i)
{
    mainWidget->setStepText(QString("Channel detected on channel %0. Press next to continue.").arg(i));
}

void ConfigGadgetFactory::onDeadBandTimerTimeout()
{
    dialog->close();
    mainWidget->disableNext(false);
    qDebug()<<"FINAL DEADBAND"<<deadBandValue;
    m_manualControlSettings->setDeadband(deadBandValue);
    m_manualControlSettings->updated();
    utilMngr->saveObjectToFlash(m_manualControlSettings);
    currentRcStep = RC_FINISH;
    setRcStep(currentRcStep);
}

float ConfigGadgetFactory::scaleChannel(int value, int max, int min, int neutral)
{
    float valueScaled;

    // Scale
    if ((max > min && value >= neutral) || (min > max && value <= neutral))
    {
        if (max != neutral)
            valueScaled = (float)(value - neutral) / (float)(max - neutral);
        else
            valueScaled = 0;
    }
    else
    {
        if (min != neutral)
            valueScaled = (float)(value - neutral) / (float)(neutral - min);
        else
            valueScaled = 0;
    }

    // Bound
    if (valueScaled >  1.0f) valueScaled =  1.0;
    else
    if (valueScaled < -1.0f) valueScaled = -1.0;

    return valueScaled;
}

void ConfigGadgetFactory::setupInputTypes()
{
    QStringList names;
    names << "PWM" << "PPM" << "SBUS" << "DSM";
    QList<QVariant> types;
    types << IBoardType::INPUT_TYPE_PWM << IBoardType::INPUT_TYPE_PPM << IBoardType::INPUT_TYPE_SBUS << IBoardType::INPUT_TYPE_DSM;
    mainWidget->setReceiverTypes(names, types);
    UAVObjectField *field = m_manualControlSettings->getField("FlightModePosition");
    QStringList options = field->getOptions();
    mainWidget->setFlightModes(options);
}

bool ConfigGadgetFactory::boardReboot()
{
    if(!telMngr->isConnected() || !firmwareIap->getIsPresentOnHardware())
        return false;
    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    firmwareIap->setBoardRevision(0);
    firmwareIap->setBoardType(0);
    connect(&timeout, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(firmwareIap,SIGNAL(transactionCompleted(UAVObject*,bool)), &loop, SLOT(quit()));
    int magicValue = 1122;
    int magicStep = 1111;
    for(int i = 0; i < 3; ++i)
    {
        //Firmware IAP module specifies that the timing between iap commands must be
        //between 500 and 5000ms
        timeout.start(600);
        loop.exec();
        firmwareIap->setCommand(magicValue);
        magicValue += magicStep;
        if(magicValue == 3344)
            magicValue = 4455;
        emit rebootMessage(QString(tr("Sending IAP Step %0").arg(i + 1)));
        firmwareIap->updated();
        timeout.start(1000);
        loop.exec();
        if(!timeout.isActive())
        {
            emit rebootMessage(QString(tr("Sending IAP Step %0 failed").arg(i + 1)));
            return false;
        }
        timeout.stop();
    }
    conMngr->disconnectDevice();
    timeout.start(200);
    loop.exec();
    conMngr->suspendPolling();
    boardDisconnected();
    return true;
}

void ConfigGadgetFactory::delay(int ms)
{
    QEventLoop loop;
    QTimer time;
    connect(&time, SIGNAL(timeout()), &loop, SLOT(quit()));
    time.start(ms);
    loop.exec();
}

void ConfigGadgetFactory::fastMdata()
{
    // Check that the metadata hasn't already been saved.
    if (!originalMetaData.empty())
        return;

    UAVObjectManager *objManager = pm->getObject<UAVObjectManager>();

    // Store original metadata
    UAVObjectUtilManager* utilMngr = pm->getObject<UAVObjectUtilManager>();
    originalMetaData = utilMngr->readAllNonSettingsMetadata();

    // Iterate over list of UAVObjects, configuring all dynamic data metadata objects.
    QVector< QVector<UAVDataObject*> > objList = objManager->getDataObjectsVector();
    foreach (QVector<UAVDataObject*> list, objList) {
        foreach (UAVDataObject* obj, list) {
            if(!obj->isSettings()) {
                UAVObject::Metadata mdata = obj->getMetadata();
                UAVObject::SetFlightAccess(mdata, UAVObject::ACCESS_READWRITE);

                switch(obj->getObjID()) {
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

void ConfigGadgetFactory::restoreMdata()
{
    UAVObjectManager *objManager = pm->getObject<UAVObjectManager>();
    foreach (QString objName, originalMetaData.keys()) {
        UAVObject *obj = objManager->getObject(objName);
        obj->setMetadata(originalMetaData.value(objName));
    }
    originalMetaData.clear();
}

void ConfigGadgetFactory::setRcStep(ConfigGadgetFactory::rc_config_steps step)
{
    switch (step) {
    case RC_DETECT_ROLL_CHANNEL:
    case RC_DETECT_FLIGHTMODE_CHANNEL:
    case RC_DETECT_YAW_CHANNEL:
    case RC_DETECT_THROTTLE_CHANNEL:
    case RC_DETECT_PITCH_CHANNEL:
        connect(receiverActivityObj, SIGNAL(objectUpdated(UAVObject*)), this, SLOT(onReceiverActivityUpdated()), Qt::UniqueConnection);
        break;
    default:
        disconnect(receiverActivityObj, SIGNAL(objectUpdated(UAVObject*)), this, SLOT(onReceiverActivityUpdated()));
        break;
    }
    switch (step) {
    case RC_DETECT_ROLL_CHANNEL:
        break;
    case RC_DETECT_PITCH_CHANNEL:
        mainWidget->setDone(MainWidget::CHANNEL_ROLL, true);
        break;
    case RC_DETECT_THROTTLE_CHANNEL:
        mainWidget->setDone(MainWidget::CHANNEL_PITCH, true);
        break;
    case RC_DETECT_YAW_CHANNEL:
        mainWidget->setDone(MainWidget::CHANNEL_THROTTLE, true);
        break;
    case RC_DETECT_FLIGHTMODE_CHANNEL:
        mainWidget->setDone(MainWidget::CHANNEL_YAW, true);
        break;
    default:
        disconnect(receiverActivityObj, SIGNAL(objectUpdated(UAVObject*)), this, SLOT(onReceiverActivityUpdated()));
        break;
    }
    mainWidget->setActive(MainWidget::CHANNEL_ALL, false);
    switch (step) {
    case RC_DETECT_ROLL_CHANNEL:
    case RC_DETECT_ROLL_MAXVALUE:
    case RC_DETECT_ROLL_MINVALUE:
    case RC_DETECT_ROLL_NEUTRALVALUE:
        mainWidget->setActive(MainWidget::CHANNEL_ROLL, true);
        break;
    case RC_DETECT_FLIGHTMODE_CHANNEL:
    case RC_DETECT_FLIGHTMODE_MAXVALUE:
    case RC_DETECT_FLIGHTMODE_MINVALUE:
    case RC_DETECT_FLIGHTMODE_NEUTRALVALUE:
        mainWidget->setActive(MainWidget::CHANNEL_FLIGHT_MODE, true);
        break;
    case RC_DETECT_YAW_CHANNEL:
    case RC_DETECT_YAW_MAXVALUE:
    case RC_DETECT_YAW_MINVALUE:
    case RC_DETECT_YAW_NEUTRALVALUE:
        mainWidget->setActive(MainWidget::CHANNEL_YAW, true);
        break;
    case RC_DETECT_THROTTLE_CHANNEL:
    case RC_DETECT_THROTTLE_MAXVALUE:
    case RC_DETECT_THROTTLE_MINVALUE:
    case RC_DETECT_THROTTLE_NEUTRALVALUE:
        mainWidget->setActive(MainWidget::CHANNEL_THROTTLE, true);
        break;
    case RC_DETECT_PITCH_CHANNEL:
    case RC_DETECT_PITCH_MAXVALUE:
    case RC_DETECT_PITCH_MINVALUE:
    case RC_DETECT_PITCH_NEUTRALVALUE:
        mainWidget->setActive(MainWidget::CHANNEL_PITCH, true);
        break;
    default:
        mainWidget->setActive(MainWidget::CHANNEL_ALL, false);
        break;
    }
    QString specificText;
    QString generalText;
    switch (step) {
    case RC_DETECT_ROLL_CHANNEL:
    case RC_DETECT_ROLL_MAXVALUE:
    case RC_DETECT_ROLL_MINVALUE:
    case RC_DETECT_ROLL_NEUTRALVALUE:
        specificText = tr("roll");
        break;
    case RC_DETECT_PITCH_CHANNEL:
    case RC_DETECT_PITCH_MAXVALUE:
    case RC_DETECT_PITCH_MINVALUE:
    case RC_DETECT_PITCH_NEUTRALVALUE:
        specificText = tr("pitch");
        break;
    case RC_DETECT_THROTTLE_CHANNEL:
    case RC_DETECT_THROTTLE_MAXVALUE:
    case RC_DETECT_THROTTLE_MINVALUE:
    case RC_DETECT_THROTTLE_NEUTRALVALUE:
        specificText = tr("throttle");
        break;
    case RC_DETECT_YAW_CHANNEL:
    case RC_DETECT_YAW_MAXVALUE:
    case RC_DETECT_YAW_MINVALUE:
    case RC_DETECT_YAW_NEUTRALVALUE:
        specificText = tr("yaw");
        break;
    default:
        break;
    }
    switch (step) {
    case RC_WELCOME:
        generalText = QString("This wizard will guide you through the RC configuration.\n"
                              "Please move each stick (and only that one) as requested.\n"
                              "Click 'next' when ready");
        break;
    case RC_DETECT_ROLL_CHANNEL:
    case RC_DETECT_PITCH_CHANNEL:
    case RC_DETECT_YAW_CHANNEL:
    case RC_DETECT_THROTTLE_CHANNEL:
        generalText = QString("Please move the %0 stick several times").arg(specificText);
        break;
    case RC_DETECT_FLIGHTMODE_CHANNEL:
        generalText = QString("Please move the flight mode switch several times");
        break;
    case RC_DETECT_ROLL_MAXVALUE:
    case RC_DETECT_YAW_MAXVALUE:
        generalText = QString("Please move the %0 stick all the way to the right and then press next").arg(specificText);
        break;
    case RC_DETECT_PITCH_MAXVALUE:
    case RC_DETECT_THROTTLE_MAXVALUE:
        generalText = QString("Please move the %0 stick all the way up and then press next").arg(specificText);
        break;
    case RC_DETECT_THROTTLE_MINVALUE:
    case RC_DETECT_PITCH_MINVALUE:
        generalText = QString("Please move the %0 stick all the way down and then press next").arg(specificText);
        break;
    case RC_DETECT_ROLL_MINVALUE:
    case RC_DETECT_YAW_MINVALUE:
        generalText = QString("Please move the %0 stick all the way to the left and then press next").arg(specificText);
        break;
    case RC_DETECT_ROLL_NEUTRALVALUE:
    case RC_DETECT_PITCH_NEUTRALVALUE:
    case RC_DETECT_THROTTLE_NEUTRALVALUE:
    case RC_DETECT_YAW_NEUTRALVALUE:
        generalText = QString("Please center the %0 stick").arg(specificText);
        break;
    case RC_DETECT_FLIGHTMODE_MAXVALUE:
        generalText = QString("Please move the flight mode switch all the way to one side and then press next").arg(specificText);
        break;
    case RC_DETECT_FLIGHTMODE_MINVALUE:
        generalText = QString("Please move the flight mode switch all the way to the other side and then press next").arg(specificText);
        break;
    case RC_DETECT_FLIGHTMODE_NEUTRALVALUE:
        generalText = QString("Please center the flight mode switch and then press next").arg(specificText);
        break;
    case RC_ABORTED:
        generalText = QString("Input calibration was aborted!");
        currentStatus = STATUS_IDLE;
        currentRcStep = RC_WELCOME;
        restoreMdata();
        break;
    case RC_DEADBAND_QUESTION:
        generalText = QString("Input Calibration ended!");
        mainWidget->setStepText(generalText);
        dialog->setup(TLSplashDialog::TYPE_QUESTION, TLSplashDialog::BUTTONS_OK | TLSplashDialog::BUTTONS_CANCEL, "The input configuration ended, do you want to proceed with deadband detection?", false, TLSplashDialog::BUTTONS_OK, 5, this);
        dialog->show();
        break;
    case RC_DEADBAND_CALCULATE:
        generalText = QString("Please center all sticks and switches and press next when ready");
        break;
    case RC_FINISH:
        mainWidget->onConfigEnded();
        currentStatus = STATUS_IDLE;
        currentRcStep = RC_WELCOME;
        restoreMdata();
        mainWidget->setDone(MainWidget::CHANNEL_ALL, false);
        dialog->setup(TLSplashDialog::TYPE_WAIT, TLSplashDialog::BUTTONS_OK, "RC Configuration done!", false, TLSplashDialog::BUTTONS_OK, 5, this);
        dialog->show();
        break;
    default:
        break;
    }
    mainWidget->setStepText(generalText);
}
