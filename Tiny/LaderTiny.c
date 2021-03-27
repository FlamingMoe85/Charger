#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

volatile char slaveSelStatusNew = 0, slaveSelStatusOld = 0;
volatile char dataIn, dataOut = 'A', charCntr = 1, recFlag = 0;

void SetCompVal(unsigned int val)
{
	TC1H = (((unsigned char)(val >> 8)) & 3);
	OCR1B = (unsigned char)(val & 255);
}

unsigned int GetBatVoltage()
{
	ADMUX = 0b01000001;
	ADCSRB = 0b00001000;
	//ADCSRB = 0;
	ADCSRA = 0b1101001;
	while((ADCSRA & 16) == 0)
	{
	}
	return ADC;
}

unsigned int GetBatCurrent()
{
	ADMUX = 0b01001111;
	ADCSRB = 0b01001000;
	ADCSRA = 0b11010011;
	while((ADCSRA & 16) == 0)
	{
	}
	ADCSRA = 0b11010011;
	while((ADCSRA & 16) == 0)
	{
	}
	ADCSRA = 0b11010011;
	while((ADCSRA & 16) == 0)
	{
	}
	return ADC;
}

unsigned int GetUsoll()
{
	ADMUX = 0b01000110;
	ADCSRB = 1;
	ADCSRA = 0b11010011;
	while((ADCSRA & 16) == 0)
	{
	}
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

int main(void)
{ 
unsigned int stromSoll, stromIst, stell = 0;
unsigned int tmpI = 0;

static char dummyChar;
DDRB = 0b00001010;
PORTB |= 1;

USICR = 0b00011000;

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
SetCompVal(0);

DIDR0 = 255;

/*
GIMSK = 0b00010000;
PCMSK1 = 0b01000100;

sei(); 
*/
SetCompVal(0);
while(1)
{


slaveSelStatusNew = (PINB & 0b01000000);// read slave select Pin
if(slaveSelStatusNew != slaveSelStatusOld)//SS edge detect
{
	if(slaveSelStatusOld == 0)//jetzt SS IDLE - vorher also activ
	{
		
		dummyChar = USIBR; //read buffer 
		charCntr++;
		if(charCntr >= 5) charCntr = 0;
		USIDR = 'A' + charCntr; //create fake data to return
	}
	else
	{
		//USIDR = 'a' + charCntr;//Griff nach dem Strohhalm
	}
}
slaveSelStatusOld = slaveSelStatusNew; // part of edge detection


stromSoll = GetIsoll();
stromIst = GetBatCurrent();

 
if(stromIst > stromSoll)
{
	if(stell > 0)stell--;
}
if(stromIst < stromSoll)
{
	if(stell < 426)stell++;
}
SetCompVal(stell);

/*
SetCompVal(GetIsoll());
if(GetBatCurrent() > 20) PORTB |= 1;
else PORTB &= 254;
*/
}

}//main

SIGNAL (PCINT_vect)
{
volatile static char clkNew = 0, clkOld = 0, selectOld = 0, selectNew = 0, bitCntr = 0;  



	GIFR |= 0b00100000;
	clkNew = (PINB & 0b00000100);
	selectNew = (PINB & 0b01000000);

	if((clkNew != clkOld) && (clkNew != 0))//rising edge
	{
		bitCntr++;
	
		dataIn = dataIn << 1;
		if(PINB & 1) dataIn++;
		if(dataOut & 128)
		{
			PORTB |= 2;
		}
		else
		{
			PORTB &= 0b11111101;
		}
		dataOut = dataOut << 1;
	}
	clkOld = clkNew;

	if(selectNew != selectOld)
	{
		
		if(selectNew != 0)
		{
			recFlag = 1;
			if(dataIn == 'a')SetCompVal(100);
			if(dataIn == 'b')SetCompVal(0);
			dataOut = 'A' + charCntr;
			charCntr++;
			if(charCntr == 5) charCntr = 0;
			if(bitCntr == 16)SetCompVal(0);
			bitCntr = 0;  
		}
		else
		{
			bitCntr = 0;  
		}
	}
	selectOld = selectNew;

}
