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

#include "HighLevelProc.h"
#include "Config.h"
#include "ConstWriter.h"
#include "HighLevelProtocols.h"

sendTTL sendTTLDef;
readSymbol writeOut;
//struct Symbol symbols[maxSymbol];
//struct Sequence sequences[maxSequence];

encodeProtocolCallback encodeProtocol;
radioConfigCallback radioConfig;

//char a;
//char b;
//char state = 0;
//char decBuffer[decBufferSize];
//char decBufferPos = 0;
char hasDecoder;
char hasEncoder;


struct Enviroment enviroments[maxEnviroments];
struct Enviroment *en;



int encodeStep = 0;
char lastLevel = 0;
unsigned long sumTime = 0;


char bufferLock = 0;

short lastDValue = 0;

int repeat = 0;

char txrx;
int modulationType;
long frequency;
long bandwidth;
long drate;
long fhub;
char oldtxrx = 0;
int oldmodulationType = 0;
long oldfrequency = 0;
long oldbandwidth = 0;
long olddrate = 0;
long oldfhub = 0;
char oldprotocolID = 0;

char *parseBuffer;
int parseBufferPos;
char debugMode = 0;
char tempBuffer[tempBufferSize];
char printDescription = 0;

char loopBack = 0;
int linePos = 0;
long lastTimestamp = 0;



char rawRecorderState = 0;
short rawRecorderBuffer[rawRecorderSize];
unsigned short rawRecorderPos;
char scanMode = 0;
char paralellTaskCount = 0;
char writeRawToConsole = 0;



int getTickCounter()
{
  return en->tickCounter;

}

void resetTickCounter()
{
  en->tickCounter = 0;
}

void debugOut(long value)
{
  Serial.print('<');
  Serial.print(value, DEC);
  Serial.print('>');


}

char getScanMode()
{
  return scanMode;

}

void resetRawRecorder()
{
  rawRecorderPos = 0;
}

void putRawRecorder(short value)
{
  if (rawRecorderPos < rawRecorderSize)
  {
    rawRecorderBuffer[rawRecorderPos] = value;
    rawRecorderPos++;
  }
}

void printRawRecorder()
{
  writeConst("----- raw recorder content: -----\n");
  int lineCounter = 0; int lineNr = 1;
  writeConst("[0] ");
  for (int i = 0; i < rawRecorderPos; i++)
  {
    writeDecodeInt(rawRecorderBuffer[i]);
    char foundSymbol = findSymbol(rawRecorderBuffer[i]);
    if (foundSymbol != -1)
    {
      writeDecode('{');
      writeDecode(foundSymbol);
      writeDecode('}');
    }
    lineCounter ++;
    if (lineCounter <= 9) writeDecode(',');
    if (lineCounter >= 10)
    {
      writeDecode('\n');
      writeDecode('[');
      writeDecodeInt(lineNr * 10);
      writeDecode(']');
      writeDecode(' ');

      lineCounter = 0;
      lineNr++;
    }
  }

  writeDecode('\n');
  writeDecode('\n');
  writeDecode('\n');
}

char isPrintDescription()
{
  return printDescription;
}

void resetDualDecoder()
{
  en->dualDecoderState = 1;
}

char dualDecoder(char symbol, char refSymbol1, char refSumbol2)
{
  switch (en->dualDecoderState)
  {
    case 1:
      if (symbol == refSymbol1)
      {
        en->dualDecoderState = 2;
        return 0;
      }
      if (symbol == refSumbol2) {
        en->dualDecoderState = 3;
        return 0;
      }
    case 2:
      if (symbol == refSymbol1)
      {
        en->dualDecoderState = 1;
        return 1;
      }

      if (symbol == refSumbol2)
      {
        en->dualDecoderState = 1;
        return 2;
      }
      break;

    case 3:
      if (symbol == refSymbol1)
      {
        en->dualDecoderState = 1;
        return 3;
      }
      if (symbol == refSumbol2)
      {
        en->dualDecoderState = 1;
        return 4;
      }
      break;
  }
  return -1;
}

void resetBitSumer()
{
  en->bitSumer = 0;
  en->bitSumerPos = 0;
}

void sumBit(char bitValue)
{
  if (bitValue) en->bitSumer |= 1 << en->bitSumerPos;
  en->bitSumerPos++;
}


int readLine(int readch, char *buffer, int len)
{
  if (readch >= 10) {
     parseBuffer = buffer;

    
    if ((linePos < len - 1) && (readch != '\n' && readch != '\r'))
    {
      parseBuffer[linePos] = readch;
      linePos++;
     
    }

    if (readch == '\n' || readch == '\r')
    {
      
      
      if (linePos != 0)
      {
        txrx = radioIDLE; rawRecorderState = 0; updateRadio();
      //  Serial.println("out");
        parseBuffer[linePos] = 0; linePos++;
        
        Serial.println(parseBuffer);
        resetParseBuffer();
        parseCommandLine(parseBuffer);
      }
      linePos = 0;
    }

  }
  return -1;


}

int readLine2(int readch, char *buffer, int len)
{
  if (loopBack)
  {
    writeDecode(readch);
    if (readch == '\r')
    {
      writeDecode('\n');
    }
  }
  Serial.println(readch, DEC);
  int rpos;
  if (readch > 0) {
    if (txrx != radioIDLE && linePos == 0) {
      //   txrx = radioIDLE; rawRecorderState = 0; updateRadio();
    }

    switch (readch) {
      case '\n': // Ignore new-lines
      //   break;
      case '\r': // Return on CR

        rpos = linePos;

        linePos = 0;  // Reset position index ready for next time

        if (rpos != 0)
        {
          txrx = radioIDLE; rawRecorderState = 0; updateRadio();
          parseBuffer = buffer;
          resetParseBuffer();
          parseCommandLine(parseBuffer);

        }
        return rpos;
      default:

        if (linePos < len - 1) {
          parseBuffer[linePos++] = readch;
          parseBuffer[linePos] = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}


void writeNewLine()
{
  writeDecode('\n');
  writeDecode('\r');

}

void printProtocols()
{
  char tempProtocol = en->protocolID;
  printDescription = 1;
  writeConst("1) ----- Protocols -----\n");
  writeConst("#0 raw mode\n");
  for (int i = 0; i < 100; i++)
  {
    setProtocol(i);
  }


  en->protocolID = tempProtocol;
  printDescription = 0;
  writeConst("[P] selected protocol: ");
  writeDecodeInt(en->protocolID);
  writeDecode('\n');
  writeDecode('\n');
}
void printHelp()
{


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
  writeDecode('\n');
  writeDecode('\n');

}

void printSymbolTable()
{
  printProtocols();

  writeConst("2a) ----- Timing symbols -----\n");
  writeConst("(0123456789=duration:min:max or duration:tollerance example:1=150:100:200,1=150:50)\n");
  for (int i = 0; i < en->symbolCount; i++)
  {
    Symbol *s = &en->symbols[i];

    writeDecode('[');
    writeDecodeInt(i);
    writeDecode(']');
    if (s->duration == 0)
    {
      writeDecode(' ');
      writeDecode('N');

    } else
    {
      writeConst(" duration:");
      writeDecodeInt(s->duration);
      writeConst(" min:");
      writeDecodeInt(s->min);
      writeConst(" max:");
      writeDecodeInt(s->max);
    }
    writeDecode('\n');
  }

  writeDecode('\n');
  writeConst("2b) ----- Dual sequences of timingsymbols-----\n");
  writeConst("(abcdef=FirstSecond example:a=12)\n");

  for (int i = 0; i < en->sequenceCount; i++)
  {
    Sequence *q = &en->sequences[i];
    writeDecode('[');
    writeDecode(q->name);
    writeDecode(']');
    writeConst(" first:");
    writeDecode( q->a);
    writeConst(" second:");
    writeDecode(q->b);

    writeDecode('\n');

    // debugOut(q->a);    debugOut(q->b);   debugOut(q->c);   debugOut(q->d);
  }

  writeDecode('\n');
  writeConst("2c) ----- Dual sub sequences -----\n");
  writeConst("(vwxyz=FirstSecond example:v=a1)\n");

  for (int i = 0; i < en->subSequenceCount; i++)
  {
    SubSequence *q = &en->subSequences[i];
    writeDecode('[');
    writeDecode(q->name);
    writeDecode(']');
    writeConst(" first:");
    writeDecode( q->a);
    writeConst(" second:");
    writeDecode(q->b);

    writeDecode('\n');

    // debugOut(q->a);    debugOut(q->b);   debugOut(q->c);   debugOut(q->d);
  }

  writeDecode('\n');

  writeConst("2d) ----- Symbol Commands -----\n");
  writeConst("[s] Start Symbol: ");
  writeDecode(en->startSymbol);
  writeDecode('\n');

  writeConst("[t] Terminal Symbol: ");
  writeDecode(en->endSymbol);
  writeDecode('\n');

  writeConst("[i] Init Symbol: ");
  writeDecode(en->initSymbol);
  writeDecode('\n');

  writeDecode('\n');
  writeConst("3) ----- RF Tunings -----\n");

  writeConst("[F] frequency: ");
  writeDecodeInt(frequency); writeConst(" hz");
  writeDecode('\n');

  writeConst("[M] modulation(am,fm): ");
  if (modulationType == modAM) writeConst("am");
  if (modulationType == modFM) writeConst("fm");
  writeDecode('\n');

  writeConst("[B] bandwidth: ");
  writeDecodeInt(bandwidth); writeConst(" hz");
  writeDecode('\n');

  writeConst("[A] data rate: ");
  writeDecodeInt(drate); writeConst(" baud");
  writeDecode('\n');

  writeConst("[Q] frequency hub: ");
  writeDecodeInt(fhub);
  writeConst(" hz");
  writeDecode('\n');

  writeConst("    radio: ");
  if (txrx == radioIDLE) writeConst("idle");
  if (txrx == radioTX) writeConst("tx");
  if (txrx == radioRX) writeConst("rx");
  writeDecode('\n');
  writeDecode('\n');


  writeConst("4) ----- other Settings -----\n");
  writeConst("[D] debug mode(on,off): ");
  if (debugMode == 0) writeConst("off");
  if (debugMode == 1) writeConst("on");
  writeDecode('\n');

  writeConst("[R] repeat: ");
  writeDecodeInt(repeat);
  writeDecode('\n');



  writeConst("[U] uart date rate: ");
  writeDecodeInt(9600); writeConst(" baud");
  writeDecode('\n');

  writeConst("    decoder: ");
  if (hasDecoder == 0) writeConst("none");
  if (hasDecoder == 1) writeConst("default");
  if (hasDecoder == 2) writeConst("custom");
  writeDecode('\n');

  writeConst("    encoder: ");
  if (hasEncoder == 0) writeConst("none");
  if (hasEncoder == 1) writeConst("default");
  if (hasEncoder == 2) writeConst("custom");
  writeDecode('\n');

  writeConst("    raw recorder: ");
  if (rawRecorderState == 0) writeConst("stop");
  if (rawRecorderState == 1) writeConst("record");
  if (rawRecorderState == 2) writeConst("2");
  if (rawRecorderState == 3) writeConst("3");
  if (rawRecorderState == 4) writeConst("4");

  writeConst(" length: ");
  writeDecodeInt(rawRecorderPos);
  writeDecode('\n');

  writeConst("    state machine: ");
  writeDecodeInt(en->state);
  writeDecode('\n');
  writeDecode('\n');
  writeDecode('\n');
  writeDecode('\n');
  writeDecode('\n');

}



void updateRadio()
{
  if (oldtxrx != txrx ||  oldprotocolID != en->protocolID || oldmodulationType != modulationType || oldfrequency != frequency || oldbandwidth != bandwidth || olddrate != drate || oldfhub != fhub)
  {
    radioConfig(0, txrx, en->protocolID, modulationType, frequency,  bandwidth,  drate,  fhub, 0, oldtxrx != txrx, oldprotocolID != en->protocolID, oldmodulationType != modulationType, oldfrequency != frequency, oldbandwidth != bandwidth, olddrate != drate, oldfhub != fhub);
  }
  oldtxrx = txrx;  oldprotocolID = en->protocolID; oldmodulationType = modulationType; oldfrequency = frequency; oldbandwidth = bandwidth; olddrate = drate; oldfhub = fhub;
}

char decodeDefault(char symbol, long value, char protocolID)
{
  if (symbol == en->startSymbol)
  {
    writeDecode('{');

  } else if (symbol == en->endSymbol)
  {
    writeDecode('}'); writeDecode('\n');

  } else
  {
    writeDecode(symbol);
  }
}

char encodeDefault(char symbol, char protocolID)
{
  if (symbol == '{')
  {
    if (en->startSymbol != 'N') writeEncode(en->startSymbol);
  }
  else if (symbol == '}')
  {
    if (en->endSymbol != 'N') writeEncode(en->endSymbol);
  }
  else
  {
    writeEncode(symbol);
  }
}

void writeDecode(short symbol)
{


  writeOut(symbol);

}

void writeDecodeInt(long value)
{
  ltoa(value, tempBuffer, 10);
  for (int i = 0; i < tempBufferSize; i++)
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
  if (rawRecorderState == 4)
  {
    putRawRecorder(time);
  } else
  {
    if (writeRawToConsole == 1)
    {
      writeDecodeInt(time); writeDecode('\n');

    } else
    {
      sendTTLDef(time);

    }
  }
  // debugOut(time);
}

void writeEncode(char symbolSrc)
{
  char symbolNr = 0;

  if (symbolSrc >= '0' && symbolSrc <= '9')
  {
    symbolNr = symbolSrc - '0';
    Symbol *s = &en->symbols[symbolNr];
    // debugOut(symbol);
    //   debugOut(s->duration);
    if (s->duration != 0) writeEncodeRaw(s->duration);
  }
  if (symbolSrc >= 'a' && symbolSrc <= 'f')
  {
    symbolNr = symbolSrc - 'a';
    Sequence *seq = &en->sequences[symbolNr];

    if (seq->a != 'N')
    {

      Symbol *s1 = &en->symbols[seq->a - '0'];
      writeEncodeRaw(s1->duration);
    }

    if (seq->b != 'N')
    {
      Symbol *s1 = &en->symbols[seq->b - '0'];
      writeEncodeRaw(s1->duration);
    }


  }

  if (symbolSrc >= 'v' && symbolSrc <= 'z')
  {

    symbolNr = symbolSrc - 'v';
    SubSequence *subSeq = &en->subSequences[symbolNr];

    if (subSeq->a >= 'a' && subSeq->a <= 'f')
    {
      symbolNr = subSeq->a - 'a';
      Sequence *seq = &en->sequences[symbolNr];

      Symbol *s = &en->symbols[seq->a - '0'];
      writeEncodeRaw(s->duration);

      s = &en->symbols[seq->b - '0'];
      writeEncodeRaw(s->duration);
    }

    if (subSeq->b >= 'a' && subSeq->b <= 'f')
    {
      symbolNr = subSeq->b - 'a';
      Sequence *seq = &en->sequences[symbolNr];

      Symbol *s = &en->symbols[seq->a - '0'];
      writeEncodeRaw(s->duration);

      s = &en->symbols[seq->b - '0'];
      writeEncodeRaw(s->duration);
    }


    if (subSeq->a >= '0' && subSeq->a <= '9')
    {
      Symbol *s = &en->symbols[subSeq->a - '0'];
      writeEncodeRaw(s->duration);

    }

    if (subSeq->b >= '0' && subSeq->b <= '9')
    {
      Symbol *s = &en->symbols[subSeq->b - '0'];
      writeEncodeRaw(s->duration);

    }










  }



}

void decodeSymbol(char symbol, long value)
{
  en->decodeProtocol(symbol, value, en->protocolID);
  en->tickCounter++;


}

char encodeSymbol(char symbol)
{
  return encodeProtocol(symbol, en->protocolID);
}


char desequenceSubStep(char symbol, short value, char sequenceStartPoint)
{

  // decodeSymbol(symbol,value);
  //return 0;



  switch (en->bposSub)
  {
    case 0:
      //en->prevSign1Sub = -1;
      //en->prevSignValue1Sub = -1;
      //en->prevFound1Sub = -1;

      for (int i = sequenceStartPoint; i < en->subSequenceCount; i++)
      {
        SubSequence *s = &en->subSequences[i];

        if (s->a == symbol)
        {

          en->bposSub = 1;
          en->prevSign1Sub = symbol;
          en->prevSignValue1Sub = value;
          en->prevFound1Sub = i;
          return 0;
        }
      }
      decodeSymbol(symbol, value);
      en->bposSub = 0;

      return 0;

    case 1:


      for (int i = sequenceStartPoint; i < en->subSequenceCount; i++)
      {
        SubSequence *s = &en->subSequences[i];
        if (s->a == en->prevSign1Sub &&  s->b == symbol)
        {

          decodeSymbol('v' + i, value);
          en->bposSub = 0;
          return 0;

        }
      }

      
      //Original
     // decodeSymbol(en->prevSign1Sub, en->prevSignValue1Sub); decodeSymbol(symbol, value);
     // en->bposSub = 0;
      
      //Alternativ
       en->bposSub = 1;
       decodeSymbol(en->prevSign1Sub, en->prevSignValue1Sub);
       en->prevSign1Sub = symbol; en->prevSignValue1Sub = value;
      return 0;
  }

  return 0;
}




char desequenceStep(char symbol, short value, char sequenceStartPoint)
{


  switch (en->bpos)
  {
    case 0:
      //en->prevSign1 = -1;
      //en->prevSignValue1 = -1;
      //en->prevFound1 = -1;

      for (int i = sequenceStartPoint; i < en->sequenceCount; i++)
      {
        Sequence *s = &en->sequences[i];

        if (s->a == symbol)
        {

          en->bpos = 1;
          en->prevSign1 = symbol;
          en->prevSignValue1 = value;
          en->prevFound1 = i;
          return 0;
        }
      }
      en->bpos = 0;
      desequenceSubStep(symbol, value, 0);
      //     decodeSymbol(symbol, value);


      return 0;

    case 1:


      for (int i = sequenceStartPoint; i < en->sequenceCount; i++)
      {
        Sequence *s = &en->sequences[i];
        if (s->a == en->prevSign1 &&  s->b == symbol)
        {
          en->bpos = 0;
          desequenceSubStep(s->name, value, 0);

          return 0;

        }
      }

    //Original
   //   en->bpos = 0;
   //   desequenceSubStep(en->prevSign1, en->prevSignValue1, 0); desequenceSubStep(symbol, value, 0);

      ///alternativ
       en->bpos = 1;
       desequenceSubStep(en->prevSign1, en->prevSignValue1, 0);
       en->prevSign1 = symbol; en->prevSignValue1 = value;



      return 0;
  }

  return 0;
}



char findSymbol(long DValue)
{
  short DValueDiv = DValue;
  if (DValue >= 32760) DValueDiv = 32760;
  if (DValue <= -32760) DValueDiv = -32760;
  for (int i = 0; i < en->symbolCount; i++)
  {
    Symbol *s = &en->symbols[i];
    if ( DValueDiv > s->min && DValueDiv < s->max)
    {
      return 48 + i;
      break;
    }
  }
  return -1;
}

void decodeStep(long DValue)
{


  //Serial.println(DValue,DEC);
  short DValueDiv = DValue;
  if (DValue >= maxRange) DValueDiv = maxRange;
  if (DValue <= minRange) DValueDiv = minRange;
  if (en->protocolID == 0)
  {
    writeDecodeInt(DValue); writeDecode(' ');
  } else
  {
    for (int i = 0; i < en->symbolCount; i++)
    {
      Symbol *s = &en->symbols[i];
      if ( DValueDiv >= s->min && DValueDiv <= s->max)
      {
        if (s->name == en->initSymbol)
        {
          en->bpos = 0; en->bposSub = 0;

        }

        desequenceStep(s->name, DValue, s->sequenceStartPoint);
        break;
      }
    }
  }
}

void decodeTickDeltaTime(unsigned long  deltaTime, char inputLevel)
{
  if (txrx == radioRX)
  {
    char level = inputLevel;
    if (lastLevel != level)
    {
      decodeStep(lastLevel * sumTime);
      sumTime = 0;
      lastLevel = level;
    }
    sumTime += deltaTime ;
  }
}

void decodeTickAbsoluteTime(unsigned long  absoluteTime, char inputLevel)
{
  if (txrx == radioRX)
  {
    char level = inputLevel;
    if (level == 0) level = -1;
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



void setHighLevelCallback(sendTTL func1, readSymbol func2, radioConfigCallback func3)
{
  en = &enviroments[0];

  //  en->a=9;
  // Symbol *s= &en->symbols[0];
  // s->duration =99;
  // en->symbols[0].duration =11111;



  writeOut = func2;
  sendTTLDef = func1;
  radioConfig = func3;
  initHighLevel();

  setProtocol(protDefault);
  updateRadio();
}

void sendSymbolString(char *symbolStr, int repeat)
{
  for (int j = 0; j < repeat; j++)
  {
    for (int i = 0; i < 200; i++)
    {
      sendSymbol(symbolStr[i]);
      if (symbolStr[i] == 0) break;
    }
  }
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
    encodeStep = 0;
  }
  if (en->protocolID == 0)
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
  oldtxrx = txrx;  oldprotocolID = en->protocolID; oldmodulationType = modulationType; oldfrequency = frequency; oldbandwidth = bandwidth; olddrate = drate; oldfhub = fhub;
  radioConfig(0, 0, 0, 0, 0,  0,  0,  0, 1, 1, 1, 1, 1, 1, 1, 1);
}

void enviromentReset()
{
  en->protocolID = 0;
  en->state = 0;
  en->decBufferPos = 0;
  en->symbolCount = 0;
  en->sequenceCount = 0;
  en->subSequenceCount = 0;
  en->bpos = 0;
  en->bposSub = 0;
  resetBuffersAndSetState(0);

  for (int i = 0; i < maxSymbol; i++)
  {
    struct Symbol *s = &en->symbols[i];
    s->min = 0; s->max = 0; s->duration = 0; s->name = 'N';
    s->sequenceStartPoint = 0;
  }
  for (int i = 0; i < maxSequence; i++)
  {
    struct Sequence *s = &en->sequences[i];
    s->a = 'N'; s->b = 'N'; s->name = 'N';
  }

  for (int i = 0; i < maxSubSequence; i++)
  {
    struct SubSequence *s = &en->subSequences[i];
    s->a = 'N'; s->b = 'N'; s->name = 'N';
  }



}

void resetHighLevel()
{
  txrx = radioIDLE;
  modulationType = modAM;
  frequency = 0;
  bandwidth = 0;
  drate = 0;
  fhub = 0;
  repeat = 1;
  debugMode = 0;
  linePos = 0;
  sumTime = 0;
  lastLevel = 0;
  enviromentReset();

}

void prepare()
{
  en->symbolCount = 0;
  for (int i = 0; i < maxSymbol; i++)
  {
    Symbol *s = &en->symbols[i];
    if (s->duration != 0)
    {
      en->symbolCount = i + 1;
    }
  }
  en->sequenceCount = 0;
  for (int i = 0; i < maxSequence; i++)
  {
    Sequence *s = &en->sequences[i];
    if (s->a != 'N' || s->b != 'N')
    {
      en->sequenceCount = i + 1;
    }
  }

  en->subSequenceCount = 0;
  for (int i = 0; i < maxSubSequence; i++)
  {
    SubSequence *s = &en->subSequences[i];
    if (s->a != 'N' || s->b != 'N')
    {
      en->subSequenceCount = i + 1;
    }
  }
}


void setSequence(unsigned char nr, char symbolA, char symbolB)
{
  if (printDescription == 0)
  {
    if (nr >= 'a' && nr <= 'f')
    {
      char nrLocal = nr - 'a';
      Sequence *s = &en->sequences[nrLocal];
      s->a = 'N'; s->b = 'N';
      if (symbolA != 'N')  s->a = symbolA ;
      if (symbolB != 'N')  s->b = symbolB ;
      s->name = nr;
    }

    if (nr >= 'v' && nr <= 'z')
    {
      char nrLocal = nr - 'v';
      SubSequence *s = &en->subSequences[nrLocal];
      s->a = 'N'; s->b = 'N';
      if (symbolA != 'N')  s->a = symbolA ;
      if (symbolB != 'N')  s->b = symbolB ;
      s->name = nr;

    }
    prepare();

  }
}

void setSubSequence(unsigned char nr, char symbolA, char symbolB)
{
  if (printDescription == 0)
  {
    char nrLocal = nr - 'v';
    SubSequence *s = &en->subSequences[nrLocal];
    s->a = 'N'; s->b = 'N';
    if (symbolA != 'N')  s->a = symbolA ;
    if (symbolB != 'N')  s->b = symbolB ;
    s->name = nr;
    prepare();
  }
}

void setSymbolRange(unsigned char nr, short duration, short minTime, short maxTime)
{
  if (printDescription == 0)
  {
    short minTimeDiv = minTime;
    short maxTimeDiv = maxTime;
    short durationDiv = duration;
    char nrLocal = nr - 48;

    Symbol *s = &en->symbols[nrLocal];
    s->duration = durationDiv;  s->name = nr;
    if (minTime < 0)
    {
      s->min = minTimeDiv; s->max = maxTimeDiv;
    } else
    {
      s->min = minTimeDiv; s->max = maxTimeDiv;
    }

    if (s->min == 0 && s->max == 0) duration = 0;
    if (s->duration == 0)
    {
      s->min = 0; s->max = 0;

    }
    prepare();
  }
}

void setSymbolMargin(unsigned char nr, short duration, float margin)
{
  short marginLocal = abs(duration * margin);
  setSymbolRange(nr, duration, duration - marginLocal, duration + marginLocal);
}

void setSymbolAbsoluteMargin(unsigned char nr, short duration, short margin)
{
  setSymbolRange(nr, duration, duration - margin, duration + margin);
}

void setSymbolFullRange(unsigned char nr, short duration)
{
  setSymbolRange(nr, duration, -32762, 32762);
}

void setSymbolFullRangeHigh(unsigned char nr, short duration)
{
  setSymbolRange(nr, duration, 1, 32762 );
}

void setSymbolFullRangeLOW(unsigned char nr, short duration)
{
  setSymbolRange(nr, duration, -32762, -1 );
}

char getState()
{
  return en->state;
}

void setState(char value)
{
  en->state = value;

}

void resetBuffersAndSetState(char value)
{
  resetTickCounter();
  resetBuffer();
  setState(value);


}

void resetBuffer()
{
  en->decBufferPos = 0;
}

short getDecBufferPos()
{
  return en->decBufferPos;
}

void writeBuffer(char symbol)
{
  if (en->decBufferPos < decBufferSize)
  {
    en->decBuffer[en->decBufferPos] = symbol;
    en->decBufferPos++;
  }
}

char readBuffer()
{
  if (en->decBufferPos < decBufferSize)
  {
    char r = en->decBuffer[en->decBufferPos];
    en->decBufferPos++;
    return r;
  }
  return 0;
}

char readDecBuffer(short index)
{
  return en->decBuffer[index];
}

void flushDecodeBuffer()
{
  writeDecode('{');
  for (int i = 0; i < en->decBufferPos; i++)
  {
    writeDecode(en->decBuffer[i]);
  }
  writeDecode('}');
  writeNewLine();
}

void flushEncodeBuffer()
{
  for (int i = 0; i < en->decBufferPos; i++)
  {
    writeEncode(en->decBuffer[i]);
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
  parseBufferPos = 0;
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
  while (az() == ' ') next();
}

char nextIgnoreBlank()
{
  if (az() == ' ')
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
  long number = 0;
  while (az() >= 48 && az() <= 57)
  {
    number += (az() - 48);
    next();
    if (az() >= 48 && az() <= 57) number *= 10;
  }
  skipBlank();
  return number * sign;
}

void parseData()
{
  int startPos = parseBufferPos;
  if (az() == '{')
  {
    for (int i = 0; i < repeat; i++)
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

        nextIgnoreBlank();
      }
    }
  }
  if (debugMode == 1 && en->protocolID == 1) writeDecode('\n');
  debugMode = 0;
}

void parseSequence(char symbol)
{
  char first = 'N';
  if (az() > '0' && az() <= '9')
  {
    first = az();
    nextIgnoreBlank();
  }
  char second = 'N';
  if (az() > '0' && az() <= '9')
  {
    second = az();
    nextIgnoreBlank();
  }

  setSequence(symbol, first, second);
}


void parseSubSequence(char symbol)
{
  char first = 'N';
  if ((az() > '0' && az() <= '9') || (az() >= 'a' && az() <= 'f' ))
  {
    first = az();
    nextIgnoreBlank();
  }
  char second = 'N';
  if ((az() > '0' && az() <= '9') || (az() >= 'a' && az() <= 'f' ))
  {
    second = az();
    nextIgnoreBlank();
  }

  setSequence(symbol, first, second);
}


void parseSymbol(char symbol)
{
  char variant = 0; short right = 0; short left = 0;
  short duration = parseNumber();
  if (az() == ':')
  { nextIgnoreBlank();
    left = parseNumber();
    variant = 1;
  }
  if (az() == ':')
  {
    nextIgnoreBlank();
    right = parseNumber();
    variant = 2;
  }
  if (variant == 0) setSymbolAbsoluteMargin(symbol,  duration, 0);
  if (variant == 1) setSymbolAbsoluteMargin(symbol,  duration, left);
  if (variant == 2) setSymbolRange(symbol, duration, left, right  );
}

void parseModulationType()
{
  if (az() == 'a')
  {
    next();
    modulationType = modAM;
  }
  if (az() == 'f')
  {
    next();
    modulationType = modFM;
  }
  nextIgnoreBlank();
}

char parseCommandLine(char *in)
{
  char tempTXRX = 0;  scanMode = 0;
  parseBuffer = in;
  resetParseBuffer();
  writeRawToConsole = 0;
  if (az() == 'Z')
  {
    resetRawRecorder();
    rawRecorderState = 0;
    return 0;
  }


  if (az() == 'L')
  {
    next();
    if (az() == 'P')
    {
      printSymbolTable();
    }
    if (az() == 'R')
    {
      printRawRecorder();
      rawRecorderState = 0;
    }

    if (az() == 'H')
    {
      printHelp();
      return 0;
    }
    return 0;
  }
  tempTXRX  = radioIDLE; char autoRX = 0;
  skipBlank();
  if (az() == '|')
  {
    rawRecorderState = -1; next();
  }
  if (az() == '<')
  {
    nextIgnoreBlank();
    if (rawRecorderState == -1)
    {
      rawRecorderState = 1;
      resetRawRecorder();
    } else
    {
      rawRecorderState = 0;
    }

    if (az() == '|')
    {
      rawRecorderState = 3; nextIgnoreBlank();
    }
    tempTXRX  = radioRX;
  }
  if (az() == '>')
  {
    nextIgnoreBlank();
    if (rawRecorderState == -1) rawRecorderState = 2;

    if (az() == '$')
    {
      nextIgnoreBlank();
      writeRawToConsole = 1;


    }

    if (az() == '?')
    {
      nextIgnoreBlank();
      debugMode = 1;


    }

    if (az() == '|')
    {
      rawRecorderState = 4; nextIgnoreBlank();
    }
    if (tempTXRX  == radioRX) autoRX = 1;
    tempTXRX  = radioTX;
  }
  char command = 0;
  skipBlank();
  while ((az() >= 'a' && az() <= 'z') || (az() >= 'A' && az() <= 'Z') || (az() >= 48 && az() <= 57))
  {
    command = az(); nextIgnoreBlank();
    if (az() == '=')
    {
      nextIgnoreBlank();
    }
    switch (command)
    {
      case 'F':
        frequency = parseNumber();
        break;

      case 'B':
        bandwidth = parseNumber();
        break;

      case 'A':
        drate = parseNumber();
        break;

      case 'Q':
        fhub = parseNumber();
        break;

      case 'P':
        if (az() == 'S')
        {
          nextIgnoreBlank(); if (rawRecorderState == 3) scanMode = 1;
        } else
        {
          setProtocol(parseNumber());
        }
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
        parseSequence(command);
        break;

      case 'v':
      case 'w':
      case 'x':
      case 'y':
      case 'z':

        parseSubSequence(command);
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

      case 'M':
        parseModulationType();
        break;
      default: break;
    }
  }
  if (rawRecorderState == 3)
  {
    if (scanMode == 1)
    {
      writeConst("Scanning protocols:\n");
      for (int j = 1; j < 10; j++)
      {

        // writeDecode('\n');
        setProtocol(j);
        for (int i = 0; i < rawRecorderPos; i++)
        {
          decodeStep(rawRecorderBuffer[i]);
        }
        //  writeDecode('\n');

      }
      scanMode = 0;
      setProtocol(1);
      writeDecode('\n');


    } else
    {
      for (int i = 0; i < rawRecorderPos; i++)
      {
        decodeStep(rawRecorderBuffer[i]);
      }
    }
    writeDecode('\n');
    rawRecorderState = 0;
    return 0;
    scanMode = 0;
  }


  if (rawRecorderState == 2)
  {
    txrx = tempTXRX ;
    updateRadio();
    for (int i = 0; i < rawRecorderPos; i++)
    {
      sendTTLDef(rawRecorderBuffer[i]);
    }
    txrx = radioIDLE; rawRecorderState = 0;
    updateRadio();
    rawRecorderState = 0;
    return 0;
  }
  if (writeRawToConsole == 1)
  {
    parseData();
    writeRawToConsole  = 0;

    return 0;
  }

  txrx = tempTXRX ;
  updateRadio();
  parseData();
  if (tempTXRX != 0)
  {
    if (autoRX == 1)
    {
      txrx = radioRX;
      updateRadio();
    } else
    {
      if (txrx == radioTX)
      {
        txrx = radioIDLE; rawRecorderState = 0 ;
        updateRadio();
      }
    }
  } else
  {
    txrx = radioIDLE; rawRecorderState = 0;
    updateRadio();
  }
  if (rawRecorderState == 4)
  {
    rawRecorderState = 0;
  }
  return 0;
}

void setDecodeProtocol(decodeProtocolCallback func)
{

  if (printDescription == 0) en->decodeProtocol = func;
}

void setEncodeProtocol(encodeProtocolCallback func)
{
  if (printDescription == 0)  encodeProtocol = func;
}

void setModulationType(char value)
{
  if (printDescription == 0 && paralellTaskCount == 0)  modulationType = value;
}

void setFrequency(long value)
{
  if (printDescription == 0 && paralellTaskCount == 0) frequency = value;
}

void setBandwidth(long value)
{
  if (printDescription == 0 && paralellTaskCount == 0)  bandwidth = value;
}

void setDrate(long value)
{
  if (printDescription == 0 && paralellTaskCount == 0) drate = value;
}

void setFhub(long value)
{
  if (printDescription == 0 && paralellTaskCount == 0)  fhub = value;
}

char getProtocolID()
{
  return en->protocolID;
}

void switchEnviroment(char enviromentSlot)
{
  en = &enviroments[enviromentSlot];


}

void defineProtocolParalellTask(char protID, char enviromentSlot)
{
  switchEnviroment(enviromentSlot);
  enviromentReset();
  onSetProtocol(protID);
  prepare();
  switchEnviroment(0);
  paralellTaskCount++;
}

void endDefineProtocolParalellTask()
{
  switchEnviroment(0);
}

void setProtocol(char protID)
{
  paralellTaskCount = 0;
  switchEnviroment(0);
  if (printDescription == 0) resetHighLevel();


  en->protocolID = protID;

  onSetProtocol(protID);
  if (printDescription == 0)
  {
    prepare();
    en->decBufferPos = 0;
    if (en->decodeProtocol == 0)
    {
      hasDecoder = 0;
    } else if (en->decodeProtocol == decodeDefault)
    {
      hasDecoder = 1;
    } else
    {
      hasDecoder = 2;
    }

    if (encodeProtocol == 0)
    {
      hasEncoder = 0;
    } else if (encodeProtocol == encodeDefault)
    {
      hasEncoder = 1;
    } else
    {
      hasEncoder = 2;
    }
  }

}









