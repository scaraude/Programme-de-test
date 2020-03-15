#include <SPI.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h>

#define LED 0  //ligne led (tableau)
#define TDEBUT 1
#define DUREE 2
#define MAIN 3
#define NOTEREC 40

#define BLEU pixels.Color(0,0,255)
#define ROUGE pixels.Color(255,0,0)
#define VERT pixels.Color(0,255,0)
#define TURQUOISE pixels.Color(0,150,150)
#define ORANGE pixels.Color(150,50,0)

#define PIN        22 // On Trinket or Gemma, suggest changing this to 1
#define BRIGHTNESS 100
#define NUMPIXELS 144 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
File myFile;

uint8_t note_rec[4][NOTEREC] = {0};
String balise;
int i_note = 0;
int tempo = 100;
int curseur = 0;
int cmp_mesure;
int tempsDuneMesure = 0;
float sensorValue = 0.006153 * analogRead(A8) - 1.3753;
float utemps = 60000 / (tempo * 4) * sensorValue;
bool changerMusique = false;


void setup() {
  //--------------------INIT SERIAL-----------------
  Serial.begin(9600);
  while (!Serial) {
  }


  //--------------------INIT SD-----------------
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  //--------------------INIT LED-----------------
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setBrightness(BRIGHTNESS);
  pixels.show();   // Send the updated pixel colors to the hardware.
}

void loop() {

  if (changerMusique)
    ChangerDeMusique();
    
  myFile = SD.open("NUVOLE.txt"); //------------------------------LOAD FILE
  
  if (myFile) {
    while (myFile.available()) {
      balise = GetBalise();
      if (balise.substring(0, 7) == "measure") // incrément MESURE
      {
        sensorValue = 0.006153 * analogRead(A8) - 1.3753;
        utemps = 60000 / (tempo * 4) * sensorValue;
        LitMesure();
        //AfficheTableau();
        JoueMesure();
      }
    }
    
    if (myFile)
      myFile.close();
      
  } else {
    Serial.println("error opening test.txt");
  }
}



void JoueMesure()
{
  curseur = 0;
  LedOn();

  unsigned long T_rec = millis();
  bool passed = false;

  while (curseur <= tempsDuneMesure)
  {
    
    if (millis() >= T_rec + utemps * 0.5 && !passed)
    {
      curseur++;
      LedOff();
      passed = true;
    }
    
    if (millis() >= T_rec + utemps)
    {
      LedOn();
      T_rec = millis();
      passed = false;
    }
    
  }
  tempsDuneMesure = 0;
}

void LitMesure()
{
  bool  alter = false;
  cmp_mesure = GetMesure(balise);
  //Serial.println(cmp_mesure);

  curseur = 0;
  i_note = 0;
  memset(note_rec, 0, sizeof(note_rec));

  while (balise != "/measure") // ---------------- Boucle MESURE
  {
    balise = GetBalise();

    if (balise.substring(0, 11) == "sound tempo") {
      tempo = GetTempo(balise);
      utemps = 60000 / (tempo * 4); // POSSIBLEMENT INUTILE à checker si on peut l'enlever
      /* Serial.print("\nNouveau tempo = ");
        Serial.println(tempo);*/
    }

    if (balise.substring(0, 4) == "note") { // --------------- read NOTE

      bool chord = false;

      while (balise != "/note") {

        balise = GetBalise();

        if (balise == "chord/")
          chord = true;
        if (balise == "pitch")
          alter = GetNote();
        if (balise == "duration") {
          note_rec[DUREE][i_note] = GetInt();
          if (chord) {
            note_rec[TDEBUT][i_note] = note_rec[TDEBUT][i_note - 1];
          }
          else {
            note_rec[TDEBUT][i_note] = curseur;
            curseur += note_rec[DUREE][i_note];
          }
          if ((note_rec[TDEBUT][i_note] + note_rec[DUREE][i_note]) > tempsDuneMesure)
            tempsDuneMesure = note_rec[TDEBUT][i_note] + note_rec[DUREE][i_note];
        }
        if (balise == "staff") {
          note_rec[MAIN][i_note] = (char)myFile.read() - '0';
          if (alter)
            note_rec[MAIN][i_note] += 2;
        }
        if (balise == "/note")    //fin NOTE
          i_note++;
      }
    }

    if (balise == "backup") { // --------------- read BACKUP
      while (balise != "duration")
        balise = GetBalise();
      curseur -= GetInt();
    }
  }
}

void LedOff()
{
  for (int i = 0; i <= i_note; i++)
  {
    if (curseur >= (note_rec[TDEBUT][i] + note_rec[DUREE][i]) && note_rec[MAIN][i] != 0)
    {
      pixels.setPixelColor(note_rec[LED][i], 0);
      pixels.show();
      note_rec[MAIN][i] = 0;
    }
  }
}

void LedOn()
{
  for (int i = 0; i <= i_note; i++)
  {
    if (note_rec[TDEBUT][i] == curseur && note_rec[DUREE][i] != 0)
    {
      switch (note_rec[MAIN][i])
      {
        case 1 : pixels.setPixelColor(note_rec[LED][i], BLEU); break;
        case 2 : pixels.setPixelColor(note_rec[LED][i], ROUGE); break;
        case 3 : pixels.setPixelColor(note_rec[LED][i], TURQUOISE); break;
        case 4 : pixels.setPixelColor(note_rec[LED][i], ORANGE); break;
      }
      pixels.show();
    }
  }
  Serial.println();
}

void AfficheTableau()
{
  int i, j;
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < NOTEREC; j++) {
      Serial.print(note_rec[i][j]);
      Serial.print("\t");
    }
    Serial.println();
  }
  Serial.println();
}


String GetBalise()
{
  String balise = "";
  char c = myFile.read();
  if (c == '<')
  {
    c = myFile.read();
    while (c != '>')
    {
      balise += c;
      c = myFile.read();
    }
  }
  return balise;
}


int GetMesure(String balise)
{
  balise.remove( 0, 16);
  for (int i = 0; i < balise.length(); i++) {
    if (balise.charAt(i) == '"')
      balise.remove(i);
  }
  return balise.toInt();
}

int GetTempo(String balise)
{
  balise.remove( 0, 13);
  for (int i = 0; i < balise.length(); i++) {
    if (balise.charAt(i) == '"')
      balise.remove(i);
  }
  return balise.toInt();
}

bool GetNote()
{
  char note[2];
  int8_t alter = 0;
  while (balise != "/pitch")
  {
    balise = GetBalise();
    if (balise == "step")   // read STEP
      note[0] = (char)myFile.read();
    if (balise == "octave")   //read OCTAVE
      note[1] = (char)myFile.read();
    if (balise == "alter")
      alter = GetInt();
  }

  //Serial.println(note);

  //------------------------------------- CONVERTION LED ---------------------------
  if (note[1] == '2')
  {
    switch (note[0]) {
      case 'C': note_rec[LED][i_note] = 1; break;
      case 'D': note_rec[LED][i_note] = 5; break;
      case 'E': note_rec[LED][i_note] = 9; break;
      case 'F': note_rec[LED][i_note] = 11; break;
      case 'G': note_rec[LED][i_note] = 15; break;
      case 'A': note_rec[LED][i_note] = 19; break;
      case 'B': note_rec[LED][i_note] = 22; break;
      default: break;
    }
  }

  if (note[1] == '3')
  {
    switch (note[0]) {
      case 'C': note_rec[LED][i_note] = 25; break;
      case 'D': note_rec[LED][i_note] = 29; break;
      case 'E': note_rec[LED][i_note] = 32; break;
      case 'F': note_rec[LED][i_note] = 34; break;
      case 'G': note_rec[LED][i_note] = 38; break;
      case 'A': note_rec[LED][i_note] = 42; break;
      case 'B': note_rec[LED][i_note] = 46; break;
      default: break;
    }
  }

  if (note[1] == '4')
  {
    switch (note[0]) {
      case 'C': note_rec[LED][i_note] = 48; break;
      case 'D': note_rec[LED][i_note] = 52; break;
      case 'E': note_rec[LED][i_note] = 56; break;
      case 'F': note_rec[LED][i_note] = 58; break;
      case 'G': note_rec[LED][i_note] = 62; break;
      case 'A': note_rec[LED][i_note] = 66; break;
      case 'B': note_rec[LED][i_note] = 69; break;
      default: break;
    }
  }

  if (note[1] == '5')
  {
    switch (note[0]) {
      case 'C': note_rec[LED][i_note] = 71; break;
      case 'D': note_rec[LED][i_note] = 75; break;
      case 'E': note_rec[LED][i_note] = 79; break;
      case 'F': note_rec[LED][i_note] = 81; break;
      case 'G': note_rec[LED][i_note] = 85; break;
      case 'A': note_rec[LED][i_note] = 89; break;
      case 'B': note_rec[LED][i_note] = 93; break;
      default: break;
    }
  }

  if (note[1] == '6')
  {
    switch (note[0]) {
      case 'C': note_rec[LED][i_note] = 95; break;
      case 'D': note_rec[LED][i_note] = 98; break;
      case 'E': note_rec[LED][i_note] = 102; break;
      case 'F': note_rec[LED][i_note] = 105; break;
      case 'G': note_rec[LED][i_note] = 108; break;
      case 'A': note_rec[LED][i_note] = 112; break;
      case 'B': note_rec[LED][i_note] = 116; break;
      default: break;
    }
  }

  if (note[1] == '7')
  {
    switch (note[0]) {
      case 'C': note_rec[LED][i_note] = 119; break;
      default: break;
    }
  }

  //---------------------------------------- ALTERATION ----------------
  if (alter != 0) {
    note_rec[LED][i_note] += 2 * alter;
    return true;
  }
  else
    return false;
}


String GetString()
{
  char c;
  String gstring = "";
  while (c != '<')
  {
    c = (char)myFile.read();
    if (c != '<')
      gstring += c;
  }
  return gstring;
}


int GetInt()
{
  char c;
  String gstring = "";
  while (c != '<')
  {
    c = (char)myFile.read();
    if (c != '<')
      gstring += c;
  }
  return gstring.toInt();
}
