#define DEBUG 0
#define SAFE_SCREEN_TIME 60000

#define SerialP Serial
#define PLAY  0x03
#define VOL   0x08
#define STOP  0x16
byte buff[10] = {0x7E, 0xFF, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF};
byte nomPlay = 1;
bool oneplay = 1;

void mp3_check (uint8_t *buf) {      // КС для плеера
  uint16_t sum = 0;
  for (byte i = 1; i < 7; i++) {
    sum += buf[i];
  }
  sum = -sum;
  *(buf + 7) = (uint8_t)(sum >> 8);
  *(buf + 8) = (uint8_t)sum;
}

void mp3_cmd (byte com, byte atr) {  // команда на плеер
  buff[3] = com;
  buff[6] = atr;
  mp3_check(buff);
  for (byte i = 0; i < 10; i++) {
    SerialP.write(buff[i]);
  }
}

void play (byte np) {     // проигрываем запись номер nm в любой момент
  mp3_cmd(PLAY, np);
  delay(100);
}

void stopPlay () {
  mp3_cmd(STOP, 0);
  delay(100);
}

// дисплей ______________________________
#include   "SPI.h"  
#include   "SdFat.h"
#include   <UTFT.h>
// sck - 13, miso - 12, mosi - 11, ss - 10
#include   "UTFT_SdRaw.h"
#define    SD_CHIP_SELECT SS
#define    dispLED 9

// led - 9, mosi - 8, sck - 7, cs - 6, reset - 5, d/c - 4
UTFT       myGLCD(TFT01_24SP, 8, 7, 6, 5, 4);
SdFat      mySD;
UTFT_SdRaw myFiles(&myGLCD); 
// дисплей ______________________________

// лента ______________________________
#define LED_PIN 3
#define LED_NUM 12
#include "FastLED.h"
CRGB leds[LED_NUM];
byte counter;
CRGB COLORS[] = {CRGB::Red, CRGB::Chartreuse, CRGB::Coral, CRGB::Crimson, CRGB::DeepSkyBlue, 
                  CRGB::GreenYellow, CRGB::Indigo, CRGB::Magenta, CRGB::Lime, CRGB::MediumSeaGreen, CRGB::MediumPurple, 
                  CRGB::Orange, CRGB::PaleGreen, CRGB::PaleVioletRed, CRGB::Plaid, CRGB::Purple, CRGB::SandyBrown, 
                  CRGB::SkyBlue, CRGB::Tomato, CRGB::Yellow};
// лента ______________________________

unsigned long btnTimer = millis();
unsigned long colorTime;
bool safeScreenFlag = false;
int BTN_PIN = A0;

void setup()
{
  SerialP.begin(9600);
  pinMode(BTN_PIN, INPUT);
  pinMode(13, OUTPUT);
  // analogWrite(dispLED, 1023);
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, LED_NUM);

  myGLCD.InitLCD();
  myGLCD.clrScr();

  while (!mySD.begin(SD_CHIP_SELECT)) {}

  myFiles.load(0, 0, 320, 240, "fon.raw");
  FastLED.showColor(COLORS[random(0,20)]);

  while (millis() - btnTimer < 2500) {}
}

void setSeed()
{
  uint32_t seed = 0;
  for (int i = 0; i < 16; i++) {
    seed *= 4;
    seed += analogRead(A1) & 3;
    randomSeed(seed);
  }
}

void ligthEffect1() {
  setSeed();
  colorTime = millis();
  
  CRGB color = COLORS[random(0,20)];

  while(millis() - colorTime < 2500){
    FastLED.clear();
    leds[counter] = color;
    if (++counter >= LED_NUM) counter = 0;
    FastLED.show();
    delay(30);
  }

  FastLED.clear();
  FastLED.show();
}


void ligthEffect2() {
  colorTime = millis();

  while(millis() - colorTime < 2500){
    setSeed();
    FastLED.clear();
    leds[0] = COLORS[random(0,20)];
    leds[1] = COLORS[random(0,20)];
    leds[2] = COLORS[random(0,20)];
    leds[3] = COLORS[random(0,20)];
    leds[4] = COLORS[random(0,20)];
    leds[5] = COLORS[random(0,20)];
    leds[6] = COLORS[random(0,20)];
    leds[7] = COLORS[random(0,20)];
    leds[8] = COLORS[random(0,20)];
    leds[9] = COLORS[random(0,20)];
    leds[10] = COLORS[random(0,20)];
    leds[11] = COLORS[random(0,20)];
    
    FastLED.show();
    delay(60);
  }

  FastLED.clear();
  FastLED.show();
}

void ligthEffect3() {
  colorTime = millis();

  while(millis() - colorTime < 5500){
    for (int i = 0; i < LED_NUM; i++) {
    leds[i].setHue(counter + i * 255 / LED_NUM);
    }
    counter++;        
    FastLED.show();
    delay(30); 
  }

  FastLED.clear();
  FastLED.show();
}

void choiseEffect(){
  setSeed();
  int effect = random(0, 4);

  if (DEBUG) Serial.println(effect);

  switch (effect)
  {
    case 0:
      ligthEffect3();
      break;
    case 1:
      ligthEffect2();
      break;
    default:
      ligthEffect1();
      break;
  }
}

void waitForButton()
{
  auto l_m = millis();
  while (true){
      if (millis() - l_m > 50){
        auto buttonState = digitalRead(BTN_PIN);
        if (!buttonState) break;
        l_m = millis();
      }
  }
}

void printAns()
{
  setSeed();
  int id = random(0, 8);

  switch (id)
  {
    case 0:
      myFiles.load(16, 48, 190, 50, "t1.raw");
      break;
    case 1:
      myFiles.load(16, 48, 190, 50, "t3.raw");
      break;
    default:
      myFiles.load(16, 48, 190, 50, "t2.raw");
      break;
  }
}

void mainFrame()
{
  play(1);
  myFiles.load(0, 0, 320, 240, "avk.raw");

  delay(40);
  setSeed();

  choiseEffect();

  delay(40);

  FastLED.showColor(COLORS[random(0,20)]);  
  
  printAns();

  btnTimer = millis();
  while (millis() - btnTimer < 6500);
  
  stopPlay();
  myFiles.load(0, 0, 320, 240, "fon.raw");
}

void secretFrame()
{
  FastLED.showColor(CRGB::Red);
  myFiles.load(0, 0, 320, 240, "ish.raw");
  btnTimer = millis();
  while (millis() - btnTimer < 3500) {}
  myFiles.load(0, 0, 320, 240, "fon.raw");
}

void loop() 
{
  auto buttonState = digitalRead(BTN_PIN);
  
  if (buttonState && !safeScreenFlag)
  {
    FastLED.showColor(COLORS[random(0,20)]);
    int clickCount = 1;

    waitForButton();

    btnTimer = millis();

    while(millis() - btnTimer < 400)
    {
      buttonState = digitalRead(BTN_PIN);
      if (buttonState) { 
        btnTimer = millis();
        clickCount++;

        waitForButton();
      }
      delay(10);
    }
    
    if (DEBUG) Serial.println(clickCount); //debug

    if (clickCount == 1)
    {
      mainFrame();
    }
    else if (clickCount == 5)
    {
      secretFrame();
    }
  }
  else if (buttonState && safeScreenFlag)
  {
    btnTimer = millis();
    FastLED.showColor(COLORS[random(0,20)]);
    safeScreenFlag = false;
    analogWrite(dispLED, 1023);
    myFiles.load(0, 0, 320, 240, "fon.raw");
  }
  else if(!safeScreenFlag && millis() - btnTimer >= SAFE_SCREEN_TIME)
  {
    FastLED.clear();
    FastLED.show();
    analogWrite(dispLED, 0);
    myGLCD.clrScr();
    safeScreenFlag = true;
  }
}