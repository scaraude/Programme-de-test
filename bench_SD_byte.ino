/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

#include <SPI.h>
#include <SD.h>

File myFile;
uint8_t tab = 0;

void setup() {


  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(3)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

}

void loop() {

  static unsigned long curseur = 0;

  // re-open the file for reading:
  myFile = SD.open("MadWorld.txt");
  if (myFile) {

    byte c;
    int i = 0;
    int t = 0;

    unsigned long diff = 0;
    unsigned long somme = 0;
    unsigned long Max = 0;
    unsigned long Min = 100000000;

    // read from the file until there's nothing else in it:
    while (myFile.available()) {

      curseur = micros();

      c = myFile.read();
      tab = c;
      i++;

      diff = micros() - curseur;
      somme = diff + somme;
      Min = min(Min, diff);
      Max = max(Max, diff);
    }

    Serial.print("nombre d'item : ");
    Serial.println(i);
    Serial.print("nombre de note : ");
    Serial.println(t);
    Serial.print("Min : ");
    Serial.println(Min);
    Serial.print("Max : ");
    Serial.println(Max);
    Serial.print("Moyenne : ");
    Serial.println(somme / i);

    // close the file:
    myFile.close();

  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  delay(4000);
  Serial.println();

}
