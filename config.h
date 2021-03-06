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


#ifndef rfConfig_H_
#define rfConfig_H_

#define CSPIN  53
#define MISO_PIN  47
#define MOSI_PIN  45
#define SCK_PIN  43
#define CS_PIN  41



//#define rxPin 4
//#define txPin 4

#define rxPin 20
#define txPin 24

//#define rawRecorderSize 0
//#define decBufferSize 128
//#define maxSymbol 6
//#define maxSequence 3
//#define maxSubSequence 3
//#define maxEnviroments 1

#define rawRecorderSize 1
#define decBufferSize 256
#define maxSymbol 10
#define maxSequence 5
#define maxSubSequence 5
#define maxEnviroments 12

#define CC1101


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
