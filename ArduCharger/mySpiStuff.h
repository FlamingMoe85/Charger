#include <SPI.h>


#define SPI2_NSS_PIN PB12 
SPIClass SPI_2(2);

char sendSPI2(char txVal)
{
  char retChar;
  //delay(50);
  digitalWrite(SPI2_NSS_PIN, LOW); // manually take CSN low for SPI_2 transmission
  delay(10);
  retChar = SPI_2.transfer(txVal); //Send the HEX data 0x55 over SPI-2 port and store the received byte to the <data> variable.
  delay(10);
  digitalWrite(SPI2_NSS_PIN, HIGH); // manually take CSN high between spi transmissions
  delay(10);
  //delay(150);
  return retChar;
}

void SendSpiMsg()
{
  //Serial.print("Send SPI ");
  /*
  sendSPI2(dataOutBuf[0]);
  sendSPI2(dataOutBuf[1]);
  sendSPI2(dataOutBuf[2]);
  sendSPI2(dataOutBuf[3]);
  sendSPI2(dataOutBuf[4]);
  sendSPI2(dataOutBuf[5]);
  */
  delay(10);
  digitalWrite(SPI2_NSS_PIN, LOW); // manually take CSN low for SPI_2 transmission
  delay(10);
/*
Serial.print(dataOutBuf[0]);
Serial.print(dataOutBuf[1]);
Serial.print(dataOutBuf[2]);
Serial.print(dataOutBuf[3]);
Serial.print(dataOutBuf[4]);
Serial.print(dataOutBuf[5]);
*/
/*
Serial.print(' ');
  
  Serial.print((char)SPI_2.transfer(dataOutBuf[0]));
  delay(10);
  Serial.print((char)SPI_2.transfer(dataOutBuf[1]));
  delay(10);
  Serial.print((char)SPI_2.transfer(dataOutBuf[2]));
  delay(10);
  Serial.print((char)SPI_2.transfer(dataOutBuf[3]));
  delay(10);
  Serial.print((char)SPI_2.transfer(dataOutBuf[4]));
  delay(10);
  Serial.print((char)SPI_2.transfer(dataOutBuf[5]));
  delay(10);
  Serial.println(' ');
  Serial.println(' ');
  */
  SPI_2.transfer(dataOutBuf[0]);
  delay(10);
  SPI_2.transfer(dataOutBuf[1]);
  delay(10);
  SPI_2.transfer(dataOutBuf[2]);
  delay(10);
  SPI_2.transfer(dataOutBuf[3]);
  delay(10);
  SPI_2.transfer(dataOutBuf[4]);
  delay(10);
  SPI_2.transfer(dataOutBuf[5]);
  delay(10);
  digitalWrite(SPI2_NSS_PIN, HIGH); // manually take CSN low for SPI_2 transmission
  delay(10);
}

void SendSingleChar(char val)
{
  
}

