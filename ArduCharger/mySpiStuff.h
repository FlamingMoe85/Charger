#include <SPI.h>


#define SPI2_NSS_PIN PB12 
SPIClass SPI_2(2);

char sendSPI2(char txVal)
{
  char retChar;
  delay(50);
  digitalWrite(SPI2_NSS_PIN, LOW); // manually take CSN low for SPI_2 transmission
  delay(50);
  retChar = SPI_2.transfer(txVal); //Send the HEX data 0x55 over SPI-2 port and store the received byte to the <data> variable.
  delay(50);
  digitalWrite(SPI2_NSS_PIN, HIGH); // manually take CSN high between spi transmissions
  delay(50);
  //delay(150);
  return retChar;
}

void SendSpiMsg()
{
  sendSPI2(dataOutBuf[0]);
  sendSPI2(dataOutBuf[1]);
  sendSPI2(dataOutBuf[2]);
  sendSPI2(dataOutBuf[3]);
  sendSPI2(dataOutBuf[4]);
  sendSPI2(dataOutBuf[5]);
}
