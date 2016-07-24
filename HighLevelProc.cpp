/*
* HighLevelProc.c
*
* Created: 23.06.2016 13:22:47
*  Author: thomas
*/

#include "HighLevelProc.h"
//#include "ProtocolImpl.h"
#include "Arduino.h"  // only use for Math functions
#include "config.h"



sendTTL sendTTLDef;
readSymbol writeOut;
struct Symbol symbols[maxSymbol];
struct Sequence sequences[maxSequence];

decodeProtocolCallback decodeProtocol;
encodeProtocolCallback encodeProtocol;

radioConfigCallback radioConfig;

char lastLevel = 0;
unsigned long sumTime = 0;
char a;
char b;
char bpos;
char bufferLock = 0;
char symbolCount = 0;
char sequenceCount = 0;
short lastDValue = 0;
char protocolID = 0;
int repeat=0;

char txrx;
int modulationType;  
long frequency;  
long bandwidth;  
long drate;
long fhub;

char oldtxrx=0;
int oldmodulationType=0;  
long oldfrequency=0;  
long oldbandwidth=0;  
long olddrate=0;
long oldfhub=0;
char oldprotocolID=0;

char state = 0;
char decBuffer[decBufferSize];
char decBufferPos = 0;
char *parseBuffer;
int parseBufferPos;

char debugMode =0;
char tempBuffer[tempBufferSize];
char printDescription=0;

char hasDecoder;
char hasEncoder;
char loopBack =0;

int linePos=0;
long lastTimestamp=0;
unsigned int bitSumer=0;
char bitSumerPos=0;
int encodeStep=0;
char dualDecoderState =0;


/*void onWriteDecodeRecive(char symbol)
{
  readSymbolDef(symbol);
}*/


void resetDualDecoder()
{
 dualDecoderState = 1;  
  
}

char dualDecoder(char symbol,char refSymbol1,char refSumbol2)
{
    
  
  switch (dualDecoderState)
  {
     case 1: 
     if (symbol == refSymbol1)
      {
        dualDecoderState = 2;
        return 0;
      }
      if (symbol == refSumbol2) {
        dualDecoderState = 3;
        return 0;
      } 
      
      
       case 2:
      if (symbol == refSymbol1)
      {
        dualDecoderState = 1;
       // writeBuffer('0');
      
        return 1;
      }

      if (symbol == refSumbol2)
      {
        dualDecoderState = 1;
       
       // writeBuffer('f');
        return 2;
      }
      break;

    case 3:
    if (symbol == refSymbol1)
      {
        dualDecoderState = 1;
       // writeBuffer('1');
        return 3;
      }
    
      if (symbol == refSumbol2)
      {
        dualDecoderState = 1;
       // writeBuffer('1');
        return 4;
      }
      break;
    
  }
  return -1;
  
}

void resetBitSumer()
{
  bitSumer=0;
  bitSumerPos=0;
}

void sumBit(char bitValue)
{
  
  if (bitValue) bitSumer |= 1<<bitSumerPos;
  bitSumerPos++;
}


#ifdef AVRBoard
#define writeDescription(x) SerialPrint_P(PSTR(x))
void SerialPrint_P(PGM_P str) {
  if (printDescription)
  {
    writeDecode('#');writeDecodeInt(protocolID); writeDecode(' ');
    for (uint8_t c; (c = pgm_read_byte(str)); str++) writeDecode(c);
    writeDecode('\n');
  }
}

#define writeConst(x) SerialPrint_P2(PSTR(x))
void SerialPrint_P2(PGM_P str) {
   
    for (uint8_t c; (c = pgm_read_byte(str)); str++) writeDecode(c);
}

#endif


#if (defined ARMBoard || defined DefaultBoard) 
void writeDescription(char *text)
{
   if (printDescription)
   {
   writeDecode('#');writeDecodeInt(protocolID); writeDecode(' ');
    for (int i = 0; i < 200; i++)
    {
      writeDecode(text[i]);
      if (text[i] == 0) break;
    }
    writeDecode('\n');
    }
}

void writeConst(char *text)
{
    for (int i = 0; i < 200; i++)
    {
      writeDecode(text[i]);
      if (text[i] == 0) break;
    }    
}
#endif


int readLine(int readch, char *buffer, int len)
{
 
  if (loopBack)
  {
  writeDecode(readch);
  if (readch == '\r')
  {  
    writeDecode('\n');         
  } 
  }
 parseBuffer = buffer;
  int rpos;
  if (readch > 0) {
    switch (readch) {
      case '\n': // Ignore new-lines
        break;
      case '\r': // Return on CR
        //dec->UnlockBuffer();
        rpos = linePos;
         if (txrx != radioIDLE && linePos ==0) {
   txrx =radioIDLE; updateRadio();
         }
        linePos = 0;  // Reset position index ready for next time
        resetParseBuffer();
        if (rpos != 0) parseCommandLine(parseBuffer);
        
        
        return rpos;
      default:
        //dec->LockBuffer();
        if (linePos < len - 1) {
          parseBuffer[linePos++] = readch;
          parseBuffer[linePos] = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}


void printHelp()
{
 printDescription = 1;
 
 
 
 writeConst("----- Availible Protocols: -----\n");
  writeConst("#0 raw mode\n");
 
 for (int i=0; i< 100; i++)
 {
 setProtocol(i);
 }
 
 writeDecode('\n');

 
  writeConst("----- Examples: -----\n");
    writeConst("1) Send Intertechno(P=2) on 433 mhz repeat 5 with code 10f0000000f0\n");
  writeConst("    > P = 2 F=433000000 R=5  {10f0000000f0}\n");
  writeDecode('\n');
  
  writeConst("2) do the Same after call first example \n");
  writeConst("    > {10f0000000f0}\n");
  writeDecode('\n');
  
   
     writeConst("3) Recive Intertechno on 433 mhz (default frequency)\n");
      writeConst("    < P = 2 \n");
      writeDecode('\n');
      
   writeConst("4) Send Intertechno on 433 mhz and recive after send\n");
      writeConst(" <> P = 2 F=433000000 {10f0000000f0}\n");
      writeDecode('\n');
  
  
 /// writeConst(" > P = 1 1=100:300:200 2=-300:-100:-200 a=12 {12a}\n");
  
  
   writeDecode('\n');


 printDescription = 0; 
  
  
  
}

void printSymbolTable()
{
  
  
  
  writeConst("----- Symbols -----\n");
  
  writeConst("(min:max:duration example:1=100:200:150)\n");

  
 
  
  for (int i = 0; i < symbolCount; i++)
  {
    Symbol *s = &symbols[i];
    writeDecode('[');
    writeDecodeInt(i);
    writeDecode(']');
    writeConst(" min:");
    writeDecodeInt(s->min);
    writeConst(" max:");
    writeDecodeInt(s->max);
    writeConst(" duration:");
    writeDecodeInt(s->duration);
    writeDecode('\n');
  }
  
  writeDecode('\n');
  writeConst("----- Sequences -----\n");
  writeConst("(FirstSecond example:a=12)\n");
 
  
  for (int i = 0; i < sequenceCount; i++)
  {
    Sequence *q = &sequences[i];
    writeDecode('[');
    writeDecode('a'+i);
    writeDecode(']');
    writeConst(" first:");
    writeDecode('0'+ q->a);
    writeConst(" second:");
    writeDecode('0'+ q->b);
    
    writeDecode('\n');
  }
  
  writeDecode('\n');
  writeConst("----- RF Tunings -----\n");
  writeConst("[F] frequency: ");
  writeDecodeInt(frequency); writeConst(" hz");
  writeDecode('\n'); 
  
  writeConst("modulation: ");
  if (modulationType == modAM) writeConst("am");
  if (modulationType == modFM) writeConst("fm");
  writeDecode('\n');
  
  writeConst("bandwidth: ");
  writeDecodeInt(bandwidth); writeConst(" hz");
  writeDecode('\n'); 
  
  writeConst("data rate: ");
  writeDecodeInt(drate); writeConst(" baud");
  writeDecode('\n');
  
  writeConst("frequency hub: "); writeConst(" hz");
  writeDecodeInt(fhub);
  writeDecode('\n');
  
  
  
  writeConst("radio: ");
  if (txrx == radioIDLE) writeConst("idle");
  if (txrx == radioTX) writeConst("tx");
  if (txrx == radioRX) writeConst("rx");
  writeDecode('\n');
  
   writeDecode('\n');
  writeConst("----- other Settings -----\n");
   writeConst("[D] debug mode: ");
  if (debugMode == 0) writeConst("off");
  if (debugMode == 1) writeConst("on");
  writeDecode('\n');
  
  writeConst("[R] repeat: "); 
  writeDecodeInt(repeat);
  writeDecode('\n');
  
    writeConst("[P] protocol: "); 
  writeDecodeInt(protocolID);
  writeDecode('\n');
  
    writeConst("decoder: ");
  if (hasDecoder == 0) writeConst("none");
  if (hasDecoder == 1) writeConst("default");
  if (hasDecoder == 2) writeConst("custom");
  
  writeDecode('\n');
  
   writeConst("encoder: ");
  if (hasEncoder == 0) writeConst("none");
  if (hasEncoder == 1) writeConst("default");
  if (hasEncoder == 2) writeConst("custom");
  
  writeDecode('\n');

  
  
  
  writeDecode('\n');
 

}



void updateRadio()
{
  
 if (oldtxrx != txrx ||  oldprotocolID != protocolID || oldmodulationType != modulationType || oldfrequency != frequency || oldbandwidth != bandwidth || olddrate != drate || oldfhub != fhub)
 { 
  
   radioConfig(0, txrx, protocolID, modulationType, frequency,  bandwidth,  drate,  fhub,0,oldtxrx != txrx,oldprotocolID != protocolID,oldmodulationType != modulationType,oldfrequency != frequency,oldbandwidth != bandwidth,olddrate != drate,oldfhub != fhub);
 }
   
   
  oldtxrx = txrx;  oldprotocolID = protocolID; oldmodulationType = modulationType; oldfrequency = frequency; oldbandwidth = bandwidth; olddrate = drate; oldfhub = fhub; 
 
}

void writeDecode(short symbol)
{
  writeOut(symbol);
}

void writeDecodeInt(long value)
{
  ltoa(value,tempBuffer,10);
 for (int i=0; i< tempBufferSize; i++)
 {
   if (tempBuffer[i] == 0) break;
   writeDecode(tempBuffer[i]);
 } 
  
  
}



void writeEncodeRaw(long time)
{
  if (debugMode == 1)
  {
    decodeStep(time);  
    
    
  }
  
  
  if (time < 0)  sendTTLDef(time);
  if (time > 0)  sendTTLDef(time);
}

void writeEncode(char symbolSrc)
{
  
  char symbol = 0;
  if (symbolSrc >= 48 && symbolSrc <= 57)
  {
    symbol = symbolSrc - 48;
    Symbol *s = &symbols[symbol];
    if (s->duration != 0) writeEncodeRaw(s->duration);
  }
  if (symbolSrc >= 'a' && symbolSrc <= 'f')
  {
    symbol = symbolSrc - 'a' + 10;
    Sequence *seq = &sequences[symbol - 10];
    if (seq->a != 0 && seq->b != 0)
    {

      Symbol *s1 = &symbols[seq->a];
      writeEncodeRaw(s1->duration);
      Symbol *s2 = &symbols[seq->b];
      writeEncodeRaw(s2->duration);
    }
  }
}

void decodeSymbol(char symbol,long value)
{
  if (symbol <= 9) decodeProtocol(symbol + 48,value, protocolID); else decodeProtocol(symbol + 'a' - 10,value, protocolID);
}

char encodeSymbol(char symbol)
{
  return encodeProtocol(symbol, protocolID);
}

char desequenceStep(char symbol,short value,char sequenceStartPoint)
{
  switch (bpos)
  {
    case 2:
      decodeSymbol(a,value);
      a = b;
      b = symbol;
      bpos = 2;
      break;

    case 1:
      a = b;
      b = symbol;
      bpos = 2;
      break;

    case 0:
      b = symbol;
      a = -1;
      bpos = 1;
      break;
  }
  char matchA = 0;
  for (int i = sequenceStartPoint; i < sequenceCount; i++)
  {
    Sequence *s = &sequences[i];

    if (s->a == symbol)
    {
      matchA = 1;
    }

    if (bpos == 2 && s->a == a && s->b == b)
    {
      decodeSymbol(10 + i,value);
      bpos = 0;
      return 0;
    }
  }
  if (matchA == 0)
  {

    if (bpos == 1)
    {
      decodeSymbol(symbol,value);
      bpos = 0;
      return 0;
    }
    if (bpos == 2)
    {
      decodeSymbol(a,value); decodeSymbol(b,value);
      bpos = 0;
      return 0;
    }
  }

}




void decodeStep(long DValue)
{
  
  short DValueDiv = DValue;
  if (DValue >= 32760) DValueDiv = 32760;
  if (DValue <= -32760) DValueDiv = -32760;
  
  if (protocolID == 0)
  {
    writeDecodeInt(DValue); writeDecode(' ');
  } else
  {

   // char DSign = 0;
   // if (DValueDiv > 0) DSign = 1;
    
    for (int i = 0; i < symbolCount; i++)
    {
      Symbol *s = &symbols[i];
      //Serial.println(s->min);

      if ( DValueDiv > s->min && DValueDiv < s->max)
      {
        desequenceStep(i,DValue,s->sequenceStartPoint);

        break;
      }
    }
  }
  
}

void decodeTickDeltaTime(unsigned long  deltaTime, char level)
{
  if (txrx == radioRX)
  {
  if (lastLevel != level)
  {
    decodeStep(lastLevel * sumTime);
    sumTime = 0;
    lastLevel = level;
  }
  sumTime += deltaTime ;
  }
}

void decodeTickAbsoluteTime(unsigned long  absoluteTime, char level)
{
  if (txrx == radioRX)
  {
  if (lastLevel != level)
  {
    decodeStep(lastLevel * sumTime);
    sumTime = 0;
    lastLevel = level;
  }
  sumTime += (absoluteTime - lastTimestamp) ;
  }
  
  lastTimestamp = absoluteTime;
}



void setHighLevelCallback(sendTTL func1, readSymbol func2,radioConfigCallback func3)
{
  writeOut = func2;
  sendTTLDef = func1;
  radioConfig = func3;
  initHighLevel();
}

void sendSymbolString(char *symbolStr, int repeat)
{
 // radioConfig(-1, 1, -1, -1,  -1,  -1,  -1,  -1);
  for (int j = 0; j < repeat; j++)
  {
    for (int i = 0; i < 200; i++)
    {
      sendSymbol(symbolStr[i]);
      if (symbolStr[i] == 0) break;
    }
  }
   //radioConfig(-1, 0, -1, -1,  -1,  -1,  -1,  -1);
}


void beginRecive()
{
  
      txrx = radioRX;
      updateRadio();  
  
  
}
void beginTransmit()
{
  
      txrx = radioTX;
      updateRadio();  

  
}

void endTransmitRecive()
{
  
      txrx = radioIDLE;
      updateRadio();  

  
}

void sendSymbol(short symbolSrc)
{
  if (symbolSrc == '{')
  {
   
  
     encodeStep =0;
   }
  if (protocolID == 0)
  {
    sendTTLDef(symbolSrc);
  } else
  {
    char symbol = encodeSymbol(symbolSrc);
  }
  encodeStep++;
}


void initHighLevel()
{

  resetHighLevel();
  oldtxrx = txrx;  oldprotocolID = protocolID; oldmodulationType = modulationType; oldfrequency = frequency; oldbandwidth = bandwidth; olddrate = drate; oldfhub = fhub;   
  radioConfig(0, 0, 0, 0, 0,  0,  0,  0,1,1,1,1,1,1,1,1);
 
}
void resetHighLevel()
{
  txrx=radioIDLE;
 modulationType=modAM;  
frequency=0;  
bandwidth=0; 
drate=0;
 fhub=0;
 protocolID =0;
 symbolCount =0;
 sequenceCount =0;
 repeat =1;
 debugMode =0;
 linePos =0;
  
  
  state =0;
  decBufferPos = 0;
  
  sumTime = 0;
  lastLevel = 0;
 // setImplCallback(onWriteEncodeRecive, onWriteDecodeRecive, setSymbol, setSequence); :ToDo

  for (int i = 0; i < maxSymbol; i++)
  {
    struct Symbol *s = &symbols[i];
    s->min = 0; s->max = 0; s->duration =0;
    s->sequenceStartPoint =0;
  }

  for (int i = 0; i < maxSequence; i++)
  {
    struct Sequence *s = &sequences[i];
    s->a = 0; s->b = 0;
  }
}

void prepare()
{
  symbolCount = 0;
  for (int i = 0; i < maxSymbol; i++)
  {
    Symbol *s = &symbols[i];
    if (s->max != 0 && s->min != 0)
    {
      symbolCount = i + 1;
    }
  }

  sequenceCount = 0;
  for (int i = 0; i < maxSequence; i++)
  {
    Sequence *s = &sequences[i];
    if (s->a != 0 && s->b != 0)
    {
      sequenceCount = i + 1;
    }
  }
}

void setSequence(unsigned char nr, char symbolA, char symbolB)
{
  char nrLocal = nr - 'a';
  Sequence *s = &sequences[nrLocal];
  s->a = symbolA; s->b = symbolB;
  prepare();
}

void setSymbolRange(unsigned char nr, short minTime, short maxTime, short duration)
{
  short minTimeDiv = minTime;
  short maxTimeDiv = maxTime;
  short durationDiv = duration;
  char nrLocal = nr - 48;

  Symbol *s = &symbols[nrLocal];
  s->duration = durationDiv;

  if (minTime < 0)
  {
    s->min = minTimeDiv; s->max = maxTimeDiv;
  } else

  {
    s->min = minTimeDiv; s->max = maxTimeDiv;
  }
  prepare();
}

void setSymbolMargin(unsigned char nr, short duration, float margin)
{
  short marginLocal = abs(duration * margin);
  setSymbolRange(nr, duration - marginLocal, duration + marginLocal, duration);
}

void setSymbolFullRange(unsigned char nr,short duration)
{
  setSymbolRange(nr,-32762,32762,duration);
}

void setSymbolFullRangeHigh(unsigned char nr,short duration)
{
  setSymbolRange(nr,1,32762,duration);
}

void setSymbolFullRangeLOW(unsigned char nr,short duration)
{
  setSymbolRange(nr,-32762,-1,duration);
}

void resetBuffer()
{
  decBufferPos = 0;
}

void writeBuffer(char symbol)
{
  if (decBufferPos < decBufferSize)
  {
    decBuffer[decBufferPos] = symbol;
    decBufferPos++;
  }
}

char readBuffer()
{
  if (decBufferPos < decBufferSize)
  {
char r = decBuffer[decBufferPos];
decBufferPos++;
return r;
}
 return 0;
  
}

void flushDecodeBuffer()
{
  writeDecode('{');
  for (int i = 0; i < decBufferPos; i++)
  {
    writeDecode(decBuffer[i]);
  }
  writeDecode('}');
  writeDecode('\n');
}

void flushEncodeBuffer()
{
  for (int i = 0; i < decBufferPos; i++)
  {
    writeEncode(decBuffer[i]);
  }
}







void writeBufferText(char *text)
{
  
    for (int i = 0; i < 200; i++)
    {
      writeBuffer(text[i]);
      if (text[i] == 0) break;
    }
 
  
  
}






/////////////////Parser/////////////////

void parseError()
{
  
  
}

void resetParseBuffer()
{
  parseBufferPos =0;
  
}

char az()
{
  return parseBuffer[parseBufferPos];
  
  
}

char next()
{
  if (az() == 0) return 0;
  parseBufferPos++;
  return az();
  
}

void skipBlank()
{
  while (az() ==' ') next();
}

char nextIgnoreBlank()
{
 if (az() ==' ') 
 {
   skipBlank();
   return az();  
 }
   next();
  skipBlank();
  return az();  
  
}


long parseNumber()
{
  char sign = 1;
  if (az() == '-')
  {
    nextIgnoreBlank();  
    sign = -1;
  }
  

  long number=0;
  while (az() >= 48 && az() <= 57)
  {
    number+= (az()-48);
    next();
    if (az() >= 48 && az() <= 57) number *= 10;
  }
  skipBlank();  
    
    
  return number*sign;
  
}

void parseData()
{
    int startPos = parseBufferPos;
 if (az() == '{')
 {
  
  for (int i=0; i<repeat; i++)
  {
    parseBufferPos = startPos;
  if (az() == '{')
  {
  
   sendSymbol(az()); 
   next();
 
   while (true)
   {
     char az = parseBuffer[parseBufferPos];
     sendSymbol(az);
     if (az == '}') break;
     parseBufferPos++;
   }   
   sendSymbol(az()); 
   nextIgnoreBlank();
   
   }
   }
   
   }
   
  
  
  
}

void parseSymbol(char symbol)
{
  char left = az();
  nextIgnoreBlank();
  char right = az();
  nextIgnoreBlank();
  
 
  
  
}

void parseRange(char symbol)
{
  short left = parseNumber();
  if (az() == ':') nextIgnoreBlank();
   short right = parseNumber();
  if (az() == ':') nextIgnoreBlank();
  short center = parseNumber();
  
}

 // > T=433000000 M=am 1=200:220:200 2=700:800:850 3=100:200:150 5=-2000:-1000:-1500 A=ab B=cd {FFF11100011F0010F}50
 // > T=433000000 M=am 1=200:220:200 2=700:800:850 4=100:200:150 5=-2000:-1000:-1500 A=ab B=cd 
char parseCommandLine(char *in)
{
   
   
  char tempTXRX =0;
   
  parseBuffer= in;
  resetParseBuffer();
  
  if (az() == 'H')
  {
      printHelp(); 
      return 0;
  }
  
  if (az() == 'L')
  {
      printSymbolTable();
      return 0;
  }
  
  
  tempTXRX  = radioIDLE; char autoRX = 0; 
  skipBlank();
  
  if (az() == '<')
  {
    tempTXRX  = radioRX;
    nextIgnoreBlank();
   
  }
  
  if (az() == '>')
  {
      
    if (tempTXRX  == radioRX) autoRX = 1;
   

      tempTXRX  = radioTX;    
   
    nextIgnoreBlank();
   
  }
  

  char command =0;
  skipBlank();
    
  while((az() >='a' && az() <='z') || (az() >='A' && az() <='Z') || (az() >= 48 && az() <= 57))
  {
  //  Serial.print(az());
    command = az(); nextIgnoreBlank();
    
    if (az() == '=') 
    {
      nextIgnoreBlank();
    }
    switch(command)
    {
      case 'F':
      	 
       frequency = parseNumber(); 
      // writeDecodeInt(frequency);
       //Serial.print(az());
      break;
      
      case 'P':
      
       setProtocol(parseNumber()); 
     //  Serial.print(az());
       
      break;
      
      case 'R':
        repeat = parseNumber();
        break;
        
       case 'D':
        debugMode = parseNumber();
        break;
      
      case 'a': 
      case 'b':
      case 'c': 
      case 'd':
      case 'e': 
      case 'f':
       parseRange(command);
      
      break;
      
      case '0': 
      case '1': 
      case '2':
      case '3': 
      case '4':
      case '5': 
      case '6':
      case '7':
      case '8':
      case '9':
      parseSymbol(command);
      
      break;
      
      default: break;
          
    }
 
  }
  txrx = tempTXRX ;
  updateRadio();

  parseData();
  
  if (tempTXRX !=0)
  {
  if (autoRX == 1)
  {
     txrx = radioRX;  
     updateRadio();
    
  } else
  {
    if (txrx == radioTX)
    {
      txrx =radioIDLE;
      updateRadio();
    }
       
  }
  } else
  {
    txrx =radioIDLE;
      updateRadio();

  }
  
  return 0;

}






#include "HighLevelProtocols.h"

void setProtocol(char protID)
{
  resetHighLevel();
  protocolID = protID;
  onSetProtocol(protID);
  prepare();
  
  decBufferPos =0;
  
  
  
  if (decodeProtocol == 0)
  {
    hasDecoder = 0;
  } else
  if (decodeProtocol == decodeDefault)
  {
    hasDecoder = 1;   
  } else
  {
     hasDecoder = 2;   
  }
  
   if (encodeProtocol == 0)
  {
    hasEncoder = 0;
  } else
  if (encodeProtocol == encodeDefault)
  {
    hasEncoder = 1;   
  } else
  {
     hasEncoder = 2;   
  }
  
  
  
}








