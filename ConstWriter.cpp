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

#include "config.h"
#include "HighLevelProc.h"


#ifdef AVRBoard
#define writeDescription(x) SerialPrint_P(PSTR(x))
void SerialPrint_P(PGM_P str) {
  if (isPrintDescription() || getScanMode())
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
