/*
 * HighLevelProc.h
 *
 * Created: 23.06.2016 13:23:02
 *  Author: thomas
 */



#ifndef HIGHLEVELPROC_H_
#define HIGHLEVELPROC_H_

#define protRaw  0
#define protDefault  1
#define protIntertechno  2
#define protIntertechnoLong  3

#define decBufferSize 512
#define tempBufferSize  33


#define maxSymbol 10
#define maxSequence 5

#define modAM  1
#define modFM  2

#define radioIDLE 0
#define radioTX 1
#define radioRX 2


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

typedef void (*sendTTL) (long deltaTime);
typedef void (*readSymbol) (short symbol);

typedef char (*decodeProtocolCallback)(char symbol,long value, char protocolID);
typedef char (*encodeProtocolCallback)(char symbol,char protocolID);

typedef void (*radioConfigCallback) (int radioID, char txrx, int protocolNr, int modulationType,  long frequency,  long bandwidth,  long drate,  long fhub,
char changeRadioID, char changeTxrx, char changeProtocolNr, char changeModulationType,  char changeFrequency,  char changeBandwidth,  char changeDrate,   char changeFhub);

int readLine(int readch, char *buffer, int len);
char parseCommandLine(char *in);
void writeDecodeInt(long value);
void writeDecode(short symbol);
void printHelp();
void setProtocol(char protID);
void resetParseBuffer();
void decodeStep(long DValue);
void decodeTickAbsoluteTime(unsigned long  absoluteTime, char level);
void decodeTickDeltaTime(unsigned long  deltaTime, char level);
void setHighLevelCallback(sendTTL func1, readSymbol func2,radioConfigCallback func3);
void sendSymbolString(char *symbolStr, int repeat);
void sendSymbol(short symbolSrc);
void setSequence(unsigned char nr, char symbolA, char symbolB);
void setSymbol(unsigned char nr, short minTime, short maxTime, short duration);
void setSymbolMargin(unsigned char nr, short duration, float margin);
void resetHighLevel();
void initHighLevel();
void updateRadio();
void beginRecive();
void beginTransmit();
void endTransmitRecive();

#endif /* HIGHLEVELPROC_H_ */
