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
#ifndef HIGHLEVELPROC_H_
#define HIGHLEVELPROC_H_
#include "config.h"
#include "Arduino.h"

#define maxRange 32762
#define minRange -32762

#define protScan -1
#define protRaw  0
#define protDefault  1
#define protIntertechno  2
#define protIntertechnoLong  3


#define tempBufferSize  33

#define maxSymbol 10
#define maxSequence 5
#define maxSubSequence 5
#define maxEnviroments 1



#define modAM  1
#define modFM  2

#define radioIDLE 0
#define radioTX 1
#define radioRX 2

typedef char (*decodeProtocolCallback)(char symbol,long value, char protocolID);
typedef char (*encodeProtocolCallback)(char symbol,char protocolID);

struct Symbol
{
  short min;
  short max;
  short duration;
  char sequenceStartPoint;
  char name;
};

struct Sequence
{
  char a;
  char b;
 
  char name;
};

struct SubSequence
{
char a;
char b;
char name;    
};

struct Enviroment
{
  char prevSign1 =0;
  char prevSignValue1 =0;
  char prevFound1=0;
  
  char prevSign1Sub =0;
  char prevSignValue1Sub =0;
  char prevFound1Sub=0;

  int tickCounter =0;
  char a;
char b;
char state;
char decBuffer[decBufferSize];
char decBufferPos = 0;
char bitSumerPos = 0;
char dualDecoderState = 0;
unsigned int bitSumer = 0;
  struct Symbol symbols[maxSymbol];
  struct Sequence sequences[maxSequence];
  struct SubSequence subSequences[maxSequence];
  char symbolCount = 0;
char sequenceCount = 0;
char subSequenceCount = 0;
  decodeProtocolCallback decodeProtocol;
  char protocolID;
  char bpos;
  char bposSub;
  char startSymbol ='N';
  char endSymbol ='N';
  char initSymbol ='N';
  
};

typedef void (*sendTTL) (long deltaTime);
typedef void (*readSymbol) (short symbol);



typedef void (*radioConfigCallback) (int radioID, char txrx, int protocolNr, int modulationType,  long frequency,  long bandwidth,  long drate,  long fhub,
char changeRadioID, char changeTxrx, char changeProtocolNr, char changeModulationType,  char changeFrequency,  char changeBandwidth,  char changeDrate,   char changeFhub);



void resetTickCounter();
int getTickCounter();
char getScanMode();
char isPrintDescription();
void sumBit(char bitValue);
void resetBitSumer();
int readLine(int readch, char *buffer, int len);
char parseCommandLine(char *in);
void writeDecodeInt(long value);
void writeDecode(short symbol);
void writeDecodeInt(long value);
void writeNewLine();
void printHelp();
void setProtocol(char protID);
char getProtocolID();
void resetParseBuffer();
char findSymbol(long DValue);
void decodeStep(long DValue);
void decodeTickAbsoluteTime(unsigned long  absoluteTime, char inputLevel);
void decodeTickDeltaTime(unsigned long  deltaTime, char inputLevel);
void setHighLevelCallback(sendTTL func1, readSymbol func2,radioConfigCallback func3);
void sendSymbolString(char *symbolStr, int repeat);
void sendSymbol(short symbolSrc);
void setSequence(unsigned char nr, char symbolA, char symbolB);
void setSubSequence(unsigned char nr, char symbolA, char symbolB);
void setSymbol(unsigned char nr, short minTime, short maxTime, short duration);
void setSymbolMargin(unsigned char nr, short duration, float margin);
void setSymbolAbsoluteMargin(unsigned char nr, short duration, short margin);
void setSymbolRange(unsigned char nr,short duration, short minTime, short maxTime );
void resetHighLevel();
void initHighLevel();
void updateRadio();
void beginRecive();
void beginTransmit();
void endTransmitRecive();
void writeDecode(short symbol);
void writeDecodeInt(long value);
void writeEncode(char symbolSrc);
void writeBufferText(char *text);
void flushEncodeBuffer();
void flushDecodeBuffer();
short getDecBufferPos();
char readBuffer();
void writeBuffer(char symbol);
char readDecBuffer(short index);
void resetBuffer();
short getDecBufferPos();
char getState();
void setState(char value);
void resetBuffersAndSetState(char value);
char decodeDefault(char symbol, long value, char protocolID);
char encodeDefault(char symbol, char protocolID);
void setDecodeProtocol(decodeProtocolCallback func);
void setEncodeProtocol(encodeProtocolCallback func);
void setModulationType(char value);
void setFrequency(long value);
void setBandwidth(long value);
void setDrate(long value);
void setFhub(long value);
#endif /* HIGHLEVELPROC_H_ */
