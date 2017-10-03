#include <Arduino.h>
#include "Util.h"
#include "LcdHelper.h"


LcdHelper::LcdHelper(DogLcd * lcd){
 this->lcd = lcd;
}

LcdHelper::~LcdHelper(){}

void LcdHelper::printTimeComp(const char* c, const unsigned int t){
  if(t<10){
    lcd->print(c);
  }
  lcd->print(t);
}

void LcdHelper::printTime(const unsigned long seconds){
  unsigned long dsec = seconds % 60;
  unsigned long  dmin = (seconds / 60);
  printTimeComp(" ",dmin);
  lcd->print(":");
  printTimeComp("0",dsec);
}

void LcdHelper::printMpdPlaybackStatus(const char *state){
  lcd->setCursor(0,2);
  if(strcmp(state, "play")==0){
    lcd->write((uint8_t)0);
  }else if(strcmp(state, "stop")==0){
    lcd->write((uint8_t)2);
  }else if(strcmp(state, "pause")==0){
    lcd->write((uint8_t)1);
  }
}

void LcdHelper::printLine(const int lineIndex, const char *text){
  lcd->setCursor(0,lineIndex);
  if(strlen(text)>16){
    char substring[14];
    strncpy(substring, text, 13);
    substring[13] = '\0';
    lcd->print(substring);
    lcd->print("...");
  }else{
    lcd->print(text);
    for(int i=0;i<16-strlen(text);i++){
      lcd->print(" ");
    }
  }
}

void LcdHelper::printProgressBar(const int sec, const int duration){
  int b = sec*1.0/duration*8 + 0.5;
  for(int i=0;i<8;i++){
    if(b>i){
      lcd->write((uint8_t)5);
    }else{
      if(i==0){
        lcd->write((uint8_t)6);
      }else if(i<7){
        lcd->write((uint8_t)3);
      }else{
        lcd->write((uint8_t)4);
      }
    }
  }
}

void LcdHelper::printSong(const char * artist, const char* title, const int sec, const int duration){
  printLine(0, artist);
  printLine(1, title);
  
  lcd->setCursor(2,2);
  printProgressBar(sec, duration);
  
  lcd->setCursor(11,2);
  printTime(sec);
}

void LcdHelper::initLcd(){
  // set up the LCD type and the contrast setting for the display 
  lcd->begin(DOG_LCD_M163, 0x25);
  
  // create characters
  uint8_t play[] = {0x08,0x04|0x08,0x02|0x04|0x08,0x01|0x02|0x04|0x08,0x02|0x04|0x08,0x04|0x08,0x08,0};
  lcd->createChar(0, play);
  uint8_t pause[] = {0,0x1b,0x1b,0x1b,0x1b,0x1b,0x1b,0};
  lcd->createChar(1, pause);
  uint8_t stop[] = {0,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0};
  lcd->createChar(2, stop);
  uint8_t prog_f[] = {0,0,0,0x0e,0x0e,0,0,0};
  lcd->createChar(3, prog_f);
  uint8_t prog_end[] = {0,0x01,0x01,0x0f,0x0f,0x01,0x01,0};
  lcd->createChar(4, prog_end);
  uint8_t prog_p[] = {0,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0};
  lcd->createChar(5, prog_p);
  uint8_t prog_start[] = {0,0x10,0x10,0x1e,0x1e,0x10,0x10,0};
  lcd->createChar(6, prog_start);
  lcd->noDisplay();
  lcd->noCursor();
  lcd->clear(); 
}

void LcdHelper::printQuestion(const String *text){
  lcd->clear();
  lcd->setCursor(0,0);
  lcd->print(*text);
}

void LcdHelper::printAnswer(const String *answer, char c){
    lcd->setCursor(0,2);
    lcd->print(*answer);
    lcd->print(c);
    lcd->setCursor(answer->length(),2);
}


