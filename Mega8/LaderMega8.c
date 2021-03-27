#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

unsigned short adcSollV;
unsigned short adcCurrent;
unsigned short slowDown =0;
unsigned short soll = 5, stell = 0;

int main(void)
{ 
DDRB = 0b00000010;


TCCR1A = 0b10000010;//clear on match
TCCR1B = 0b00011001;
ICR1 = 64;
OCR1A = 10;


//ADCSRA |= 0b11000111;//start conversions

TCCR2 = 0b00001111;
OCR2 = 78;

	while(1)
	{
		while((TIFR & 128) != 128);
		TIFR |= 128;

		ADMUX = 0b01000101;//Vcc as Ref, ADC5
		ADCSRA |= 0b11000111;//start conversions
		while((ADCSRA & 16) != 16);
		//adcSollV = ADC;
		adcSollV = (ADC >> 5);

		ADMUX = 0b01000010;//Vcc as Ref, ADC5
		ADCSRA |= 0b11000111;//start conversions
		while((ADCSRA & 16) != 16);
		adcCurrent = ADC;

		OCR1A = adcSollV;
	
	/*
		soll = adcSollV >> 5;
		if(soll > 10) soll -= 10;
		else soll = 0;
 	

 		slowDown++;
		if(slowDown >= 5)
		{
			slowDown = 0;
			if(adcCurrent < soll)
			{
				if(stell < 1024) stell++;
			}
			else if(adcCurrent > soll)
			{
				if(stell > 0) stell--;
			}

			OCR1A = stell;
		}
*/
	}
}
