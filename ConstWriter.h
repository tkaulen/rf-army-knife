#ifndef CONSTWRITER_H_
#define CONSTWRITER_H_

#include "config.h"


#ifdef AVRBoard
#define writeDescription(x) SerialPrint_P(PSTR(x))
void SerialPrint_P(PGM_P str);

#define writeConst(x) SerialPrint_P2(PSTR(x))
void SerialPrint_P2(PGM_P str);
#endif

#if (defined ARMBoard || defined DefaultBoard)
void writeDescription(char *text);
void writeConst(char *text);



#endif




#endif
