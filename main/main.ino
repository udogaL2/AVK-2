#define DEBUG 1

#include   "SPI.h"  
#include   "SdFat.h"
#include   <UTFT.h>
#include   <UTFT_DLB.h>
#include   "UTFT_SdRaw.h"
#define    SD_CHIP_SELECT SS

UTFT_DLB   myGLCD(TFT01_24SP, 8, 7, 6, 5, 4);
SdFat      mySD;
UTFT_SdRaw myFiles(&myGLCD);  

extern uint8_t FontRusProp18[];

uint32_t btnTimer = millis();
int BTN_PIN = A0;

void setup() {
  Serial.begin(9600);
  pinMode(BTN_PIN, INPUT);
  pinMode(13, OUTPUT);

  myGLCD.InitLCD();
  myGLCD.clrScr();

  myGLCD.setFont(FontRusProp18);
  
  while (!mySD.begin(SD_CHIP_SELECT)) {}

  while (millis() - btnTimer < 2500) {}
}


void waitForButton(){
  auto l_m = millis();
  while (true){
      if (millis() - l_m > 50){
        auto buttonState = digitalRead(BTN_PIN);
        if (!buttonState) break;
        l_m = millis();
      }
  }
}

void setSeed(){
  uint32_t seed = 0;
  for (int i = 0; i < 16; i++) {
    seed *= 4;
    seed += analogRead(A1) & 3;
    randomSeed(seed);
  }
}

void getAns(){
  setSeed();
  int id = random(0, 6);

  switch (id){
    case 0:
      printTextTwoStr("Узнаете на", "консультации.");
      break;
    case 1:
      printTextOneStr("Это общеизвестно!");
      break;
    default:
      printTextOneStr("Это же очевидно!");
      break;
    }
}

void printTextOneStr(String s1){
  char arr[s1.length()+1];
  s1.toCharArray(arr, s1.length()+1);

  RusPrintStr(myGLCD,arr,20,60,0);
}

void printTextTwoStr(String s1, String s2){
  char arr[s1.length()+1];
  s1.toCharArray(arr, s1.length()+1);

  RusPrintStr(myGLCD,arr,20,60,0);

  delay(200);
  char arr2[s2.length()+1];
  s2.toCharArray(arr2, s2.length()+1);

  RusPrintStr(myGLCD,arr2,20,80,0);
}

void mainFrame(){
  myGLCD.setColor(VGA_BLACK);
  myGLCD.setBackColor(VGA_WHITE);
  
  myFiles.load(0, 0, 320, 240, "avk.raw");

  getAns();
  
  btnTimer = millis();
  while (millis() - btnTimer < 6500) {}
  myGLCD.clrScr();
}

void secretFrame(){
  myFiles.load(0, 0, 320, 240, "ish.raw");
  btnTimer = millis();
  while (millis() - btnTimer < 3500) {}
  myGLCD.clrScr();
}

void loop() {
  auto buttonState = digitalRead(BTN_PIN);
  
  if (buttonState) {
    int clickCount = 1;

    waitForButton();

    btnTimer = millis();

    while(millis() - btnTimer < 400){
      buttonState = digitalRead(BTN_PIN);
      if (buttonState) { 
        btnTimer = millis();
        clickCount++;

        waitForButton();
      }
      delay(10);
    }
    
    if (DEBUG) Serial.println(clickCount); //debug

    if (clickCount == 1){
      mainFrame();
    }
    else if (clickCount == 5){
      secretFrame();
    }
  };
}


//=====================================================================
// перекодировка русских символов и вывод на экран
void RusPrintStr(UTFT_DLB tft,char *st, int x, int y, int deg)
{
char szTextASCII[60];  
     ConvertUniToAscii(st,szTextASCII,30);     
     tft.print(szTextASCII, x, y,deg);
}

//=========================================================================
// возвращает длину получившейся строки в приемние или 0 при ошибке
// iDlPri - максимальное длина буфера
int  ConvertUniToAscii(char szIst[],char szPri[], int iDlPri)
{
int iDl = strlen(szIst);  
int iPr=0,iI;
uint8_t ui8C,ui8C2;

 
    for(iI=0;iI<iDl;iI++)
    { ui8C= szIst[iI]; 
      if(ui8C!=208&&ui8C!=209) {szPri[iPr]=ui8C; iPr++;}
      else
      {  iI++;
         ui8C2=szIst[iI];  
         if(ui8C==208)
         {  
            if(ui8C2>=144&&ui8C2<=191) {ui8C2=ui8C2-16; szPri[iPr]=ui8C2; iPr++;}
            else 
            { if(ui8C2==129) {ui8C2=240; szPri[iPr]=ui8C2; iPr++;}
            }
         }
         if(ui8C==209)
         {  if(ui8C2>=128&&ui8C2<=143||ui8C2==145) {ui8C2=ui8C2+96; szPri[iPr]=ui8C2; iPr++;}
         }
         
      }
      if(iPr>=iDlPri) {szPri[iDlPri-1]=0; return 0;}
   }  
   szPri[iPr]=0;
   return iPr;
}  