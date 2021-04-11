#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QByteArray>
#include <QtSerialPort/QSerialPort>
#include <QTimer>

namespace Ui {
class Widget;
}

#define SUC_I 'A'
            #define SET_I_START 'B'
            #define SET_I_END 'C'
            #define GET_I_START  'D'
            #define GET_I_END 'E'
            #define ERR_I 'F'

            #define SUC_U  'G'
            #define SET_U_START  'H'
            #define SET_U_END 'I'
            #define GET_U_START  'J'
            #define GET_U_END 'K'
            #define ERR_U  'L'

            #define SUC_PWM 'M'
            #define SET_PWM_START 'N'
            #define SET_PWM_END  'O'
            #define GET_PWM_START 'P'
            #define GET_PWM_END  'Q'
            #define ERR_PWM 'Q'

            #define SUC_MODE 'R'
            #define SET_MODE_START 'S'
            #define SET_MODE_END 'T'
            #define GET_MODE_START  'U'
            #define GET_MODE_END 'V'
            #define ERR_MODE  'W'

            #define CLK_TOKEN '?'
            #define BUSY_TOKEN '!'

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    QSerialPort serial;
    QTimer comTimer, readTimer;
    QByteArray sendMsg, recMsg;
    int charsToSend, sendActive, recActive, recCnt, mode, modeOld;

    void DisasmblVal(unsigned int val, QByteArray* charValArr);

private slots:
    void Slot_Read();
    void Slot_IsollChange(int iSoll);
    void Slot_UsollChange(int uSoll);
    void Slot_PWMsollChange(int pwmSoll);
    void Slot_Send();
    void Slot_ComTimer();
    void Slot_Usel();
    void Slot_Isel();
    void Slot_PWMsel();
};

#endif // WIDGET_H
