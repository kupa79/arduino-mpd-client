#ifndef LCD_HELPER_H
#define LCD_HELPER_H

#include <DogLcd.h>


class LcdHelper{
private:
  DogLcd * lcd;
  void printTimeComp(const char* c, const unsigned int t);
  void printTime(const unsigned long seconds);
  void printLine(const int lineIndex, const char *text);
  void printProgressBar(const int sec, const int duration);


public:
  LcdHelper(DogLcd* lcd);
  ~LcdHelper();
  void printMpdPlaybackStatus(const char *state);
  void printSong(const char *artist, const char *title, const int sec, const int duration);
  void printQuestion(const String *text);
  void printAnswer(const String *answer, char c);
  void initLcd();

};

#endif


