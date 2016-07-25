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


void calcDecodeBuffer()
{
  resetBitSumer();
   writeDecode('{');
  for (int i = 0; i < getDecBufferPos(); i++)
  {
    
    char z = readDecBuffer(i);
    if (z=='f') sumBit(1);
    if (z=='0') sumBit(0);
    if (i==4)  // HouseCode
    {
      
     resetBitSumer();  
    }
    
    if (i==4) // GRoupSwicht
    {
      
     resetBitSumer();  
    }
    
  }
  writeDecode('}');
  writeDecode('\n');
  
  
  
}


char decodeIntertechno(char symbol,long value, char protocolID)
{
  
if (symbol == '5')
  {
    resetBuffer();
    setState(1);
    
  }

  switch (getState())
  {
    //	 Serial.print(symbol);
    case 0:
      if (symbol == '5') setState(1);   return 0;
      break;
    case 1:
    if (getDecBufferPos() ==4) writeBuffer(' ');
    if (getDecBufferPos() ==9) writeBuffer(' ');
    if (getDecBufferPos() ==12) writeBuffer(' ');
    
     
      if (symbol == 'a')

      {
        setState(2);
        return 0;
      }
      if (symbol == 'b') {
        setState(3);
        return 0;
      }
      if (symbol == '3')
      {
        setState(0);
        if (getDecBufferPos() == 15 && (    (readDecBuffer(13) == 'f'  || readDecBuffer(13) == '0') && (readDecBuffer(14) == 'f'  || readDecBuffer(14) == '0')    ) )   flushDecodeBuffer();

        return 0;
      }
      resetBuffer();

      break;
    case 2:
      if (symbol == 'a')
      {
        setState(1);
        writeBuffer('0');
      
        return 0;
      }

      if (symbol == 'b')
      {
        setState(1);
       
        writeBuffer('f');
        return 0;
      }
      break;

    case 3:
      if (symbol == 'b')
      {
        setState(1);
        writeBuffer('1');
        return 0;
      }
      break;

    case 4:
      if (symbol == '3')
      {
        setState(0);
      }
    default:
      break;
  }
  return 0;
  
  
}

char encodeIntertechno(char symbol, char protocolID)
{
  
  if (symbol == '0')
      {
        writeEncode('a'); writeEncode('a'); // send symbol A (from set sequence) 300 ys High 1000 ys Low
      }

      if (symbol == '1')
      {
        writeEncode('b'); writeEncode('b');
      }
      if (symbol == 'f')
      {
        writeEncode('a'); writeEncode('b');
      }
      if (symbol == '{')  // start condition
      {
        writeEncode('5');
      }
      if (symbol == '}')  // end conbdition
      {
        writeEncode('3'); writeEncode('5');
      }
  
return 0;  
  
  
}
