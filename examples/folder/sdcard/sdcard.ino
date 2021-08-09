
/*
  SD card file dump
  
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4
 
*/

/*  Read a file from the SD card using the
  SD library and send it over the serial port.*/
  
#include <SPI.h>
#include <SD.h>

const int chipSelect =4; 

void setup() {
  
Serial.begin(115200);
Serial.print(" start the SD card "); 

if(!SD.begin(chipSelect))   // put your setup code here, to run once:
{
Serial.print("SD card is not present");
return; 
}

Serial.print("Card initalized");

 File dataFile = SD.open("Futronics.txt");

 if(dataFile) {
   while(dataFile.available()){
    Serial.write(dataFile.read());
   }
   dataFile.close();
 }
   else {
    Serial.print("error opening txt file");


 }
 }


void loop() {
  // put your main code here, to run repeatedly:

}
