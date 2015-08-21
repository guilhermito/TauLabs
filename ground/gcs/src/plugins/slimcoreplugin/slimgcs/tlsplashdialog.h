/**
 ******************************************************************************
 * @file       tlsplashscreen.h
 * @author     Tau Labs, http://taulabs.org, Copyright (C) 2014
 * @addtogroup [Group]
 * @{
 * @addtogroup TLSplashScreen
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

#ifndef TLSPLASHSCREEN_H
#define TLSPLASHSCREEN_H

#include <QWidget>
#include <QMovie>
#include <QTimer>
#include <QPushButton>
#include "../core_global.h"

namespace Ui {
class TLSplashScreen;
}

class CORE_EXPORT TLSplashDialog : public QWidget
{
    Q_OBJECT

public:
    enum Buttons {
        BUTTONS_NO_BUTTONS = 0,
        BUTTONS_OK =        0x01,
        BUTTONS_CANCEL =    0x02,
        BUTTONS_NEXT =      0x04,
        BUTTONS_CUSTOM0 =   0x08,
        BUTTONS_CUSTOM1 =   0x10,
        DONT_SHOW_AGAIN =   0x20
    };
    enum DialogType {
        TYPE_WAIT,
        TYPE_QUESTION,
        TYPE_WARNING,
        TYPE_ERROR
    };

    explicit TLSplashDialog(QWidget *parent = 0);
    ~TLSplashDialog();
    void setSplashMovie(QMovie *movie);
    void setImage(QPixmap *image);
    void setText(QString text, bool waiting);
    void setup(DialogType type, int button, QString text, bool waitingText, Buttons autoCountDown, int buttonStartTime = 0, QObject *obj = NULL, bool showDontShow = false);
    void setup(QMovie *movie, int button, QString text, bool waitingText,Buttons autoCountDown, int buttonStartTime = 0, QObject *obj = NULL, bool showDontShow = false);
    void setup(QPixmap *image, int button, QString text, bool waitingText, Buttons autoCountDown, int buttonStartTime = 0, QObject *obj = NULL, bool showDontShow = false);
    void setCustomButton0Text(QString text);
    void setCustomButton1Text(QString text);
    Buttons getLastButtonClicked(){return lastButtonClicked;}
    void updateText(QString text);
private:
    Buttons lastButtonClicked;
    Ui::TLSplashScreen *ui;
    QString tempText;
    QString tempButtonText;
    int waitTextCount;
    QTimer waitTextTimer;
    QTimer buttonTimer;
    void setButtons(int button, Buttons buttonAutoClick, int buttonCountDown);
    int m_buttonCountDown;
    QPushButton *currentTimedButton;
    int currentButtons;
    QObject *currentObject;
private slots:
    void onWaitTextTimerTimeout();
    void onButtonClicked();
    void onButtonTimerTimeout();
signals:
    void okClicked(QObject *);
    void cancelClicked(QObject *);
    void nextClicked(QObject *);
    void custom0Clicked(QObject *);
    void custom1Clicked(QObject *);
};

#endif // TLSPLASHSCREEN_H
