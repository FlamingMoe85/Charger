#include <SPI.h>


#define SPI2_NSS_PIN PB12 
#define SCK PB13
#define MISO  PB14
#define MOSI  PB15



#define SOFT_SPI
#ifndef SOFT_SPI
SPIClass SPI_2(2);
#endif
#define OPTO_DELAY  1



char SoftSpiTransfer(char txVal)
{
  char retChar = 0;
  for(int i=0; i<8; i++)
  {
    retChar = retChar << 1;
     if((txVal & 128) == 128)digitalWrite(MOSI, HIGH);
     else digitalWrite(MOSI, LOW);
     delay(OPTO_DELAY);
     txVal = txVal << 1;
     
     
     digitalWrite(SCK, HIGH);
     if((digitalRead(MISO)) == HIGH) retChar |= 1;
     
     delay(OPTO_DELAY);
     
     digitalWrite(SCK, LOW);
  }
  return retChar;
}

char sendSPI2(char txVal)
{
  char retChar;
  //delay(50);
  digitalWrite(SPI2_NSS_PIN, LOW); // manually take CSN low for SPI_2 transmission
  delay(10);
#ifdef SOFT_SPI
  retChar = SoftSpiTransfer(txVal);
#else
  retChar = SPI_2.transfer(txVal); //Send the HEX data 0x55 over SPI-2 port and store the received byte to the <data> variable.
#endif
  delay(10);
  digitalWrite(SPI2_NSS_PIN, HIGH); // manually take CSN high between spi transmissions
  delay(10);
  //delay(150);
  return retChar;
}

void SendSpiMsg()
{
  //Serial.print("Send SPI ");
  delay(10);
  digitalWrite(SPI2_NSS_PIN, LOW); // manually take CSN low for SPI_2 transmission
  delay(10);
  
#ifdef SOFT_SPI
  SoftSpiTransfer(dataOutBuf[0]);
#else
  SPI_2.transfer(dataOutBuf[0]);
#endif
  delay(10);
  
#ifdef SOFT_SPI
  SoftSpiTransfer(dataOutBuf[1]);
#else
  SPI_2.transfer(dataOutBuf[1]);
#endif
  delay(10);
  
#ifdef SOFT_SPI
  SoftSpiTransfer(dataOutBuf[2]);
#else
  SPI_2.transfer(dataOutBuf[2]);
#endif
  delay(10);
  
#ifdef SOFT_SPI
  SoftSpiTransfer(dataOutBuf[3]);
#else
  SPI_2.transfer(dataOutBuf[3]);
#endif
  delay(10);
  
#ifdef SOFT_SPI
  SoftSpiTransfer(dataOutBuf[4]);
#else
  SPI_2.transfer(dataOutBuf[4]);
#endif
  delay(10);
  
#ifdef SOFT_SPI
  SoftSpiTransfer(dataOutBuf[5]);
#else
  SPI_2.transfer(dataOutBuf[5]);
#endif

  delay(10);
  digitalWrite(SPI2_NSS_PIN, HIGH); // manually take CSN low for SPI_2 transmission
  delay(10);
}

void SendSingleChar(char val)
{
  
}

