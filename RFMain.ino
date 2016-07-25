#include <Wire.h>
#include "ArduinoIO.h"
#include "HighLevelProc.h"


//#include "CC1001.h"

char inputBuffer[512];

void onRadioConfig(int radioID, char txrx, int protocolNr, int modulationType,  long frequency,  long bandwidth,  long drate,  long fhub, char changeRadioID, char changeTxrx, char changeProtocolNr, char changeModulationType,  char changeFrequency,  char changeBandwidth,  char changeDrate,   char changeFhub)
{
  
  Serial.println(txrx,DEC); 
  if (changeTxrx)
  {
    
      if (txrx  == radioTX) setIOTX();  
      if (txrx  == radioRX) setIORX(); 
      if (txrx  == radioIDLE) setIOTX();           
  }
/*  ccConfigAll(radioID,  txrx,  protocolNr, modulationType,   frequency,   bandwidth,   drate,   fhub,
 changeRadioID,  changeTxrx,  changeProtocolNr,  changeModulationType,   changeFrequency,   changeBandwidth,   changeDrate,    changeFhub);*/

}


void setup() {

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // put your setup code here, to run once:
 
 initArduinoIO();
 setHighLevelCallback(onSendTTL, onReadSymbol,onRadioConfig);
 setProtocol(protIntertechno);
 updateRadio();
 parseCommandLine(">| R= 36  {1111 1111 f1 f0} \n");
 //parseCommandLine("> P=2 {10f0000000f0}");
}


void loop() {
  while(true)
  {
   // digitalWrite(35,HIGH);  digitalWrite(35,LOW);
    
 //   unsigned int  t = TCNT1;
 //   TCNT1 =0;
  //    decodeTickDeltaTime(t,getRXLevel());
//  decodeTickAbsoluteTime(micros(),getRXLevel());

   char chRead = Serial.read(); 
 
   if (chRead != -1)
  {
   
    /// Serial.write(chRead);
    readLine(chRead, inputBuffer, 200);
    }
   
  }
   
}
