#include "ArduinoIO.h"
#include "CC1001.h"
#include "HighLevelProc.h"
#include "config.h"
unsigned short pos;
unsigned long oldTime = 0;
int vTXIO;
int vRXIO;


#ifdef AVRBoard

uint8_t TXPORT;
uint8_t TXBIT;
volatile uint8_t *TXREGISTER;
volatile uint8_t *TXINPUTREGISTER;
uint8_t RXPORT;
uint8_t RXBIT;
volatile uint8_t *RXREGISTER;
volatile uint8_t *RXINPUTREGISTER;

void initArduinoIO()
{
  
  
   pinMode (51, OUTPUT);
  digitalWrite(51, HIGH);
  pinMode (49, OUTPUT);
  digitalWrite(49, LOW);
  pinMode (39, OUTPUT);
  digitalWrite(39, HIGH);

  pinMode (37, OUTPUT);
  digitalWrite(37, LOW);
  
  pinMode (txPin, OUTPUT);
  pinMode (rxPin, INPUT);
  
  
  vTXIO = txPin; vRXIO = rxPin;

  TXPORT = digitalPinToPort(vTXIO);
  TXBIT = digitalPinToBitMask(vTXIO);
  TXREGISTER = portOutputRegister(TXPORT);
  TXINPUTREGISTER = portInputRegister(TXPORT);

  RXPORT = digitalPinToPort(vRXIO);
  RXBIT = digitalPinToBitMask(vRXIO);
  RXREGISTER = portOutputRegister(RXPORT);
  RXINPUTREGISTER = portInputRegister(RXPORT);
}


void setIOTX()
{
  pinMode (vTXIO, OUTPUT);
}

void setIORX()
{
  pinMode(vRXIO, INPUT);
}

void setTXLevel(unsigned char value)
{
  if (value == 0)  *TXREGISTER &= ~TXBIT; else *TXREGISTER |= TXBIT;
}

char getRXLevel()
{
  if (*RXINPUTREGISTER & RXBIT) return 1;
}
#endif

#ifdef ARMBoard

void initArduinoUtils(int vvTXIO, int vvRXIO)
{
  vTXIO = vvTXIO; vRXIO = vvRXIO;
}

void setIOTX()
{
  pinMode (vTXIO, OUTPUT);
}

void setIORX()
{
  pinMode(vRXIO, INPUT);
}




void setTXLevel(unsigned char value)
{
  if (value == 1) digitalWrite(vTXIO, HIGH); else digitalWrite(vTXIO, LOW);
}

char getRXLevel()
{
  return digitalRead(vRXIO);
}

#endif

void onReadSymbol(short symbol)
{

  // Serial.print("Symbol:");
   Serial.print((char)symbol);

}

void onSendTTL(long deltaTime)
{
  
  unsigned long ms = abs(deltaTime);
  unsigned long start = micros();
  unsigned long diffTime = start - oldTime;
  if (diffTime < ms) ms -= diffTime;
  if (deltaTime < 0 )setTXLevel(0); else setTXLevel(1);
  if (ms < 1000000  )
  {
    while (true)
    {
      oldTime = micros();
      if (oldTime - start > ms) break;
    }
  }

}
