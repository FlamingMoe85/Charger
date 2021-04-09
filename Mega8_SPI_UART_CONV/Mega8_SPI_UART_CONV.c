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
	
}

void ReadUart()
{
	if((UCSRA & 128) == 128)
	{
		uartIn = UDR;
		mode = UART_TO_SPI;
	}
}

void PutUartToSpi()
{
	mode = SPI_SENDING;
	EnableSlaveSelect();
	SPDR = uartIn;
	
}

void ReadSpi()
{

		
	mode = SPI_TO_UART;
	DisableSlaveSelect();
	spiIn = SPDR;
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
