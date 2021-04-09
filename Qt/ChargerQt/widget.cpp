#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    serial.setPortName("COM7");
    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);
    serial.open(QIODevice::ReadWrite);

    connect(&serial, SIGNAL(readyRead()), this, SLOT(Slot_Read()));
    //connect(ui->pushButton_Send, SIGNAL(clicked()), this, SLOT(Slot_Send()));
    connect(ui->horizontalSlider_Isoll, SIGNAL(valueChanged(int)), this, SLOT(Slot_IsollChange(int)));
    connect(ui->horizontalSlider_Usoll, SIGNAL(valueChanged(int)), this, SLOT(Slot_UsollChange(int)));
    connect(ui->horizontalSlider_PWMsoll, SIGNAL(valueChanged(int)), this, SLOT(Slot_PWMsollChange(int)));
    connect(&comTimer, SIGNAL(timeout()), this, SLOT(Slot_ComTimer()));

    connect(ui->radioButton_I, SIGNAL(clicked()), this, SLOT(Slot_Isel()));
    connect(ui->radioButton_U, SIGNAL(clicked()), this, SLOT(Slot_Usel()));
    connect(ui->radioButton_PWM, SIGNAL(clicked()), this, SLOT(Slot_PWMsel()));

    comTimer.setInterval(200);
    comTimer.start();
    recActive = 0;

    mode = 0;
}

Widget::~Widget()
{
    delete ui;
}

void Widget::Slot_Read()
{
    int measVal;
    char dummyChar;
     serial.getChar(&dummyChar);
     qDebug() << "SLOT Read dummyChar " << dummyChar;
     recCnt++;
     if(recActive == 1)recMsg.append(dummyChar);
     if((dummyChar == GET_PWM_START) ||
        (dummyChar == GET_U_START) ||
        (dummyChar == GET_I_START) ||
        (dummyChar == GET_MODE_START))
     {
         recMsg.clear();
         recMsg.append(dummyChar);
         recActive = 1;
         recCnt = 0;
     }
     if((((dummyChar == GET_PWM_END) ||
          (dummyChar == GET_U_END) ||
          (dummyChar == GET_I_END))) &&
          (recCnt >= 2))
     {
         if(recMsg.count() == 6)
         {

            measVal =   ((unsigned int)((recMsg.at(1)-'0')) * 1000);
            measVal +=  ((unsigned int)((recMsg.at(2)-'0')) * 100);
            measVal +=  ((unsigned int)((recMsg.at(3)-'0')) * 10);
            measVal +=  ((unsigned int)((recMsg.at(4)-'0')) * 1);
/*
            qDebug() << "Slot receive  recMsg: " << recMsg
                     << "recMsg.at(1)" << recMsg.at(1) << " " << ((unsigned int)((recMsg.at(1)-'0')) * 1000)
                     << "recMsg.at(2)" << recMsg.at(2) << " " << ((unsigned int)((recMsg.at(2)-'0')) * 100)
                     << "recMsg.at(3)" << recMsg.at(3) << " " << ((unsigned int)((recMsg.at(3)-'0')) * 10)
                     << "recMsg.at(4)" << recMsg.at(4) << " " << ((unsigned int)((recMsg.at(4)-'0')) * 1)
                     << measVal;
*/
            if(dummyChar == GET_I_END)ui->lcdNumber_Iist->display(measVal);
            if(dummyChar == GET_U_END)ui->lcdNumber_Uist->display(measVal);
            if(dummyChar == GET_PWM_END)ui->lcdNumber_PWMist->display(measVal);
         }

         recActive = 0;
     }

     if(sendActive == 1)
     {
        dummyChar = sendMsg.at(charsToSend);
        serial.putChar(dummyChar);
        //qDebug() <<"Slot Read sendActive putChar " << dummyChar;
        charsToSend++;
     }
     else if(recActive == 1)
     {
       // qDebug() <<"Slot Read recActive";
         serial.putChar('?');
     }

    if((dummyChar == SET_PWM_END) || (dummyChar == SET_U_END) || (dummyChar == SET_I_END) || (dummyChar == SET_MODE_END))
    {
        sendActive = 0;
    }
}

void Widget::Slot_IsollChange(int iSoll)
{
    if(ui->radioButton_I->isChecked())
    {
        ui->lcdNumber_Isoll->display(iSoll);
        if(sendActive == 0)
        {
            //Slot_Send();
            sendActive = -1;
        }
    }
}

void Widget::Slot_UsollChange(int iSoll)
{
    if(ui->radioButton_U->isChecked())
    {
        ui->lcdNumber_Usoll->display(iSoll);
        if(sendActive == 0)
        {
            //Slot_Send();
            sendActive = -1;
        }
    }
}

void Widget::Slot_PWMsollChange(int iSoll)
{
    if(ui->radioButton_PWM->isChecked())
    {
        ui->lcdNumber_PWMsoll->display(iSoll);
        if(sendActive == 0)
        {
            //Slot_Send();
            sendActive = -1;
        }
    }
}

void Widget::Slot_Send()
{
    if(modeOld == mode)
    {
        if(ui->radioButton_I->isChecked())
        {
            sendMsg.clear();
            sendMsg.append(SET_I_START);
            DisasmblVal(ui->horizontalSlider_Isoll->value(), &sendMsg);
            sendMsg.append(SET_I_END);
        }
        if(ui->radioButton_U->isChecked())
        {
            sendMsg.clear();
            sendMsg.append(SET_U_START);
            DisasmblVal(ui->horizontalSlider_Usoll->value(), &sendMsg);
            sendMsg.append(SET_U_END);
        }
        if(ui->radioButton_PWM->isChecked())
        {
            sendMsg.clear();
            sendMsg.append(SET_PWM_START);
            DisasmblVal(ui->horizontalSlider_PWMsoll->value(), &sendMsg);
            sendMsg.append(SET_PWM_END);
        }
    }
    else
    {
        sendMsg.clear();
        sendMsg.append(SET_MODE_START);
        DisasmblVal(mode, &sendMsg);
        sendMsg.append(SET_MODE_END);
        modeOld = mode;
    }

    //qDebug() <<"Slot_Send() " << sendMsg;
    charsToSend = 1;
    //qDebug() <<"Slot_Send() " << sendMsg.at(0);
    serial.putChar(sendMsg.at(0));
}

void Widget::DisasmblVal(unsigned int val, QByteArray* charValArr)
{
    unsigned int valLoadIndx = 0;

    for(valLoadIndx = 4; valLoadIndx > 0; valLoadIndx--)
    {
        if(val > 8999){charValArr->append('9'); val -= 9000;}
        else if(val > 7999){charValArr->append('8'); val -= 8000;}
        else if(val > 6999){charValArr->append('7'); val -= 7000;}
        else if(val > 5999){charValArr->append('6'); val -= 6000;}
        else if(val > 4999){charValArr->append('5'); val -= 5000;}
        else if(val > 3999){charValArr->append('4'); val -= 4000;}
        else if(val > 2999){charValArr->append('3'); val -= 3000;}
        else if(val > 1999){charValArr->append('2'); val -= 2000;}
        else if(val > 999) {charValArr->append('1'); val -= 1000;}
        else               {charValArr->append('0');}

        val = val*10;
    }
}

void Widget::Slot_ComTimer()
{
    static int whichReq = GET_I_START;
    //qDebug() << "Timer Slot sendActive " << sendActive <<"   recActive " << recActive  ;
    if((sendActive == -1) && (recActive == 0))
    {
        sendActive = 1;
        Slot_Send();
    }
    else if((sendActive == 0) && (recActive == 0))
    {
        if(whichReq == GET_I_START)
        {
            serial.putChar(whichReq);
            whichReq = GET_U_START;
            qDebug() << "Change Req to  " << whichReq;
        }
        else if(whichReq == GET_U_START)
        {
            serial.putChar(whichReq);
            whichReq = GET_PWM_START;
            qDebug() << "Change Req to  " << whichReq;
        }
        else if(whichReq == GET_PWM_START)
        {
            serial.putChar(whichReq);
            whichReq = GET_I_START;
            qDebug() << "Change Req to  " << whichReq;
        }

        recActive = 1;

    }
}

void Widget::Slot_Usel()
{
    ui->radioButton_I->setChecked(false);
    ui->radioButton_PWM->setChecked(false);
    ui->radioButton_U->setChecked(true);
    mode = 'v';
    sendActive = -1;
}
void Widget::Slot_Isel()
{
    ui->radioButton_PWM->setChecked(false);
    ui->radioButton_U->setChecked(false);
    ui->radioButton_I->setChecked(true);
    mode = 'i';
    sendActive = -1;
}
void Widget::Slot_PWMsel()
{
    ui->radioButton_I->setChecked(false);
    ui->radioButton_U->setChecked(false);
    ui->radioButton_PWM->setChecked(true);
    mode = 'p';
    sendActive = -1;
}
