#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>

/*******************************************************

  This program is used to test the LCD module display and 5 buttons.

********************************************************/

// Select the pin used on LCD
LiquidCrystal lcd(8, 9, 30, 5, 6, 7);
File file;
File copy;

// define the button
int lcd_key     = 0;
int adc_key_in  = 0;

int compteurFichier = 0;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int read_LCD_buttons()
{
  adc_key_in = analogRead(0);          // read analog A0 value
  // when read the 5 key values in the vicinity of the following：0,144,329,504,741
  // By setting different threshold, you can read the one button
  if (adc_key_in > 1000) return btnNONE;
  if (adc_key_in < 50)   return btnRIGHT;
  if (adc_key_in < 250)  return btnUP;
  if (adc_key_in < 450)  return btnDOWN;
  if (adc_key_in < 650)  return btnLEFT;
  if (adc_key_in < 850)  return btnSELECT;

  return btnNONE;
}

void setup()
{
  lcd.begin(16, 2);              // star

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

  file = SD.open("/");
  copy = file.openNextFile();
  compteurFichier++;
  copy = file.openNextFile();
  compteurFichier++;

}

void loop()
{
  lcd_key = read_LCD_buttons();  // read key

  lcd.setCursor(0, 1);           // The cursor moves to the beginning of the second line.
  lcd.print(copy.name());

  if (lcd_key != btnNONE)
  {
    KeyChange(lcd_key);
  }
}

void KeyChange(int key)
{
  lcd.clear();
  lcd.setCursor(0, 0);           // The cursor moves to the beginning of the second line.

  switch (key)               // display key
  {
    case btnRIGHT:
      {
        lcd.print("RIGHT ");
        copy = file.openNextFile();
        compteurFichier++;
        if (!copy) {
          compteurFichier=0;
          
          file.rewindDirectory();
          copy = file.openNextFile();
          compteurFichier++;
          copy = file.openNextFile();
          compteurFichier++;
        }
        delay(700);
        break;
      }
    case btnLEFT:
      {
        int j;
        
        lcd.print("LEFT   ");
        file.rewindDirectory();
        for(j = 0; j<compteurFichier-1;j++)
          copy = file.openNextFile();
        compteurFichier = j;
        delay(700);
        break;
      }
    case btnUP:
      {
        lcd.print("UP    ");
        delay(700);
        break;
      }
    case btnDOWN:
      {
        lcd.print("DOWN  ");
        delay(700);
        break;
      }
    case btnSELECT:
      {
        lcd.print("SELECT");
        delay(700);
        break;
      }
    case btnNONE:
      {
        lcd.print(copy.name());
        break;
      }
  }
}
