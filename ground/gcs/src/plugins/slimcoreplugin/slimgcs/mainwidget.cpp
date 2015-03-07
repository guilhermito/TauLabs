/**
 ******************************************************************************
 * @file       mainwidget.cpp
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2014
 * @addtogroup [Group]
 * @{
 * @addtogroup MainWidget
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

#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QDebug>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget), configRunning(false)
{
    ui->setupUi(this);
    ui->tabWidget->setTabIcon(0, QIcon(":/images/resources/wireless.png"));
    ui->tabWidget->setTabIcon(1, QIcon(":/images/resources/airplane_grey.png"));
    ui->tabWidget->setTabIcon(2, QIcon(":/images/resources/debug_grey.png"));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(updateTabsIcon(int)));
    channelControls controls;
    controls.channelNumber = ui->channelNumberRoll;
    controls.channelFunction = ui->labelRoll;
    controls.channelInverted = ui->reverseRoll;
    controls.channelDone = ui->checkRoll;
    controls.channelPercentage = ui->percentageRoll;
    controls.channelSlider = ui->sliderRoll;
    channelUIGroups.insert(CHANNEL_ROLL, controls);
    controls.channelNumber = ui->channelNumberPitch;
    controls.channelFunction = ui->labelPitch;
    controls.channelInverted = ui->reversePitch;
    controls.channelDone = ui->checkPitch;
    controls.channelPercentage = ui->percentagePitch;
    controls.channelSlider = ui->sliderPitch;
    channelUIGroups.insert(CHANNEL_PITCH, controls);
    controls.channelNumber = ui->channelNumberThrottle;
    controls.channelFunction = ui->labelThrottle;
    controls.channelInverted = ui->reverseThrottle;
    controls.channelDone = ui->CheckThrottle;
    controls.channelPercentage = ui->percentageThrottle;
    controls.channelSlider = ui->sliderThrottle;
    channelUIGroups.insert(CHANNEL_THROTTLE, controls);
    controls.channelNumber = ui->channelNumberYaw;
    controls.channelFunction = ui->labelYaw;
    controls.channelInverted = ui->reverseYaw;
    controls.channelDone = ui->checkYaw;
    controls.channelPercentage = ui->percentageYaw;
    controls.channelSlider = ui->sliderYaw;
    channelUIGroups.insert(CHANNEL_YAW, controls);

    controls.channelNumber = ui->channelNumberFlightMode;
    controls.channelFunction = ui->labelFlightMode;
    controls.channelInverted = ui->reverseThrottle;
    controls.channelDone = NULL;
    controls.channelPercentage = NULL;
    controls.channelSlider = ui->sliderFlightMode;
    channelUIGroups.insert(CHANNEL_FLIGHT_MODE, controls);

    foreach (channelControls control, channelUIGroups) {
        if(control.channelSlider)
            connect(control.channelSlider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChanged(int)));
    }
    flightModeComboBoxes << ui->flightMode0 << ui->flightMode1 << ui->flightMode2 << ui->flightMode3 << ui->flightMode4;

    setActive(CHANNEL_THROTTLE, true);

    connect(ui->receiverType, SIGNAL(currentIndexChanged(int)), this, SLOT(onReceiverTypeChanged(int)));
    foreach (QComboBox *box, flightModeComboBoxes) {
        connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(onFlightModeChanged(int)));
    }
    debouncer.setInterval(1000);
    debouncer.setSingleShot(true);
    connect(&debouncer, SIGNAL(timeout()), this, SLOT(debounceTimeout()));
    connect(ui->configureStart, SIGNAL(clicked()), this, SLOT(onConfigStart()));
    connect(ui->configureNext, SIGNAL(clicked()), this, SLOT(onConfigNext()));

    ui->configureNext->setVisible(false);
    enableAll(false);
    setActive(CHANNEL_ALL, false);
    setStepText("Board not connected");
}

bool MainWidget::setFlightModes(QStringList text)
{
    for(int i = 0; i < text.length(); ++i)
    {
        ui->flightMode0->insertItem(i, text.at(i));
        ui->flightMode1->insertItem(i, text.at(i));
        ui->flightMode2->insertItem(i, text.at(i));
        ui->flightMode3->insertItem(i, text.at(i));
        ui->flightMode4->insertItem(i, text.at(i));
    }
    return true;
}

bool MainWidget::setReceiverTypes(QStringList text, QList<QVariant> data)
{
    if(text.length() != data.length())
        return false;
    for(int i = 0; i < text.length(); ++i)
    {
        ui->receiverType->addItem(text.at(i), data.at(i).toInt());
    }
    return true;
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::setEnabled(MainWidget::channels channel, bool enable)
{
    QList<channelControls> controls;
    if(channel == CHANNEL_ALL)
        controls = channelUIGroups.values();
    else
        controls.append(channelUIGroups.value(channel));
    foreach(channelControls control, controls)
    {
        if(control.channelFunction)
            control.channelFunction->setEnabled(enable);
        if(control.channelInverted) {
            control.channelInverted->setEnabled(enable);
            control.channelInverted->style()->unpolish(control.channelInverted);
            control.channelInverted->setProperty("disabled", !enable);
            control.channelInverted->style()->polish(control.channelInverted);
        }
        if(control.channelNumber)
            control.channelNumber->setEnabled(enable);
        if(control.channelPercentage)
            control.channelPercentage->setEnabled(enable);
        if(control.channelSlider) {
            control.channelSlider->setEnabled(enable);
            control.channelSlider->style()->unpolish(control.channelSlider);
            control.channelSlider->setProperty("disabled", !enable);
            control.channelSlider->style()->unpolish(control.channelSlider);
        }
    }
}

void MainWidget::setChannelValue(MainWidget::channels channel,int value)
{
    channelControls control = channelUIGroups.value(channel);
    if(control.channelSlider)
        control.channelSlider->setValue(value);
}

void MainWidget::setChannelInverted(MainWidget::channels channel, bool value)
{
    channelControls control = channelUIGroups.value(channel);
    if(control.channelInverted) {
        control.channelInverted->style()->unpolish(control.channelInverted);
        control.channelInverted->setChecked(value);
        control.channelInverted->style()->polish(control.channelInverted);
    }
}

void MainWidget::onSliderValueChanged(int)
{
    foreach (channelControls control, channelUIGroups) {
        if(control.channelSlider == (QSlider*) sender())
            if(control.channelPercentage)
                control.channelPercentage->setText(QString::number(control.channelSlider->value())+ '%');
    }
}

void MainWidget::enableAll(bool value)
{
    setEnabled(CHANNEL_ALL, value);
    this->style()->unpolish(this);
    ui->configureStart->setEnabled(value);
    ui->configureNext->setEnabled(value);
    ui->flightMode0->setEnabled(value);
    ui->flightMode1->setEnabled(value);
    ui->flightMode2->setEnabled(value);
    ui->flightMode3->setEnabled(value);
    ui->flightMode4->setEnabled(value);
    ui->receiverType->setEnabled(value);
    this->style()->polish(this);
}

void MainWidget::setActive(MainWidget::channels channel, bool active)
{
    QList<channelControls> controls;
    if(channel == CHANNEL_ALL)
        controls = channelUIGroups.values();
    else
        controls.append(channelUIGroups.value(channel));
    foreach(channelControls control, controls)
    {
        if(control.channelDone) {
            control.channelDone->setProperty("active", active);
            control.channelDone->style()->unpolish(control.channelDone);
            control.channelDone->style()->polish(control.channelDone);
        }
        if(control.channelFunction) {
            control.channelFunction->setProperty("active", active);
            control.channelFunction->style()->unpolish(control.channelFunction);
            control.channelFunction->style()->polish(control.channelFunction);
        }
        if(control.channelInverted) {
            control.channelInverted->setProperty("active", active);
            control.channelInverted->style()->unpolish(control.channelInverted);
            control.channelInverted->style()->polish(control.channelInverted);
        }
        if(control.channelNumber) {
            control.channelNumber->setProperty("active", active);
            control.channelNumber->style()->unpolish(control.channelNumber);
            control.channelNumber->style()->polish(control.channelNumber);
        }
        if(control.channelPercentage) {
            control.channelPercentage->setProperty("active", active);
            control.channelPercentage->style()->unpolish(control.channelPercentage);
            control.channelPercentage->style()->polish(control.channelPercentage);
        }
        if(control.channelSlider) {
            control.channelSlider->setProperty("active", active);
            control.channelSlider->style()->unpolish(control.channelSlider);
            control.channelSlider->style()->polish(control.channelSlider);
        }
    }
}

void MainWidget::setDone(MainWidget::channels channel, bool value)
{
    QList<channelControls> controls;
    if(channel == CHANNEL_ALL)
        controls = channelUIGroups.values();
    else
        controls.append(channelUIGroups.value(channel));
    foreach(channelControls control, controls)
    {
        if(control.channelDone) {
            control.channelDone->setVisible(value);
        }
    }
}

void MainWidget::setConnected(bool value)
{
    ui->connected->setChecked(value);
    if(value)
        ui->connected->setText("Connected");
    else
        ui->connected->setText("Disconnected");
    ui->connected->style()->unpolish(ui->connected);
    ui->connected->style()->polish(ui->connected);
}

void MainWidget::setCurrentReceiverType(int type)
{
    for(int i = 0; i < ui->receiverType->count(); ++i) {
        if(ui->receiverType->itemData(i).toInt() == type) {
            ui->receiverType->setCurrentIndex(i);
            break;
        }
    }
}

void MainWidget::setCurrentFlightMode(int number, int mode)
{
    QComboBox *cb;
    if(number > flightModeComboBoxes.length())
        return;
    cb = flightModeComboBoxes.at(number);
    cb->setCurrentIndex(mode);
}

void MainWidget::onReceiverTypeChanged(int index)
{
    emit receiverTypeChanged(ui->receiverType->itemData(index).toInt());
}

void MainWidget::onFlightModeChanged(int index)
{
    QComboBox *box = qobject_cast<QComboBox*>(sender());
    emit flightModeChanged(flightModeComboBoxes.indexOf(box), index);
}

void MainWidget::onConfigStart()
{
    debounce();
    if(!configRunning)
    {
        ui->configureStart->setText("Cancel");
        ui->configureNext->setVisible(true);
        emit configStartRequested();
        configRunning = true;
    }
    else
        onConfigCancel();
}

void MainWidget::onConfigCancel()
{
    ui->configureNext->setVisible(false);
    ui->configureStart->setText("Configure R/C");
    emit configCancelRequested();
    configRunning = false;
}

void MainWidget::onConfigEnded()
{
    ui->configureNext->setVisible(false);
    ui->configureStart->setText("Configure R/C");
    configRunning = false;
}

void MainWidget::disableNext(bool value)
{
    ui->configureNext->setEnabled(!value);
}

void MainWidget::onConfigNext()
{
    emit configNextRequested();
}

void MainWidget::setStepText(QString text)
{
    ui->stepText->setText(text);
}

void MainWidget::debounceTimeout()
{
    ui->configureStart->blockSignals(false);
    ui->configureNext->blockSignals(false);
}

void MainWidget::setChannelNumberRoll(int value)
{
    QString text;
    if(value == -1)
        text = "?";
    else
        text = QString::number(value);
    ui->channelNumberRoll->setText(text);
}

void MainWidget::setChannelNumberPitch(int value)
{
    QString text;
    if(value == -1)
        text = "?";
    else
        text = QString::number(value);
    ui->channelNumberPitch->setText(text);
}

void MainWidget::setChannelNumberYaw(int value)
{
    QString text;
    if(value == -1)
        text = "?";
    else
        text = QString::number(value);
    ui->channelNumberYaw->setText(text);
}

void MainWidget::setChannelNumberThrottle(int value)
{
    QString text;
    if(value == -1)
        text = "?";
    else
        text = QString::number(value);
    ui->channelNumberThrottle->setText(text);
}

void MainWidget::setChannelNumberFlightMode(int value)
{
    QString text;
    if(value == -1)
        text = "?";
    else
        text = QString::number(value);
    ui->channelNumberFlightMode->setText(text);
}

void MainWidget::setNumberOfFlightModes(int value)
{
    for(int i = 0; i < 5; ++i)
    {
        if(i < value)
            flightModeComboBoxes.at(i)->setVisible(true);
        else
            flightModeComboBoxes.at(i)->setVisible(false);
    }
}

void MainWidget::setActiveFlighMode(int value)
{
    for(int i = 0; i < 5; ++i)
    {
        flightModeComboBoxes.at(i)->style()->unpolish(flightModeComboBoxes.at(i));
        if(i == value)
            flightModeComboBoxes.at(i)->setProperty("isActive", true);
        else
            flightModeComboBoxes.at(i)->setProperty("isActive", false);
        flightModeComboBoxes.at(i)->style()->polish(flightModeComboBoxes.at(i));
    }
}
void MainWidget::debounce()
{
    ui->configureStart->blockSignals(true);
    ui->configureNext->blockSignals(true);
    debouncer.start();
}

void MainWidget::updateTabsIcon(int currentTab)
{
    if(currentTab == 0)
    {
        ui->tabWidget->setTabIcon(0, QIcon(":/images/resources/wireless.png"));
        ui->tabWidget->setTabIcon(1, QIcon(":/images/resources/airplane_grey.png"));
        ui->tabWidget->setTabIcon(2, QIcon(":/images/resources/debug_grey.png"));
    }
    else if(currentTab == 1)
    {
        ui->tabWidget->setTabIcon(0, QIcon(":/images/resources/wireless_grey.png"));
        ui->tabWidget->setTabIcon(1, QIcon(":/images/resources/airplane.png"));
        ui->tabWidget->setTabIcon(2, QIcon(":/images/resources/debug_grey.png"));
    }
    else if(currentTab == 2)
    {
        ui->tabWidget->setTabIcon(0, QIcon(":/images/resources/wireless_grey.png"));
        ui->tabWidget->setTabIcon(1, QIcon(":/images/resources/airplane_grey.png"));
        ui->tabWidget->setTabIcon(2, QIcon(":/images/resources/debug.png"));
    }
}
