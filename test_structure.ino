//--------------------------------STRUCTURE NOTE--------------
typedef struct {
  char led;
  char tdebut;
  char duree;
  char main;
} note;

note stockGauche[60], stockDroit[60];
note *stockP, *stockR;

void setup() {

  //--------------------INIT SERIAL-----------------
  Serial.begin(9600);
  while (!Serial) {
  }

  // put your setup code here, to run once:
  stockGauche[0].led = 62;
  stockGauche[0].tdebut = 63;
  stockGauche[0].duree = 64;
  stockGauche[0].main = 65;

  stockR = stockGauche;

  Serial.println(*stockR);
}

void loop() {
  // put your main code here, to run repeatedly:

}
