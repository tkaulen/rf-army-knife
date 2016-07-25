#include "SoftwareSPI.h"
#include "config.h"

#ifdef AVRBoard
uint8_t MISO_PORT;
uint8_t MISO_BIT;
volatile uint8_t *MISO_OUTPUTREGISTER;
volatile uint8_t *MISO_INPUTREGISTER;
uint8_t MOSI_PORT;
uint8_t MOSI_BIT;
volatile uint8_t *MOSI_OUTPUTREGISTER;
volatile uint8_t *MOSI_INPUTREGISTER;
uint8_t SCK_PORT;
uint8_t SCK_BIT;
volatile uint8_t *SCK_OUTPUTREGISTER;
volatile uint8_t *SCK_INPUTREGISTER;
uint8_t CS_PORT;
uint8_t CS_BIT;
volatile uint8_t *CS_OUTPUTREGISTER;
volatile uint8_t *CS_INPUTREGISTER;

void SoftSPI(void)
{
  
  MISO_PORT = digitalPinToPort(MISO_PIN);
  MISO_BIT = digitalPinToBitMask(MISO_PIN);
  MISO_OUTPUTREGISTER = portOutputRegister(MISO_PORT);
  MISO_INPUTREGISTER = portInputRegister(MISO_PORT);
  pinMode (MISO_PIN, INPUT);

  MOSI_PORT = digitalPinToPort(MOSI_PIN);
  MOSI_BIT = digitalPinToBitMask(MOSI_PIN);
  MOSI_OUTPUTREGISTER = portOutputRegister(MOSI_PORT);
  MOSI_INPUTREGISTER = portInputRegister(MOSI_PORT);
  pinMode (MOSI_PIN, OUTPUT);

  SCK_PORT = digitalPinToPort(SCK_PIN);
  SCK_BIT = digitalPinToBitMask(SCK_PIN);
  SCK_OUTPUTREGISTER = portOutputRegister(SCK_PORT);
  SCK_INPUTREGISTER = portInputRegister(SCK_PORT);
  pinMode (SCK_PIN, OUTPUT);

  CS_PORT = digitalPinToPort(CS_PIN);
  CS_BIT = digitalPinToBitMask(CS_PIN);
  CS_OUTPUTREGISTER = portOutputRegister(CS_PORT);
  CS_INPUTREGISTER = portInputRegister(CS_PORT);
  pinMode (CS_PIN, OUTPUT);
}

void csHigh(void)
{
  *CS_OUTPUTREGISTER |= CS_BIT;
}

void csLow(void)
{
  *CS_OUTPUTREGISTER &= ~ CS_BIT;
}

void sckHigh(void)
{
  *SCK_OUTPUTREGISTER |= SCK_BIT;
}

void sckLow(void)
{
  *SCK_OUTPUTREGISTER &= ~ SCK_BIT;
}

void mosiHigh(void)
{
  *MOSI_OUTPUTREGISTER |= MOSI_BIT;
}

void mosiLow(void)
{
  *MOSI_OUTPUTREGISTER &= ~ MOSI_BIT;
}

char misoGET(void)
{
  if (* MISO_INPUTREGISTER & MISO_BIT) return 1;
  return 0;
}
#endif


#ifdef ARMBoard
void SoftSPI(void)
{
  pinMode (MISO_PIN, INPUT);
  pinMode (MOSI_PIN, OUTPUT);
  pinMode (SCK_PIN, OUTPUT);
  pinMode (CS_PIN, OUTPUT);
}

void csHigh(void)
{
  digitalWrite(CS_PIN, HIGH);
}

void csLow(void)
{
  digitalWrite(CS_PIN, LOW);
}

void sckHigh(void)
{
  digitalWrite(SCK_PIN, HIGH);
}

void sckLow(void)
{
  digitalWrite(SCK_PIN, LOW);
}

void mosiHigh(void)
{
  digitalWrite(MOSI_PIN, HIGH);
}

void mosiLow(void)
{
  digitalWrite(MOSI_PIN, LOW);
}

char misoGET(void)
{
  return 0;
}
#endif

unsigned char transferSoftwareSPI(unsigned char data)
{
  unsigned char ret = 0;
  unsigned char bitnum = 128;
  unsigned char val;
  unsigned char i = 0;

  for (i = 0; i < 8; ++i) {
    (data & bitnum) ? mosiHigh() : mosiLow();
    sckHigh();

    val = misoGET();
    sckLow();
    ret <<= 1;
    ret |= (val == 0 ? 0 : 1);
    bitnum >>= 1;
  }
  return ret;
}

