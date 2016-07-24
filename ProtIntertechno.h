void calcDecodeBuffer()
{
  resetBitSumer();
   writeDecode('{');
  for (int i = 0; i < decBufferPos; i++)
  {
    
    char z = decBuffer[i];
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
    state = 1;
    
  }

  switch (state)
  {
    //	 Serial.print(symbol);
    case 0:
      if (symbol == '5') state = 1;   return 0;
      break;
    case 1:
    if (decBufferPos ==4) writeBuffer(' ');
    if (decBufferPos ==9) writeBuffer(' ');
    if (decBufferPos ==12) writeBuffer(' ');
    
     
      if (symbol == 'a')

      {
        state = 2;
        return 0;
      }
      if (symbol == 'b') {
        state = 3;
        return 0;
      }
      if (symbol == '3')
      {
        state = 0;
        if (decBufferPos == 15 && (    (decBuffer[13] == 'f'  || decBuffer[13] == '0') && (decBuffer[14] == 'f'  || decBuffer[14] == '0')    ) )   flushDecodeBuffer();

        return 0;
      }
      resetBuffer();

      break;
    case 2:
      if (symbol == 'a')
      {
        state = 1;
        writeBuffer('0');
      
        return 0;
      }

      if (symbol == 'b')
      {
        state = 1;
       
        writeBuffer('f');
        return 0;
      }
      break;

    case 3:
      if (symbol == 'b')
      {
        state = 1;
        writeBuffer('1');
        return 0;
      }
      break;

    case 4:
      if (symbol == '3')
      {
        state = 0;
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
