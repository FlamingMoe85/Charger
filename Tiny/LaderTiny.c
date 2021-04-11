#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <avr/wdt.h>

#include "Regler_I.h"

volatile char slaveSelStatusNew = 0, slaveSelStatusOld = 0;
volatile char dataIn, dataOut = 'A',  recFlag = 0;
volatile unsigned int head = 0, headTx = 0, tailTx = 0;

unsigned int  stromSoll = 0, stromIst, voltIst, voltSoll = 0, pwmIst, pwmSoll = 0, stell_U = 0;
volatile char b = 0;

#define BUF_SIZE	20
volatile char dataInBuf[BUF_SIZE], dataOutBuf[BUF_SIZE], sendValCharArr[4], lastChar;
#define SUC_I 'A'
			#define SET_I_START 'B'
			#define SET_I_END 'C'	
			#define GET_I_START  'D'
			#define GET_I_END 'E'
			#define ERR_I 'F'
			#define GET_I_SOLL_START 'X'
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
 /*
enum Tokens{
			SUC_I = 'A',
			SET_I_START,		
			SET_I_END = 'C',	
			GET_I_START,		
			GET_I_END,
			ERR_I,		

			SUC_U,
			SET_U_START,		
			SET_U_END,		
			GET_U_START,		
			GET_U_END,
			ERR_U,		

			SUC_PWM,
			SET_PWM_START,	
			SET_PWM_END,	
			GET_PWM_START,	
			GET_PWM_END,		
			ERR_PWM,

			SUC_MODE,
			SET_MODE_START,	
			SET_MODE_END,
			GET_MODE_START,	
			GET_MODE_END,
			ERR_MODE,

			CLK_TOKEN = '?',
			BUSY_TOKEN = '!'
			};
			*/

#define	SET_RES_INACT	0
#define	SET_RES_ACT		1

#define PWM_MODE	'p'
#define CUR_MODE	'i'
#define	VOLT_MODE	'v'
unsigned char mode = 0;

unsigned int asmbldVal = 0;
volatile char curReqMode = 0;

void SetCompVal(unsigned int val);

void ToggleComVal()
{
	static char togVal = 50;

	if(togVal == 50) togVal=70;
	else togVal = 50;

	SetCompVal(togVal);
}

char CheckCifIfNum(char cif);
unsigned int GetAsmbldVal();
char AsmCif(unsigned int firstCif, char amtOfCifs);
char Process_I_End();
unsigned char CatchWrapBW(unsigned int skips);
unsigned char CatchWrapFW(unsigned int skips);
unsigned int ProcessChar(char recChar);
char GetCurReq();
void SetCurReq(char reqMode);
void CopyValArrToBuf(char* valArr, char* txBuf, char amt);


char GetCurReq()
{
	return curReqMode;
}

void SetCurReq(char reqMode)
{
	curReqMode = reqMode;
	
}

unsigned int GetAsmbldVal()
{
	return asmbldVal;
}

char AsmCif(unsigned int firstCif, char amtOfCifs)
{
	unsigned int tmpUint = 0;
	char checkedCif, cycle;
	
	for(cycle = 0; cycle < amtOfCifs; cycle++)
	{
		tmpUint = tmpUint * 10;
		checkedCif = CheckCifIfNum(dataInBuf[firstCif++]);
		if(checkedCif == 0)
		{
			return 0;
		}
		tmpUint += (checkedCif - '0');
	}
	asmbldVal = tmpUint;
	return 1;
}

char CheckCifIfNum(char cif)
{
	if(cif > '9') return 0;
	if(cif < '0') return 0;
	return cif;
}

char  Process_I_End()
{//START-C1-C2-C3-C4-STOP


	if(dataInBuf[CatchWrapBW(6)] != SET_I_START) return 0;
	return AsmCif(CatchWrapBW(5), 4);
}

char  Process_U_End()
{//START-C1-C2-C3-C4-STOP


	if(dataInBuf[CatchWrapBW(6)] != SET_U_START) return 0;
	return AsmCif(CatchWrapBW(5), 4);
}

char  Process_PWM_End()
{//START-C1-C2-C3-C4-STOP


	if(dataInBuf[CatchWrapBW(6)] != SET_PWM_START) return 0;
	return AsmCif(CatchWrapBW(5), 4);
}

char  Process_Mode_End()
{//START-C1-C2-C3-C4-STOP

	if(dataInBuf[CatchWrapBW(6)] != SET_MODE_START) return 0;
	return AsmCif(CatchWrapBW(5), 4);
}

unsigned char CatchWrapBW(unsigned int skips)
{
	if(head >= skips) 
	{
		return head - skips;
	}
	else
	{
		skips = skips - head;
		return BUF_SIZE - skips;
	}
}

unsigned char CatchWrapFW(unsigned int skips)
{
	if((skips+head) >= BUF_SIZE) //-2 -1 0 1 2 3
	{
		return  skips - (BUF_SIZE - head);
	}
	else
	{
		return head + skips;
	}
}

unsigned int ProcessChar(char recChar)
{


	switch(recChar)
	{
		case SET_I_START:{

			}break;

		case SET_I_END:{
			
			if(Process_I_End())
			{
				stromSoll = GetAsmbldVal();
			}
			}break;

		case GET_I_START:{
			SetCurReq(GET_I_START);
			}break;
			

		case GET_I_SOLL_START:{
			SetCurReq(GET_I_SOLL_START);
			}break;

		case GET_I_END:{
			
			}break;
			
		case SET_U_START:{

			}break;

		case SET_U_END:{
			if(Process_U_End())
			{
				voltSoll = GetAsmbldVal();
			}
			}break;

		case GET_U_START:{
			SetCurReq(GET_U_START);
			}break;
			

		case GET_U_SOLL_START:{
			SetCurReq(GET_U_SOLL_START);
			}break;


		case GET_U_END:{

			}break;

		case SET_PWM_START:{

			}break;

		case SET_PWM_END:{
			if(Process_PWM_End())
			{
				pwmSoll = GetAsmbldVal();
			}
			}break;

		case GET_PWM_START:{
				SetCurReq(GET_PWM_START);
			}break;

		case GET_PWM_END:{

			}break;

		case SET_MODE_START:{

			}break;

		case SET_MODE_END:{
			
			if(Process_Mode_End())
			{
				
				mode = GetAsmbldVal();
			}

			}break;

		case GET_MODE_START:{
			SetCurReq(GET_MODE_START);
			}break;

		case GET_MODE_END:{

			}break;
	}

	return 0;
}

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

void SetCompVal(unsigned int val)
{
	TC1H = (((unsigned char)(val >> 8)) & 3);
	OCR1B = (unsigned char)(val & 255);
	pwmIst = val;
}

unsigned int GetBatVoltage()
{
	char tmpC = 0b11010100;
	ADMUX = 0b01000001;
	ADCSRB = 0b00001000;

	ADCSRA = tmpC;
	while((ADCSRA & 16) == 0)
	{
		wdt_reset();
	}

	ADCSRA = tmpC;
	while((ADCSRA & 16) == 0)
	{
	wdt_reset();
	}

	ADCSRA = tmpC;
	while((ADCSRA & 16) == 0)
	{
	wdt_reset();
	}
	return ADC;
}

unsigned int GetBatCurrent()
{
	char tmpC = 0b11010100;
	ADMUX = 0b01001111;
	ADCSRB = 0b01001000;

	ADCSRA = tmpC;
	while((ADCSRA & 16) == 0)
	{
	wdt_reset();
	}

	ADCSRA = tmpC;
	while((ADCSRA & 16) == 0)
	{
	wdt_reset();
	}

	ADCSRA = tmpC;
	while((ADCSRA & 16) == 0)
	{
	wdt_reset();
	}
	
	return ADC;
}

/*
unsigned int GetUsoll()
{
	return ADC;
}

unsigned int GetIsoll()
{
	ADMUX = 0b01000011;
	ADCSRB = 0;
	ADCSRA = 0b11010011;
	while((ADCSRA & 16) == 0)
	{
	}
	return ADC;
}

unsigned int GetMode()
{
	ADMUX = 0b01001001;
	ADCSRB = 0;
	ADCSRA = 0b11010011;
	while((ADCSRA & 16) == 0)
	{
	}
	return ADC;
}
*/

SIGNAL (PCINT_vect)
{
	


	//GIFR |= 0b00100000;
	if((PINB & 0b01000000) == 0b01000000)//jetzt SS IDLE - vorher also activ
	{
		
		ProcessChar(lastChar);
		//SetCompVal(0);
	}
	else
	{
		USISR = USISR & 0b11110000;
		//SetCompVal(100);
	}
}

SIGNAL (USI_OVF_vect)
{
	char dummyChar;
	USISR |= 0b01000000;
	dummyChar = USIDR;
	lastChar = dummyChar;
	//if(dummyChar == 'S')DDRA = 0;
	if(dummyChar == 'S')DDRA = 0b00010000;

	if(dummyChar != '?')
	{
		dataInBuf[head++] = dummyChar;
		if(head >= BUF_SIZE) head = 0;
	}

	if(tailTx != headTx)
	{
		USIDR = dataOutBuf[tailTx++];
		if(tailTx >= BUF_SIZE) tailTx = 0;
	}
}

SIGNAL (ADC_vect)
{
return;
}

int main(void)
{ 

	DDRB = 0b00001010;
	PORTB |= 1;

	USICR = 0b01011000;

	TCCR1A = 0b00010001;
	TCCR1B = 0b00000001;
	TC1H = 1;
	OCR1C = 170;
	PLLCSR = 0b00000010;
	while((PLLCSR & 1) != 1)
		{
		}
	PLLCSR = 0b00000111;
	//PORTB |= 1;
	//SetCompVal(0);

	DIDR0 = 0b01101111;

	/*
	GIMSK = 0b00010000;
	PCMSK1 = 0b01000100;

	sei(); 
	*/
	GIMSK |= 0b00100000;
	PCMSK1 |= 0b01000000;
	SetCompVal(0);

	sei();
if(DDRA == 0)
	{
		DDRA = 0b00010000;
	}
	else
	{
		DDRA = 0;
	}

TCCR0B = 4;

	while(1)
	{


		if(GetCurReq() == GET_I_START)
		{
	
			dataOutBuf[headTx++] = GET_I_START;
			if(headTx >= BUF_SIZE) headTx = 0;

			DisasmblVal(stromIst, &(sendValCharArr[0]), 4);
			CopyValArrToBuf(&(sendValCharArr[0]), &(dataOutBuf[0]), 4);

			dataOutBuf[headTx++] = GET_I_END;
			if(headTx >= BUF_SIZE) headTx = 0;

			SetCurReq(0);
		}

		if(GetCurReq() == GET_U_START)
		{
	
			dataOutBuf[headTx++] = GET_U_START;
			if(headTx >= BUF_SIZE) headTx = 0;

			DisasmblVal(voltIst, &(sendValCharArr[0]), 4);
			CopyValArrToBuf(&(sendValCharArr[0]), &(dataOutBuf[0]), 4);

			dataOutBuf[headTx++] = GET_U_END;
			if(headTx >= BUF_SIZE) headTx = 0;

			SetCurReq(0);
		}

		if(GetCurReq() == GET_MODE_START)
		{
			dataOutBuf[headTx++] = GET_MODE_START;
			if(headTx >= BUF_SIZE) headTx = 0;

			DisasmblVal(mode, sendValCharArr, 4);
			CopyValArrToBuf(sendValCharArr, dataOutBuf, 4);

			dataOutBuf[headTx++] = GET_MODE_END;
			if(headTx >= BUF_SIZE) headTx = 0;

			SetCurReq(0);
		}

		if(GetCurReq() == GET_PWM_START)
		{
			
			dataOutBuf[headTx++] = GET_PWM_START;
			if(headTx >= BUF_SIZE) headTx = 0;

			DisasmblVal(pwmIst, &(sendValCharArr[0]), 4);
			CopyValArrToBuf(&(sendValCharArr[0]), &(dataOutBuf[0]), 4);

			dataOutBuf[headTx++] = GET_PWM_END;
			if(headTx >= BUF_SIZE) headTx = 0;

			SetCurReq(0);
		}

	

		if(GetCurReq() == GET_U_SOLL_START)
		{
	
			dataOutBuf[headTx++] = GET_U_SOLL_START;
			if(headTx >= BUF_SIZE) headTx = 0;

			DisasmblVal(voltSoll, &(sendValCharArr[0]), 4);
			CopyValArrToBuf(&(sendValCharArr[0]), &(dataOutBuf[0]), 4);

			dataOutBuf[headTx++] = GET_U_SOLL_END;
			if(headTx >= BUF_SIZE) headTx = 0;

			SetCurReq(0);
		}

	

		if(GetCurReq() == GET_I_SOLL_START)
		{
	
			dataOutBuf[headTx++] = GET_I_SOLL_START;
			if(headTx >= BUF_SIZE) headTx = 0;

			DisasmblVal(stromSoll, &(sendValCharArr[0]), 4);
			CopyValArrToBuf(&(sendValCharArr[0]), &(dataOutBuf[0]), 4);

			dataOutBuf[headTx++] = GET_I_SOLL_END;
			if(headTx >= BUF_SIZE) headTx = 0;

			SetCurReq(0);
		}
 

	stromIst = GetBatCurrent();
	voltIst = GetBatVoltage();

		

		if(mode == CUR_MODE)
		{	
			if((PINA & 128) == 128)
			{
				SetCompVal(Regulate_I(stromIst, stromSoll));
			}
			else
			{
				SetCompVal(0);
			}
			stell_U = 0;
		}
		else if(mode == VOLT_MODE)
		{	
			if((PINA & 128) == 128)
			{
				if(voltIst > voltSoll)
				{
					if(stell_U > 0)stell_U--;
				}
				if(voltIst < voltSoll)
				{
					if(stell_U < 426)stell_U++;
				}
				SetCompVal(stell_U);
			}
			else
			{
				SetCompVal(0);
			}
			ResetRegler_I();
		}
		else if(mode == PWM_MODE)
		{
			if((PINA & 128) == 128)
			{
				SetCompVal(pwmSoll);
				ResetRegler_I();
				stell_U = 0;
			}
			else
			{
				SetCompVal(0);
			}
		}
		else
		{
			SetCompVal(0);
			ResetRegler_I();
			stell_U = 0;
		}


	};
	

};//main


