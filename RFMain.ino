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

#include <Wire.h>
#include "ArduinoIO.h"
#include "HighLevelProc.h"


#include "CC1001.h"

char inputBuffer[256];

void onRadioConfig(int radioID, char txrx, int protocolNr, int modulationType,  long frequency,  long bandwidth,  long drate,  long fhub, char changeRadioID, char changeTxrx, char changeProtocolNr, char changeModulationType,  char changeFrequency,  char changeBandwidth,  char changeDrate,   char changeFhub)
{
  
  
  if (changeTxrx)
  {
  //  Serial.println(txrx,DEC); 
      if (txrx  == radioTX) setIOTX();  
      if (txrx  == radioRX) setIORX(); 
      if (txrx  == radioIDLE) setIOTX();           
  }
  
  #ifdef CC1101 
/*  ccConfigAll(radioID,  txrx,  protocolNr, modulationType,   frequency,   bandwidth,   drate,   fhub,
  changeRadioID,  changeTxrx,  changeProtocolNr,  changeModulationType,   changeFrequency,   changeBandwidth,   changeDrate,    changeFhub);*/
  #endif
}


void setup() {

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // put your setup code here, to run once:
 
 ccInit();
 
 initArduinoIO();
 setHighLevelCallback(onSendTTL, onReadSymbol,onRadioConfig);
 
 parseCommandLine(">| P=2 R=3  {1111 1111 f1 f0} \n");
 // parseCommandLine(">| P=1 R= 1 v=ab w=ba  {vaw12} \n");
  
  // parseCommandLine(">| P=1 R= 3  {5vwx3} \n");
  //  parseCommandLine(">| P=1 R= 1 v=a1   {321321} \n");
    // parseCommandLine(">| P=1 R= 1   {321321} \n");
 //      parseCommandLine(">| P=1 R= 1 v=a1   {v} \n");
 // parseCommandLine("> P=2 R= 10 D=1  {1111 1111 f1 f0} \n");LR
  
   // parseCommandLine("> P=2 R= 1 D=1  {} \n");
  
 //  parseCommandLine("> P=1 R= 2 D=0  {12} \n");
  
 //  parseCommandLine("> P=1 R= 36 D=1  {123435} \n");
 //parseCommandLine("> P=2 {10f0000000f0}");
}


void loop() {
  while(true)
  {
   // digitalWrite(35,HIGH);  digitalWrite(35,LOW);
    
 //   unsigned int  t = TCNT1;
 //   TCNT1 =0;
  //    decodeTickDeltaTime(t,getRXLevel());
  decodeTickAbsoluteTime(micros(),getRXLevel());
  
//  Serial.print(getRXLevel(),DEC);
// parseCommandLine("> P=2 R=3  {1111 1111 f1 f0} \n");
   char chRead = Serial.read(); 
 
   if (chRead != -1)
  {
   
    /// Serial.write(chRead);
    readLine(chRead, inputBuffer, 200);
    }
   
  }
   
}
