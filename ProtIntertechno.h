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

char decodeIntertechno(char symbol, long value, char protocolID)
{
  if (symbol == '4')
  {
    resetBuffersAndSetState(1);
    return 0;
  }

  switch (getState())
  {
    case 0:
      break;

    case 1:

      if (symbol == 'v' || symbol == 'w' || symbol == 'x')
      {
        if (symbol == 'v') writeBuffer('0');
        if (symbol == 'w') writeBuffer('1');
        if (symbol == 'x') writeBuffer('f');

        if (getTickCounter() == 4) writeBuffer(' ');
        if (getTickCounter() == 8) writeBuffer(' ');
        if (getTickCounter() == 10) writeBuffer(' ');

        if ( getTickCounter() == 11)
        {
          if (symbol == 'x' || symbol == 'v') setState(2); else  resetBuffersAndSetState(0); /// error
        }
      } else resetBuffersAndSetState(0); //error

      return 0;
      break;

    case 2:
      if (symbol == 'x' || symbol == 'v')
      {
        if (symbol == 'v') writeBuffer('0');
        if (symbol == 'x') writeBuffer('f');
        setState(3);
      }
      else resetBuffersAndSetState(0); //error
      break;

    case 3:  ///Sync Bit
      if (symbol == '2') flushDecodeBuffer('4'); //accept if nextSymbol = 4
      resetState(0);
      break;
  }
}


char encodeIntertechno(char symbol, char protocolID)
{
  if (symbol == '0') writeEncode('v'); // send symbol A (from set sequence) 300 ys High 1000 ys Low
  if (symbol == '1') writeEncode('w');
  if (symbol == 'f')  writeEncode('x');

  if (symbol == '{')  writeEncode('4'); // start condition
  if (symbol == '}')  // end conbdition
  {
    writeEncode('2'); writeEncode('4');
  }
  return 0;
}
