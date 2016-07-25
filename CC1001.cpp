/*
Copyright (C) 2016 Thomas Kaulen

 * Inspired by code from Dirk Tostmann, R.Koenig

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

#include "CC1001.h"
#include "Arduino.h"
#include "config.h"
#include "SoftwareSPI.h"


long ccFrequency = 443000000;

/////////////////////// CC1001 //////////////////////

const uint8_t CC1100_PA[] = {

  0xa3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 10 dBm
  0x00, 0x03, 0x0F, 0x1E, 0x25, 0x36, 0x38, 0x67, // -5 dBm
  0x00, 0x03, 0x0F, 0x25, 0x67, 0x40, 0x60, 0x50, //  0 dBm
  0x00, 0x03, 0x0F, 0x27, 0x51, 0x88, 0x83, 0x81, //  5 dBm
  0x00, 0x03, 0x0F, 0x27, 0x50, 0xC8, 0xC3, 0xC2, // 10 dBm

  0x00, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //-10 dBm no PA ramping
  0x00, 0x67, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // -5 dBm
  0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  0 dBm
  0x00, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  5 dBm
  0x00, 0xC2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 10 dBm

};

const uint8_t CC1100_INIT_CFG[] = {
  // CULFW   IDX NAME     RESET STUDIO COMMENT
  0x0D, // 00 IOCFG2   *29   *0B    GDO2 as serial output
  0x2E, // 01 IOCFG1    2E    2E    Tri-State
  0x2D, // 02 IOCFG0   *3F   *0C    GDO0 for input
  0x07, // 03 FIFOTHR   07   *47
  0xD3, // 04 SYNC1     D3    D3
  0x91, // 05 SYNC0     91    91
  0x3D, // 06 PKTLEN   *FF    3D
  0x04, // 07 PKTCTRL1  04    04
  0x32, // 08 PKTCTRL0 *45    32
  0x00, // 09 ADDR      00    00
  0x00, // 0A CHANNR    00    00
  0x06, // 0B FSCTRL1  *0F    06    152kHz IF Frquency
  0x00, // 0C FSCTRL0   00    00
  0x10, // 0D FREQ2    *1E    21    433.92 (InterTechno Frequency)
  0xb0, // 0E FREQ1    *C4    65
  0x71, // 0F FREQ0    *EC    e8
  0x55, // 10 MDMCFG4  *8C    55    bWidth 325kHz
  0xe4, // 11 MDMCFG3  *22   *43    Drate:1500 ((256+228)*2^5)*26000000/2^28
  0x30, // 12 MDMCFG2  *02   *B0    Modulation: ASK
  0x23, // 13 MDMCFG1  *22    23
  0xb9, // 14 MDMCFG0  *F8    b9    ChannelSpace: 350kHz
  0x00, // 15 DEVIATN  *47    00
  0x07, // 16 MCSM2     07    07
  0x00, // 17 MCSM1     30    30
  0x18, // 18 MCSM0    *04    18    Calibration: RX/TX->IDLE
  0x14, // 19 FOCCFG   *36    14
  0x6C, // 1A BSCFG     6C    6C
  0x07, // 1B AGCCTRL2 *03   *03    42 dB instead of 33dB
  0x00, // 1C AGCCTRL1 *40   *40
  0x90, // 1D AGCCTRL0 *91   *92    4dB decision boundery
  0x87, // 1E WOREVT1   87    87
  0x6B, // 1F WOREVT0   6B    6B
  0xF8, // 20 WORCTRL   F8    F8
  0x56, // 21 FREND1    56    56
  0x11, // 22 FREND0   *16    17    0x11 for no PA ramping
  0xE9, // 23 FSCAL3   *A9    E9
  0x2A, // 24 FSCAL2   *0A    2A
  0x00, // 25 FSCAL1    20    00
  0x1F, // 26 FSCAL0    0D    1F
  0x41, // 27 RCCTRL1   41    41
  0x00, // 28 RCCTRL0   00    00
};

void ccInit(void)
{
  pinMode (51, OUTPUT);
  digitalWrite(51, HIGH);
  pinMode (49, OUTPUT);
  digitalWrite(49, LOW);
  pinMode (39, OUTPUT);
  digitalWrite(39, HIGH);
  pinMode (37, OUTPUT);
  digitalWrite(37, LOW);
 SoftSPI();
 ccInitChip();
}

void CC1100_ASSERT(void)
{
  csLow();
}

void CC1100_DEASSERT(void)
{
  csHigh();
}

uint8_t ccStrobe(uint8_t strobe)
{
  CC1100_ASSERT();
  uint8_t ret = cc1100_sendbyte( strobe );
  CC1100_DEASSERT();
  return ret;
}


uint8_t cc1100_sendbyte(uint8_t data)
{
  return  transferSoftwareSPI(data);;
}
uint8_t cc1100_readReg(uint8_t addr)
{
  CC1100_ASSERT();
  cc1100_sendbyte( addr | CC1100_READ_BURST );
  uint8_t ret = cc1100_sendbyte( 0 );
  CC1100_DEASSERT();
  return ret;
}
void cc1100_writeReg(uint8_t addr, uint8_t data)
{
  CC1100_ASSERT();
  cc1100_sendbyte( addr | CC1100_WRITE_BURST );
  cc1100_sendbyte( data );
  CC1100_DEASSERT();
}

void ccIdle(void)
{
  ccStrobe(CC1100_SIDLE);
  delayMicroseconds(10);
}

void ccTX(void)
{
  cc1100_writeReg(2, 0x2d);
  uint8_t cnt = 0xff;
  // Going from RX to TX does not work if there was a reception less than 0.5
  // sec ago. Due to CCA? Using IDLE helps to shorten this period(?)
  ccStrobe(CC1100_SIDLE);
  while (cnt-- &&
         (ccStrobe(CC1100_STX) & CC1100_STATUS_STATE_BM) != CC1100_STATE_TX)
    delayMicroseconds(10);
}

void ccRX(void)
{
  cc1100_writeReg(2, 0x0d);
  uint8_t cnt = 0xff;
  while (cnt-- &&
         (ccStrobe(CC1100_SRX) & CC1100_STATUS_STATE_BM) != CC1100_STATE_RX)
    delayMicroseconds(10);
}

void ccConfigAll(int radioID, char txrx, int protocolNr, int modulationType,  long frequency,  long bandwidth,  long drate,  long fhub,
char changeRadioID, char changeTxrx, char changeProtocolNr, char changeModulationType,  char changeFrequency,  char changeBandwidth,  char changeDrate,   char changeFhub)
{
  //ccInitChip();
  
}

void ccConfigChip(const uint8_t *data)
{
  CC1100_DEASSERT();                           // Toggle chip select signal
  delayMicroseconds(30);
  CC1100_ASSERT();
  delayMicroseconds(30);
  CC1100_DEASSERT();
  delayMicroseconds(45);

  ccStrobe( CC1100_SRES );                   // Send SRES command
  delayMicroseconds(100);
  uint8_t i = 0;

  // load configuration
  for (i = 0; i < 60; i += 2) {

    if (pgm_read_byte( data[i]) > 0x40)
      break;

    cc1100_writeReg( data[i],
                     data[i + 1] );
  }

  CC1100_DEASSERT();

  CC1100_ASSERT();
  cc1100_sendbyte( CC1100_PATABLE | CC1100_WRITE_BURST );
  for ( i = 0; i < 8; i++) {
    cc1100_sendbyte(CC1100_PA[i]);
  }

  CC1100_DEASSERT();
  ccStrobe( CC1100_SCAL );
  delay(1);
}

void ccInitChip(void)
{

  CC1100_DEASSERT();                           // Toggle chip select signal
  delayMicroseconds(30);
  CC1100_ASSERT();
  delayMicroseconds(30);
  CC1100_DEASSERT();
  delayMicroseconds(45);

  ccStrobe( CC1100_SRES );                   // Send SRES command
  delayMicroseconds(100);

  CC1100_ASSERT();                             // load configuration
  cc1100_sendbyte( 0 | CC1100_WRITE_BURST );

  uint8_t i = 0;
  for ( i = 0; i < sizeof(CC1100_INIT_CFG); i++) {

    if (i >= 13 && i <=15) // frequency
    {
        
        if (i == 13)
        {
          double reducetFreq = ccFrequency / 1000.0;
          double fHex =   (reducetFreq) / (26.0 / 65536.0);
          long int i = ( long int)fHex;
          unsigned char a = i & 0xFF;
          unsigned char b = (i >> 8) & 0xFF;
          unsigned char c = (i >> 16) & 0xFF;
          cc1100_sendbyte(a);
          cc1100_sendbyte(b);
          cc1100_sendbyte(c);
       }
      
    } if ( i== 18) //// Modulation
    {
    
       cc1100_sendbyte(0x30);
    
    } else
    {
    
       cc1100_sendbyte(CC1100_INIT_CFG[i]);
    }
  }
  CC1100_DEASSERT();

  CC1100_ASSERT();
  cc1100_sendbyte( CC1100_PATABLE | CC1100_WRITE_BURST );
  for (i = 0; i < 8; i++) {
    cc1100_sendbyte(CC1100_PA[i]);
  }

  CC1100_DEASSERT();
  ccStrobe( CC1100_SCAL );
  delay(1);

  // setFrequency(433647);
  // setFrequency(433643);
  // setFrequency(433680);
}

uint32_t ccSetFrequency(uint32_t desiredFreq) {

  double reducetFreq = desiredFreq / 1000.0;
  double fHex =   (reducetFreq) / (26.0 / 65536.0);
  long int i = ( long int)fHex;
  unsigned char a = i & 0xFF;
  unsigned char b = (i >> 8) & 0xFF;
  unsigned char c = (i >> 16) & 0xFF;
  cc1100_writeReg(CC1100_FREQ0, a);
  cc1100_writeReg(CC1100_FREQ1, b);
  cc1100_writeReg(CC1100_FREQ2, c);
  ccStrobe( CC1100_SCAL );
  delay(1);
}






