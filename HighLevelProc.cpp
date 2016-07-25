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
char state = 0;
char decBuffer[decBufferSize];
char decBufferPos = 0;
char *parseBuffer;
int parseBufferPos;
char debugMode = 0;
char tempBuffer[tempBufferSize];
char printDescription = 0;
char hasDecoder;
char hasEncoder;
char loopBack = 0;
int linePos = 0;
long lastTimestamp = 0;
unsigned int bitSumer = 0;
char bitSumerPos = 0;
int encodeStep = 0;
char dualDecoderState = 0;
char rawRecorderState = 0;
short rawRecorderBuffer[rawRecorderSize];
unsigned short rawRecorderPos;
char scanMode = 0;


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
  dualDecoderState = 1;
}

char dualDecoder(char symbol, char refSymbol1, char refSumbol2)
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
        return 1;
      }

      if (symbol == refSumbol2)
      {
        dualDecoderState = 1;
        return 2;
      }
      break;

    case 3:
      if (symbol == refSymbol1)
      {
        dualDecoderState = 1;
        return 3;
      }
      if (symbol == refSumbol2)
      {
        dualDecoderState = 1;
        return 4;
      }
      break;
  }
  return -1;
}

void resetBitSumer()
{
  bitSumer = 0;
  bitSumerPos = 0;
}

void sumBit(char bitValue)
{
  if (bitValue) bitSumer |= 1 << bitSumerPos;
  bitSumerPos++;
}

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

        rpos = linePos;
        if (txrx != radioIDLE && linePos == 0) {
          txrx = radioIDLE; rawRecorderState = 0; updateRadio();
        }
        linePos = 0;  // Reset position index ready for next time
        resetParseBuffer();
        if (rpos != 0) parseCommandLine(parseBuffer);
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



void printProtocols()
{
  char tempProtocol = protocolID;
  printDescription = 1;
  writeConst("1) ----- Protocols -----\n");
  writeConst("#0 raw mode\n");
  for (int i = 0; i < 100; i++)
  {
    setProtocol(i);
  }


  protocolID = tempProtocol;
  printDescription = 0;
  writeConst("[P] selected protocol: ");
  writeDecodeInt(protocolID);
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

  writeConst("2) ----- Symbols -----\n");
  writeConst("(duration:min:max or duration:tollerance example:1=150:100:200,1=150:50)\n");
  for (int i = 0; i < symbolCount; i++)
  {
    Symbol *s = &symbols[i];
    writeDecode('[');
    writeDecodeInt(i);
    writeDecode(']');
    writeConst(" duration:");
    writeDecodeInt(s->duration);
    writeConst(" min:");
    writeDecodeInt(s->min);
    writeConst(" max:");
    writeDecodeInt(s->max);
    writeDecode('\n');
  }

  writeDecode('\n');
  writeConst("3) ----- Sequences -----\n");
  writeConst("(FirstSecond example:a=12)\n");

  for (int i = 0; i < sequenceCount; i++)
  {
    Sequence *q = &sequences[i];
    writeDecode('[');
    writeDecode('a' + i);
    writeDecode(']');
    writeConst(" first:");
    writeDecode('0' + q->a);
    writeConst(" second:");
    writeDecode('0' + q->b);
    writeDecode('\n');
  }

  writeDecode('\n');
  writeConst("4) ----- RF Tunings -----\n");

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


  writeConst("5) ----- other Settings -----\n");
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
  writeDecodeInt(state);
  writeDecode('\n');
  writeDecode('\n');
  writeDecode('\n');
  writeDecode('\n');
  writeDecode('\n');

}



void updateRadio()
{
  if (oldtxrx != txrx ||  oldprotocolID != protocolID || oldmodulationType != modulationType || oldfrequency != frequency || oldbandwidth != bandwidth || olddrate != drate || oldfhub != fhub)
  {
    radioConfig(0, txrx, protocolID, modulationType, frequency,  bandwidth,  drate,  fhub, 0, oldtxrx != txrx, oldprotocolID != protocolID, oldmodulationType != modulationType, oldfrequency != frequency, oldbandwidth != bandwidth, olddrate != drate, oldfhub != fhub);
  }
  oldtxrx = txrx;  oldprotocolID = protocolID; oldmodulationType = modulationType; oldfrequency = frequency; oldbandwidth = bandwidth; olddrate = drate; oldfhub = fhub;
}

char decodeDefault(char symbol, long value, char protocolID)
{
  writeDecode(symbol);
}

char encodeDefault(char symbol, char protocolID)
{
  writeEncode(symbol);
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
    sendTTLDef(time);
  }
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

void decodeSymbol(char symbol, long value)
{
  if (symbol <= 9) decodeProtocol(symbol + 48, value, protocolID); else decodeProtocol(symbol + 'a' - 10, value, protocolID);
}

char encodeSymbol(char symbol)
{
  return encodeProtocol(symbol, protocolID);
}

char desequenceStep(char symbol, short value, char sequenceStartPoint)
{
  switch (bpos)
  {
    case 2:
      decodeSymbol(a, value);
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
      decodeSymbol(10 + i, value);
      bpos = 0;
      return 0;
    }
  }
  if (matchA == 0)
  {

    if (bpos == 1)
    {
      decodeSymbol(symbol, value);
      bpos = 0;
      return 0;
    }
    if (bpos == 2)
    {
      decodeSymbol(a, value); decodeSymbol(b, value);
      bpos = 0;
      return 0;
    }
  }
}


char findSymbol(long DValue)
{
  short DValueDiv = DValue;
  if (DValue >= 32760) DValueDiv = 32760;
  if (DValue <= -32760) DValueDiv = -32760;
  for (int i = 0; i < symbolCount; i++)
  {
    Symbol *s = &symbols[i];
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
  short DValueDiv = DValue;
  if (DValue >= 32760) DValueDiv = 32760;
  if (DValue <= -32760) DValueDiv = -32760;
  if (protocolID == 0)
  {
    writeDecodeInt(DValue); writeDecode(' ');
  } else
  {
    for (int i = 0; i < symbolCount; i++)
    {
      Symbol *s = &symbols[i];
      if ( DValueDiv > s->min && DValueDiv < s->max)
      {
        desequenceStep(i, DValue, s->sequenceStartPoint);
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



void setHighLevelCallback(sendTTL func1, readSymbol func2, radioConfigCallback func3)
{
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
  radioConfig(0, 0, 0, 0, 0,  0,  0,  0, 1, 1, 1, 1, 1, 1, 1, 1);
}

void resetHighLevel()
{
  txrx = radioIDLE;
  modulationType = modAM;
  frequency = 0;
  bandwidth = 0;
  drate = 0;
  fhub = 0;
  protocolID = 0;
  symbolCount = 0;
  sequenceCount = 0;
  repeat = 1;
  debugMode = 0;
  linePos = 0;
  state = 0;
  decBufferPos = 0;
  sumTime = 0;
  lastLevel = 0;
  for (int i = 0; i < maxSymbol; i++)
  {
    struct Symbol *s = &symbols[i];
    s->min = 0; s->max = 0; s->duration = 0;
    s->sequenceStartPoint = 0;
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
    if (s->duration != 0)
    {
      symbolCount = i + 1;
    }
  }
  sequenceCount = 0;
  for (int i = 0; i < maxSequence; i++)
  {
    Sequence *s = &sequences[i];
    if (s->a != 0 || s->b != 0)
    {
      sequenceCount = i + 1;
    }
  }
}

void setSequence(unsigned char nr, char symbolA, char symbolB)
{
  if (printDescription == 0)
  {
    char nrLocal = nr - 'a';
    Sequence *s = &sequences[nrLocal];
    s->a = symbolA - '0'; s->b = symbolB - '0';
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
  return state;
}

void setState(char value)
{
  state = value;

}

void resetBuffer()
{
  decBufferPos = 0;
}

short getDecBufferPos()
{
  return decBufferPos;
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

char readDecBuffer(short index)
{
  return decBuffer[index];
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
        sendSymbol(az());
        nextIgnoreBlank();
      }
    }
  }
}

void parseSequence(char symbol)
{
  char left = az();
  nextIgnoreBlank();
  char right = az();
  nextIgnoreBlank();
  setSequence(symbol, left, right);
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
  char tempTXRX = 0;  scanMode =0;
  parseBuffer = in;
  resetParseBuffer();
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
      for (int j=1; j<10;j++)
      {
       
     // writeDecode('\n');
        setProtocol(j);
      for (int i = 0; i < rawRecorderPos; i++)
      {
        decodeStep(rawRecorderBuffer[i]);
      }
    //  writeDecode('\n');
        
     }
     scanMode =0;
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
    scanMode =0;
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

  if (printDescription == 0) decodeProtocol = func;
}

void setEncodeProtocol(encodeProtocolCallback func)
{
  if (printDescription == 0)  encodeProtocol = func;
}

void setModulationType(char value)
{
  if (printDescription == 0)  modulationType = value;
}

void setFrequency(long value)
{
  if (printDescription == 0) frequency = value;
}

void setBandwidth(long value)
{
  if (printDescription == 0)  bandwidth = value;
}

void setDrate(long value)
{
  if (printDescription == 0) drate = value;
}

void setFhub(long value)
{
  if (printDescription == 0)  fhub = value;
}

char getProtocolID()
{
  return protocolID;
}

void setProtocol(char protID)
{
  if (printDescription == 0) resetHighLevel();


  protocolID = protID;

  onSetProtocol(protID);
  if (printDescription == 0)
  {
    prepare();
    decBufferPos = 0;
    if (decodeProtocol == 0)
    {
      hasDecoder = 0;
    } else if (decodeProtocol == decodeDefault)
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









