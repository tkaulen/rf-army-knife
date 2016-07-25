#ifndef SOFTWARESPI_H_
#define SOFTWARESPI_H_



void SoftSPI(void);
void csHigh(void);
void csLow(void);
void sckHigh(void);
void sckLow(void);
void mosiHigh(void);
void mosiLow(void);
char misoGET(void);
unsigned char transferSoftwareSPI(unsigned char data);


#endif
