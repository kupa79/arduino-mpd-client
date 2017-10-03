
//#define DEBUG
#ifdef DEBUG
#define debug(msg) Serial.println(msg);
#else
#define debug(msg)
#endif

#ifndef ARDU_MPD_UTIL_H
#define ARDU_MPD_UTIL_H

void setupPin(const int pin, const int mode, const int initialValue);

#endif
