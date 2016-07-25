
#ifndef rfConfig_H_
#define rfConfig_H_

#include "Arduino.h" 

#undef AVRBoard
#undef ARMBoard
#define DefaultBoard

#if defined (__arm__)
#undef DefaultBoard
#define ARMBoard
#endif

#if defined (__AVR__)
#undef DefaultBoard
 #define AVRBoard
#endif

//#define AVRBoard
//#define DefaultBoard




#endif
