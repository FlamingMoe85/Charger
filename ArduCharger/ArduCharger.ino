#include <LiquidCrystal.h>


#include "C:/Charger/ArduCharger/Communication.h"
#include "C:/Charger/ArduCharger/mySpiStuff.h"

const int rs = PB1, en = PB5, d4 = PB6, d5 = PB7, d6 = PB8, d7 = PB9;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

unsigned int iIst;
unsigned int cnt = 0, cntrlFlag = 0;

struct cell{
  unsigned int cellState;
  unsigned int packState;
  unsigned int cellVolt;
  unsigned int smSel;
  unsigned int actionVal;
  unsigned int actionSM;
  unsigned int degrateCur;
}cellStrct;
#define AMT_OF_CELLS  1
cell cellArr[AMT_OF_CELLS];
unsigned int voltMeas[AMT_OF_CELLS];

#define LOW_CELL      0
#define MEDIUM_CELL   1
#define HIGH_CELL     2
#define FULL_CELL     3

#define SET_I_ACT     0
#define SET_U_ACT     1
#define SET_IDLE      2
#define HIGH_ACT      3

unsigned int cellStatus = LOW_CELL;

void SetCellVolt(unsigned int cellSel, unsigned int cellVal)
{
  if(cellSel >= AMT_OF_CELLS) return;
  cellArr[cellSel].cellVolt = cellVal;
}

void ResetCellStates()
{
  for(int i=0; i<AMT_OF_CELLS; i++)
  {
    cellArr[i].cellState = LOW_CELL;
    cellArr[i].packState = LOW_CELL;
    cellArr[i].cellVolt = 0;
  }
  
}

#define LOW_WM  857
#define MED_WM  990
#define HGH_WM  1000

#define LOW_CUR 80
#define HGH_CUR 163

void CellEvaluate(unsigned int* cellVolts)
{
  Serial.println(" ");
  Serial.print("Cell Eval_Volt:");
  for(int i=0; i<AMT_OF_CELLS; i++)
  {
    Serial.print(cellVolts[i]);
    /*
    if(cellVolts[i]>= HGH_WM)
    {
      cellArr[i].cellState = FULL_CELL;
      Serial.println(" Cell State Full");
    }
    else if((cellVolts[i]>= MED_WM) && (cellArr[i].cellState != FULL_CELL)) 
    */
    if(cellVolts[i]>= MED_WM) 
    {
      cellArr[i].cellState = HIGH_CELL;
      Serial.println("Cell State High");
    }
    else if((cellVolts[i]>= LOW_WM) && (cellArr[i].cellState != HIGH_CELL)&& (cellArr[i].cellState != FULL_CELL)) 
    {
      cellArr[i].cellState = MEDIUM_CELL;
      Serial.println("Cell State Medium");
    }
    else if((cellArr[i].cellState != HIGH_CELL) && (cellArr[i].cellState != MEDIUM_CELL)&& (cellArr[i].cellState != FULL_CELL)) 
    {
      cellArr[i].cellState = LOW_CELL;
      Serial.println("Cell State LOW");
    }
  }
}

void PackEvaluate()
{
  unsigned int lowestState = FULL_CELL;
  
      Serial.print("Pack Eval");
  for(int i=0; i<AMT_OF_CELLS; i++)
  {
    if(cellArr[i].cellState < lowestState) lowestState = cellArr[i].cellState;
      Serial.print(i);
      Serial.print("-Cell State:");
      Serial.print(cellArr[i].cellState);
      Serial.print(" ");
  }

  for(int i=0; i<AMT_OF_CELLS; i++)
  {
    cellArr[i].packState = lowestState;
    Serial.print(i);
      Serial.print("-Pack State:");
      Serial.print(cellArr[i].packState);
      Serial.print(" ");
  }
  Serial.println(" ");
}

void ChargeStategy()
{
  for(int i=0; i<AMT_OF_CELLS; i++)
  {
    Serial.print("Charge Strat Cell:");
    Serial.print(i);
    Serial.print(" ");
   if((cellArr[i].packState == LOW_CELL) || (cellArr[i].packState == MEDIUM_CELL))
   {
    Serial.print(" PS LOW/MED ");
    if(cellArr[i].cellState == LOW_CELL)
    {
      Serial.print(" CS LOW ");
      cellArr[i].smSel = SET_I_ACT;
      cellArr[i].actionVal = LOW_CUR;
    }
    if(cellArr[i].cellState == MEDIUM_CELL)
    {
      Serial.print(" CS MED ");
      cellArr[i].smSel = SET_I_ACT;
      cellArr[i].actionVal = HGH_CUR;
    }
    else if(cellArr[i].cellState == HIGH_CELL)
    {
      Serial.print(" CS HIGH ");
      cellArr[i].smSel = SET_U_ACT;
      cellArr[i].actionVal = 0;
    }
    else if(cellArr[i].cellState == FULL_CELL)
    {
      Serial.print(" CS Full ");
      cellArr[i].smSel = SET_IDLE;
      cellArr[i].actionVal = 0;
    }
   }
   else if(cellArr[i].packState == HIGH_CELL)
   {
    Serial.print(" PS High ");
    if((cellArr[i].cellState == LOW_CELL) || (cellArr[i].cellState == MEDIUM_CELL)|| (cellArr[i].cellState == FULL_CELL))
    {
      Serial.print(" CS LOW/MED/Full ");
      cellArr[i].smSel = SET_IDLE;
      cellArr[i].actionVal = 0;
    }
    else if(cellArr[i].cellState == HIGH_CELL)
    {
      Serial.print(" CS High ");
      cellArr[i].smSel = HIGH_ACT;
      cellArr[i].actionVal = 0;
    }
   }
   else // FULL_PACK 
   {
    
      Serial.print(" PS Full ");
      cellArr[i].smSel = SET_IDLE;
      cellArr[i].actionVal = 0;
   }
  }
  
      Serial.println(" ");
}

unsigned int stateMach_U = 0;
unsigned int stMach_U_measVals[3];
unsigned int StateMachine_U(cell* curCell)
{
  
      Serial.print("SM U ");
  switch(stateMach_U)
  {
    case 0:
      {
        Serial.print("Case 0");
        //(*curCell).actionSM = SET_I_START;
        //(*curCell).actionVal = 0;
        
        digitalWrite(PA13, LOW);
        delay(500);
        (*curCell).actionVal = Get_U();
        digitalWrite(PA13, HIGH);
        Serial.print("Get_U:");
        Serial.print((*curCell).actionVal);
        Serial.print(" ");
        Set_U((*curCell).actionVal);
        Set_Mode('u');
        stateMach_U = 4;
        //stateMach_U = 1;
        break;
      }
    case 1:
      {
        //(*curCell).actionSM = GET_U_START;
        //(*curCell).actionVal = 0;
        
        stateMach_U = 2;
        break;
      }
    case 2:
      {
        (*curCell).actionSM = SET_U_START;
        (*curCell).actionVal = (*curCell).cellVolt;
        stateMach_U = 3;
        break;
      }
    case 3:
      {
        (*curCell).actionSM = SET_MODE_START;
        (*curCell).actionVal = 'u';
        break;
      }
    case 4:
      {
        Serial.print("Case 4");
        break;
      }
      
  }
  Serial.println(" ");
}

unsigned int stateMach_I = 0;
unsigned int stMach_I_measVals[3];
unsigned int StateMachine_I(cell* curCell) 
{
   Serial.print("SM I ");
  switch(stateMach_I)
  {
    case 0:
      {
         Serial.print("Case 0 ");
         Serial.print("Set_I ");
         Serial.print((*curCell).actionVal);
        Set_I((*curCell).actionVal);
        Set_Mode('i');
        stateMach_I = 2;
        //(*curCell).actionSM = SET_I_START;
        //action value was set in "ChargeStategy"
        //stateMach_I = 1;
        break;
      }
    case 1:
      {
        //(*curCell).actionSM = SET_MODE_START;
        //(*curCell).actionVal = 'i';
        break;
      }
    case 2:
      {
        Serial.print("Case 2 ");
         Serial.print("Set_I ");
         Serial.print((*curCell).actionVal);
        Set_I((*curCell).actionVal);
        Set_Mode('i');
        stateMach_I = 2;
        break;
      }
  }
      Serial.println(" ");
}

void ResetStateMI()
{
  stateMach_I = 0;
}

unsigned int stateMach_H = 0;
unsigned int tmpMeas;
unsigned int StateMachine_H(cell* curCell)
{
  Serial.print("SM H ");
  switch(stateMach_H)
  {
    case 0:
      {
        Serial.print("Case 0 ");
        //Set_I(0);
        (*curCell).degrateCur = HGH_CUR;
        Set_I((*curCell).degrateCur);
        Set_Mode('i');
        stateMach_H = 1;
      }
    case 1:
      {
        tmpMeas = Get_U();
        Serial.print("Case 1 ");
        digitalWrite(PC13, LOW);
        delay(500);
        (*curCell).cellVolt = Get_U();
        digitalWrite(PC13, HIGH);
        Serial.print("Get_U ");
        Serial.print((*curCell).cellVolt);
        Serial.print(" ");

lcd.setCursor(8, 1);
lcd.print("    ");
lcd.setCursor(6, 1);
lcd.print("U:");
lcd.print((*curCell).cellVolt);
        
        if((*curCell).cellVolt >= HGH_WM) 
        {
          Set_I(0);
          (*curCell).cellState = FULL_CELL;
        }
        else
        {
          
          
          if(tmpMeas > 1010)
          {
            if((*curCell).degrateCur > 10) 
            {
              (*curCell).degrateCur -= 10;
            }
            else
            {
              (*curCell).degrateCur = 0;
            }
          }
          else if(tmpMeas > 1005)
          {
            if((*curCell).degrateCur > 5) 
            {
              (*curCell).degrateCur -= 5;
            }
            else
            {
              (*curCell).degrateCur = 0;
            }
          }
          else if(tmpMeas > 1002)
          {
            if((*curCell).degrateCur > 0) (*curCell).degrateCur--;
          }
          Serial.print(" Deg_I ");
          Serial.print((*curCell).degrateCur);
          Serial.print(" ");
          if((*curCell).degrateCur < 5)
          {
            (*curCell).cellState = FULL_CELL;
            (*curCell).degrateCur = 0;
          }
          Serial.print("Set Low Cur");
          Set_I((*curCell).degrateCur);
        }
        break;
      }
  }
  Serial.println(" ");
}

void ResetSMs()
{
  stateMach_H = 0;
  stateMach_I = 0;
  stateMach_U = 0;

  for(int i=0; i<AMT_OF_CELLS; i++)
  {
    cellArr[i].cellState = 0;
    cellArr[i].packState = 0;
  }
  Set_I(0);
  Set_U(0);
}

unsigned int AsmValue(char* recMsg)
{
  unsigned int retVal;
  retVal =   ((unsigned int)((recMsg[2]-'0')) * 1000);
  retVal +=  ((unsigned int)((recMsg[3]-'0')) * 100);
  retVal +=  ((unsigned int)((recMsg[4]-'0')) * 10);
  retVal +=  ((unsigned int)((recMsg[5]-'0')) * 1);
  return retVal;
}

bool TicksForData(char checkToken)
{
  Serial.print(' ');
  for(int i=0; i<7; i++)
  {
    dataInBuf[i] = SPI_2.transfer('?');//sendSPI2('?');
    delay(1);
    Serial.print((char)dataInBuf[i]);
  }
  Serial.print(' ');
  if(dataInBuf[6] == checkToken) return true;
  
  return false;
}

void Set_I(unsigned int val)
{
  DisasmblVal(val, sendValCharArr, 4);
  headTx = 0;
  dataOutBuf[headTx++] = SET_I_START;
  CopyValArrToBuf(sendValCharArr, dataOutBuf, 4);
  dataOutBuf[headTx++] = SET_I_END;
  
  SendSpiMsg();
}

unsigned int Get_I()
{
  for(int k=0; k<6; k++)
  {
     sendSPI2(GET_I_START);
    delay(100);
   if (TicksForData(GET_I_END))
   {
    return AsmValue(&(dataInBuf[0]));
   }
  }
}

unsigned int GetIsoll()
{
  for(int k=0; k<6; k++)
  {
     sendSPI2(GET_I_SOLL_STA);
    delay(100);
   if (TicksForData(GET_I_SOLL_END))
   {
    return AsmValue(&(dataInBuf[0]));
   }
  }
}

unsigned int GetUsoll()
{
  for(int k=0; k<6; k++)
  {
     sendSPI2(GET_U_SOLL_START);
    delay(100);
   if (TicksForData(GET_U_SOLL_END))
   {
    return AsmValue(&(dataInBuf[0]));
   }
  }
}

void Set_U(unsigned int val)
{
  DisasmblVal(val, sendValCharArr, 4);
  headTx = 0;
  dataOutBuf[headTx++] = SET_U_START;
  CopyValArrToBuf(sendValCharArr, dataOutBuf, 4);
  dataOutBuf[headTx++] = SET_U_END;
  SendSpiMsg();
}
unsigned int Get_U()
{
 
  for(int k=0; k<6; k++)
  {
     sendSPI2(GET_U_START);
    delay(100);
   if (TicksForData(GET_U_END))
   {
    return AsmValue(&(dataInBuf[0]));
   }
  }
}

void Set_Mode(unsigned int val)
{
  DisasmblVal(val, sendValCharArr, 4);
  headTx = 0;
  dataOutBuf[headTx++] = SET_MODE_START;
  CopyValArrToBuf(sendValCharArr, dataOutBuf, 4);
  dataOutBuf[headTx++] = SET_MODE_END;
  SendSpiMsg();
}
unsigned int Get_Mode()
{
  for(int k=0; k<6; k++)
  {
     sendSPI2(GET_MODE_START);
    delay(100);
   if (TicksForData(GET_MODE_END))
   {
    return AsmValue(&(dataInBuf[0]));
   }
  }
}

void setup() {
  // put your setup code here, to run once:

lcd.begin(16, 2);
lcd.setCursor(0, 0);
//lcd.print("Jasson in House");

SPI_2.begin(); //Initialize the SPI_2 port.
SPI_2.setBitOrder(MSBFIRST); // Set the SPI_2 bit order
SPI_2.setDataMode(SPI_MODE0); //Set the  SPI_2 data mode 0
SPI_2.setClockDivider(SPI_CLOCK_DIV256);  // Use a different speed to SPI 1
pinMode(SPI2_NSS_PIN, OUTPUT);

pinMode(PC13, OUTPUT);
digitalWrite(PC13, LOW);

pinMode(PB11, INPUT);

DisasmblVal('i', sendValCharArr, 4);
headTx = 0;
dataOutBuf[headTx++] = SET_MODE_START;
CopyValArrToBuf(sendValCharArr, dataOutBuf, 4);
dataOutBuf[headTx++] = SET_MODE_END;
SendSpiMsg();
delay(1000);
Set_I(0);
Serial.begin(9600);
}

void loop() {

//Set_U(500);
/*
DisasmblVal('i', sendValCharArr, 4);
headTx = 0;
dataOutBuf[headTx++] = SET_MODE_START;
CopyValArrToBuf(sendValCharArr, dataOutBuf, 4);
dataOutBuf[headTx++] = SET_MODE_END;
SendSpiMsg();
Set_I(180);


sendSPI2(GET_MODE_START);
TicksForData(GET_MODE_START);
sendSPI2(GET_I_SOLL_STA);
TicksForData(GET_I_SOLL_STA);
*/
/*
if((Serial.available())) 
{
  char tC = Serial.read();
  if((tC == 'h')) 
  {
    digitalWrite(SPI2_NSS_PIN, HIGH);
  }
  else if((tC == 'l')) 
  {
    digitalWrite(SPI2_NSS_PIN, LOW);
  }
  else
  {
    Serial.print((char)SPI_2.transfer(tC));
  }
 }
*/
  if(digitalRead(PB11) == HIGH)
  {
    cntrlFlag = 1;
    digitalWrite(PC13, HIGH);
  }
  else
  {
    cntrlFlag = 0;
    ResetSMs();
    digitalWrite(PC13, LOW);
  }
  /*
if((Serial.available())) 
{
  if((Serial.read() == 'h')) 
  {
    cntrlFlag = 1;
    digitalWrite(PA8, HIGH);
    digitalWrite(PC13, HIGH);
  }
  else
  {
    cntrlFlag = 0;
    ResetSMs();
    Set_I(0);
    Set_Mode('i');
    digitalWrite(PA8, LOW);
    digitalWrite(PC13, LOW);
  }
}
*/

iIst = Get_Mode();
lcd.setCursor(2, 0);
lcd.print("   ");
lcd.setCursor(0, 0);
lcd.print("M:");
lcd.print(iIst);

iIst = GetIsoll();
lcd.setCursor(7, 0);
lcd.print("   ");
lcd.setCursor(5, 0);
lcd.print("I:");
lcd.print(iIst);

iIst = Get_U();
lcd.setCursor(2, 1);
lcd.print("    ");
lcd.setCursor(0, 1);
lcd.print("U:");
lcd.print(iIst);


if(cntrlFlag == 1)
{
  voltMeas[0] = iIst;
  CellEvaluate(voltMeas);
  PackEvaluate();
  ChargeStategy();
  if(cellArr[0].smSel == SET_I_ACT){StateMachine_I(&(cellArr[0]));}
  if(cellArr[0].smSel == SET_U_ACT){StateMachine_U(&(cellArr[0]));}
  if(cellArr[0].smSel == HIGH_ACT){StateMachine_H(&(cellArr[0]));}
  if(cellArr[0].smSel == SET_IDLE){Set_I(0); Set_Mode('i'); digitalWrite(PC13, LOW);}
}

iIst = Get_I();
lcd.setCursor(13, 0);
lcd.print("   ");
lcd.setCursor(11, 0);
lcd.print("I:");
lcd.print(iIst);


lcd.setCursor(15, 1);
lcd.print(cnt++);
if(cnt >= 9) cnt = 0;


//delay(1000);
}
