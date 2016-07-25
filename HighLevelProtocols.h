#ifndef HIGHLEVELPROTOCOLS_H_
#define HIGHLEVELPROTOCOLS_H_

char decodeDefault(char symbol, long value, char protocolID);
char encodeDefault(char symbol, char protocolID);
void onSetProtocol(char protID);

#endif
