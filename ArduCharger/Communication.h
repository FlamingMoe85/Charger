unsigned int iSoll = 200;
unsigned int headTx = 0;
#define BUF_SIZE  20
char dataInBuf[BUF_SIZE], dataOutBuf[BUF_SIZE], sendValCharArr[4];
#define SUC_I 'A'
#define SET_I_START 'B'
      #define SET_I_END 'C' 
      #define GET_I_START  'D'
      #define GET_I_END 'E'
      #define ERR_I 'F'
      #define GET_I_SOLL_STA 'X'
      #define GET_I_SOLL_END 'Y'

      #define SUC_U  'G'
      #define SET_U_START  'H'    
      #define SET_U_END 'I'
      #define GET_U_START  'J'
      #define GET_U_END 'K'
      #define ERR_U  'L'
      #define GET_U_SOLL_START 'Z'
      #define GET_U_SOLL_END '['

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

void DisasmblVal(unsigned int val, char* charValArr, char amtCif)
{
  unsigned int tmpVal;
  unsigned int valLoadIndx = 0;

  tmpVal = val;

  charValArr[0] = 0;
  charValArr[1] = 0;
  charValArr[2] = 0;
  charValArr[3] = 0;
  
  for(valLoadIndx = 4; valLoadIndx > 0; valLoadIndx--)
  {
    if(tmpVal > 8999){charValArr[valLoadIndx-1] = '9'; tmpVal -= 9000;}
    else if(tmpVal > 7999){charValArr[valLoadIndx-1] = '8'; tmpVal -= 8000;}
    else if(tmpVal > 6999){charValArr[valLoadIndx-1] = '7'; tmpVal -= 7000;}
    else if(tmpVal > 5999){charValArr[valLoadIndx-1] = '6'; tmpVal -= 6000;}
    else if(tmpVal > 4999){charValArr[valLoadIndx-1] = '5'; tmpVal -= 5000;}
    else if(tmpVal > 3999){charValArr[valLoadIndx-1] = '4'; tmpVal -= 4000;}
    else if(tmpVal > 2999){charValArr[valLoadIndx-1] = '3'; tmpVal -= 3000;}
    else if(tmpVal > 1999){charValArr[valLoadIndx-1] = '2'; tmpVal -= 2000;}
    else if(tmpVal > 999) {charValArr[valLoadIndx-1] = '1'; tmpVal -= 1000;}
    else          {charValArr[valLoadIndx-1] = '0';}

    tmpVal = tmpVal*10;
  }
}

void CopyValArrToBuf(char* valArr, char* txBuf, char amt)
{
  unsigned int loc;
  for(loc = 0; loc < amt; loc++)
  {
    txBuf[headTx++] = valArr[3-loc];
    if(headTx >= BUF_SIZE)headTx = 0;
  }
}
