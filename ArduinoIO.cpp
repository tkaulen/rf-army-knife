/*
Copyright (C) 2016 Thomas Kaulen

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "ArduinoIO.h"
//#include "CC1001.h"
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

#if (defined ARMBoard || defined DefaultBoard)

void initArduinoIO()
{
    vTXIO = txPin; vRXIO = rxPin;
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
  if (deltaTime < 0  )setTXLevel(0); else setTXLevel(1);
  if (ms < 1000000    )
  {
    while (true)
    {
      oldTime = micros();
      if (oldTime - start > ms) break;
    }
  }

}
