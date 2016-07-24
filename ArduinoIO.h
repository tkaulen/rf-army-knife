#ifndef ArduinoIO_H_
#define ArduinoIO_H_
#include "Arduino.h"

#define rxPin 35
#define txPin 35


void initArduinoIO();
void onSendTTL(long deltaTime);
void onReadSymbol(short symbol);
void setTXLevel(unsigned char value);
char getRXLevel();
void setIOTX();
void setIORX();


#endif
