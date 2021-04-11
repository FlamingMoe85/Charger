#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

volatile unsigned char bitCntr = 0, shftVar = 0, charCntr = 0, charCntr2=0;
volatile unsigned char toggleLed = 0;
volatile unsigned int ledBase = 0;
unsigned char tog = 0;
volatile unsigned char valArr[100];

char uartIn, uartOut, spiIn, spiOut;
#define IDLE		0
#define UART_TO_SPI	1
#define SPI_SENDING	2
#define SPI_READING	3
#define SPI_TO_UART	4
char mode = IDLE;

#define SET_I_START 'B'
      #define SET_I_END 'C' 
      #define GET_I_START  'D'
      #define GET_I_END 'E'

      #define SET_U_START  'H'    
      #define SET_U_END 'I'
      #define GET_U_START  'J'
      #define GET_U_END 'K'

      #define SET_PWM_START 'N'
      #define SET_PWM_END  'O'
      #define GET_PWM_START 'P'
      #define GET_PWM_END  'Q'

      #define SET_MODE_START 'S'
      #define SET_MODE_END 'T'
      #define GET_MODE_START  'U'
      #define GET_MODE_END 'V'

void EnableSlaveSelect()
{

	TCNT1 = 0;
	PORTB &= 0b11111011;
	while(TCNT1 < 4000)
	{

	}
	PORTC = 0;
}

void DisableSlaveSelect()
{

	TCNT1 = 0;
	while(TCNT1 < 4000)
	{

	}
	PORTB |= 0b00000100;
	PORTC = 32;
}

void ReadUart()
{
	if((UCSRA & 128) == 128)
	{
		uartIn = UDR;
		if(uartIn == 'l')EnableSlaveSelect();
		else if(uartIn == 'h')DisableSlaveSelect();
		else mode = UART_TO_SPI;
	}
}

void PutUartToSpi()
{
	mode = SPI_SENDING;
	/*if ((uartIn == 'B')||  (uartIn == 'D')
		|| (uartIn == 'H')|| (uartIn == 'J')
		|| (uartIn == 'N')|| (uartIn == 'P')
		|| (uartIn == 'S')|| (uartIn == 'U'))
	{
		EnableSlaveSelect();
		
	}
	*/
	SPDR = uartIn;
	
}

void ReadSpi()
{

		
	mode = SPI_TO_UART;
	spiIn = SPDR;
	/*
	if ((spiIn == 'C')||(spiIn == 'E')
		|| (spiIn == 'I')|| (spiIn == 'K')
		||  (spiIn == 'O')||(spiIn == 'Q')
		|| (spiIn == 'T')||  (spiIn == 'V'))
	{
		DisableSlaveSelect();
		PORTC = 0;
	}
	*/
}

void PutSpiToUart()
{

	UDR = spiIn;
	mode = IDLE;
}

void UartLoopBackAndLed()
{
char tmpC;
	if((UCSRA & 128) == 128)
	{

		tmpC = UDR;
		UDR = tmpC;
		if(tmpC == 'a')PORTC = 32;
		else if(tmpC == 'b')PORTC = 0;
	}
}


int main(void)
{ 

UBRRH = (unsigned char)(0);
UBRRL = (unsigned char)(50);
/* Enable receiver and transmitter */
UCSRB = 0b00011000;
UCSRC = 0b10000110;
/* Set frame format: 8data, 1stop bit */
DDRD = 0b00000010;
DDRC = 0b00100000;
DDRB = 0b00101100;

SPCR = 0b01010011;
SPSR = 1;

PORTC = 32;

TCCR1B = 1;

while(1)
{

	ReadUart();
	if(mode == UART_TO_SPI)
	{
		PutUartToSpi();
	}
	if(mode == SPI_SENDING)
	{
		if((SPSR & (1<<SPIF)))
		{
			ReadSpi();
		}
	}
	if(mode == SPI_TO_UART)
	{
		PutSpiToUart();
	}
}

}
