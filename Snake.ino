#include "FastLED.h"

#define BREITE 8
#define HOEHE 8

//Wie viele LEDs gibt es insgesammt?
#define NUM_LEDS (BREITE*HOEHE)

// Pin for sending data to the LEDs
#define LED_PIN 6

#define PIN_N 12
#define PIN_O 11
#define PIN_S 10
#define PIN_W 9

int posSchlangeX[NUM_LEDS];
int posSchlangeY[NUM_LEDS];
int laenge = 3;

volatile boolean start;
volatile boolean ende;

char richtung;
volatile char neueRichtung;
volatile char neueRichtung2;

int kruemelX = 0;
int kruemelY = 0;
boolean schlangeAufKruemel;

CRGB led[BREITE][HOEHE];

unsigned int count;

void setup() {
  delay(1000);
  Serial.begin(9600);

  pinMode(PIN_N, INPUT_PULLUP);
  pinMode(PIN_O, INPUT_PULLUP);
  pinMode(PIN_S, INPUT_PULLUP);
  pinMode(PIN_W, INPUT_PULLUP);

  PCattachInterrupt(PIN_N, isrN, FALLING);
  PCattachInterrupt(PIN_O, isrO, FALLING);
  PCattachInterrupt(PIN_S, isrS, FALLING);
  PCattachInterrupt(PIN_W, isrW, FALLING);

  setupLeds();
  FastLED.setBrightness(30);

  reset();
}

void reset()
{
  start = false;
  ende = false;
  
  richtung = '-';
  neueRichtung = '-';
  neueRichtung2 = '-';
  
  schlangeAufKruemel = true;
  
  laenge = 3;

  for(int i = 0; i < NUM_LEDS; i++)
  {
    posSchlangeX[i] = -1;
    posSchlangeY[i] = -1;
  }
  posSchlangeX[0] = 4;
  posSchlangeY[0] = 4;
  //posSchlangeX[1] = 3;
  //posSchlangeY[1] = 4;
  //posSchlangeX[2] = 2;
  //posSchlangeY[2] = 4;
}

void gehen()
{
  for(int i = laenge-1; i>0; i--)
  {
    posSchlangeX[i]=posSchlangeX[i-1];
    posSchlangeY[i]=posSchlangeY[i-1];
  }
  
  if(richtung == 'N')
  {
    posSchlangeY[0] ++;
    if (posSchlangeY[0] >= HOEHE) posSchlangeY[0] = 0;
  }
  if(richtung == 'O')
  {
    posSchlangeX[0] ++;
    if (posSchlangeX[0] >= BREITE) posSchlangeX[0] = 0;
  }
  if(richtung == 'S')
  {
    posSchlangeY[0] --;
    if (posSchlangeY[0] < 0) posSchlangeY[0] = HOEHE-1;
  }
  if(richtung == 'W')
   {
    posSchlangeX[0] --;
    if (posSchlangeX[0] < 0) posSchlangeX[0] = BREITE-1;
  }
}

void loop() {
  count++;

  //Schlange ändert Richtung
  if(neueRichtung != '-')
  {
    if((richtung=='N' && neueRichtung!='S')||(richtung=='S' && neueRichtung!='N')||(richtung=='O' && neueRichtung!='W')||(richtung=='W' && neueRichtung!='O')||richtung=='-')richtung = neueRichtung;
    neueRichtung = neueRichtung2;
    neueRichtung2 = '-';
  }

  //Schlage geht
  if(start && !ende) gehen();
  
  //Schlage frisst Krümel
  if (posSchlangeX[0]==kruemelX && posSchlangeY[0]==kruemelY)
  {
    schlangeAufKruemel = true;
    laenge ++;
  }
  while(schlangeAufKruemel)
  {
    kruemelX = random(0, BREITE);
    kruemelY = random(0, HOEHE);
    schlangeAufKruemel = false;
    
    for(int i=0; i<laenge; i++)
    {
      if (posSchlangeX[i]==kruemelX && posSchlangeY[i]==kruemelY)
      {
        schlangeAufKruemel = true;
      }
    }
  }

  //Schlange beißt sich selbst
  for(int i=1; i<laenge; i++)
  {
    if (posSchlangeX[i]==posSchlangeX[0] && posSchlangeY[i]==posSchlangeY[0])
    {
      ende = true;
    }
  }

  //Alte Schlange Löschen
  for(int x=0; x<BREITE; x++)
  {
    for(int y=0; y<HOEHE; y++)
    {
      led[x][y] = CRGB::Black;
    }
  }
  //Neue Schlange Zeichnen
  for(int i=0; i<NUM_LEDS; i++)
  {
    if(!ende)
    {
      if(posSchlangeX[i]!=-1&&posSchlangeY[i]!=-1) led[posSchlangeX[i]][posSchlangeY[i]] = CRGB::Green;//.setHSV(count%(MAX_BRIGHT+1),MAX_BRIGHT,MAX_BRIGHT);
      led[posSchlangeX[0]][posSchlangeY[0]] = CRGB::Yellow;
    }
    else 
    {
      if(posSchlangeX[i]!=-1&&posSchlangeY[i]!=-1) led[posSchlangeX[i]][posSchlangeY[i]] = CRGB::Red;
      led[posSchlangeX[0]][posSchlangeY[0]] = CRGB::Purple;
    }
  }
  //Krümel zeichnen
  led[kruemelX][kruemelY] = CRGB::White;
  
  ledsAktualisieren();
  
  delay(500);
}

void isrKnopf(char c)
{
  if(neueRichtung == '-') neueRichtung = c;
  else if (neueRichtung2 == '-') neueRichtung2 = c;
  if (ende)
  {
    //Serial.println("Ende -> reset");
    reset();
  }
  else
  {
    //Serial.println("Start -> start = true");
    start = true;
  }
}

void isrN()
{
  isrKnopf('N');  
}
void isrO()
{
  isrKnopf('O');  
}
void isrS()
{
  isrKnopf('S');  
}
void isrW()
{
  isrKnopf('W');
}

