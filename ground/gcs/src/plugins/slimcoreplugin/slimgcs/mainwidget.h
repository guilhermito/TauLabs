/**
 ******************************************************************************
 * @file       mainwidget.h
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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QHash>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QVariant>
#include <QComboBox>
#include <QTimer>
#include "../core_global.h"

namespace Ui {
class MainWidget;
}

class CORE_EXPORT MainWidget : public QWidget
{
    Q_OBJECT

public:
    struct channelControls
    {
        QLabel *channelNumber;
        QLabel *channelPercentage;
        QCheckBox *channelInverted;
        QLabel *channelFunction;
        QSlider *channelSlider;
        QLabel *channelDone;
    };
    enum channels {CHANNEL_NONE, CHANNEL_ALL, CHANNEL_ROLL, CHANNEL_PITCH, CHANNEL_THROTTLE, CHANNEL_YAW, CHANNEL_FLIGHT_MODE};
    explicit MainWidget(QWidget *parent = 0);
    bool setFlightModes(QStringList text);
    bool setReceiverTypes(QStringList text, QList<QVariant> data);
    ~MainWidget();
public slots:
    void setEnabled(channels, bool);
    void setActive(channels, bool);
    void setDone(channels, bool);
    void setConnected(bool);
    void setCurrentReceiverType(int);
    void setCurrentFlightMode(int number, int mode);
    void onReceiverTypeChanged(int);
    void onFlightModeChanged(int);
    void onConfigStart();
    void onConfigCancel();
    void onConfigNext();
    void setStepText(QString);
    void debounceTimeout();
    void setChannelNumberRoll(int);
    void setChannelNumberPitch(int);
    void setChannelNumberYaw(int);
    void setChannelNumberThrottle(int);
    void setChannelNumberFlightMode(int);
    void setNumberOfFlightModes(int);
    void setActiveFlighMode(int);
    void setChannelValue(MainWidget::channels channel, int value);
    void setChannelInverted(MainWidget::channels, bool);
    void onSliderValueChanged(int);
    void enableAll(bool value);
    void onConfigEnded();
    void disableNext(bool);
signals:
    void receiverTypeChanged(int newType);
    void flightModeChanged(int number, int mode);
    void configStartRequested();
    void configCancelRequested();
    void configNextRequested();
private:
    void debounce();
    QHash <channels, channelControls>channelUIGroups;
    Ui::MainWidget *ui;
    QList<QComboBox *>flightModeComboBoxes;
    QTimer debouncer;
    bool configRunning;
private slots:
    void updateTabsIcon(int);
};

#endif // MAINWIDGET_H
