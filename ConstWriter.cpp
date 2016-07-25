#include "config.h"
#include "HighLevelProc.h"


#ifdef AVRBoard
#define writeDescription(x) SerialPrint_P(PSTR(x))
void SerialPrint_P(PGM_P str) {
  if (isPrintDescription())
  {
    writeDecode('#'); writeDecodeInt(getProtocolID()); writeDecode(' ');
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
  if (isPrintDescription())
  {
    writeDecode('#'); writeDecodeInt(getProtocolID()); writeDecode(' ');
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
