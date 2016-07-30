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


#include "HighLevelProc.h"
#include "ConstWriter.h"


#include "ProtIntertechno.h"

void setIntertechnoFrequencys()
{
  setModulationType(modAM);
  setFrequency(433000000);
  setBandwidth(320000);
  setDrate(1200);
  setFhub(100);
}

void onSetProtocol(char protID)
{


  switch (protID)
  {

    case protDefault:
      setDecodeProtocol(decodeDefault);
      setEncodeProtocol(encodeDefault);
      writeDescription("default");
      break;
    case protIntertechno:

      /* set decimator configuration */
      setSymbolMargin('1', 1000, 0.33); /// High Level, 1000ys, decimator tollerance +- 1000*0.33 ys
      setSymbolMargin('2', -1000, 0.33); /// Low Level, 1000ys, decimator tollerance +- 1000*0.33 ys
      setSymbolMargin('3', 300, 0.33);
      setSymbolMargin('4', -300, 0.33);
      setSymbolRange('5', -10000, minRange, -6000 ); // decimator low level range from low 6000 ys to 14000 ys, output duration low 10000 ys

      setSequence('a', '3', '2'); //// 300 ys High 1000 ys Low
      setSequence('b', '1', '4');

      setSequence('v', 'a', 'a');
      setSequence('w', 'b', 'b');
      setSequence('x', 'a', 'b');

      /* assign protocol decode/encode function */
      setDecodeProtocol(decodeIntertechno);
      setEncodeProtocol(encodeIntertechno);
      /*set radio configuration*/

      setIntertechnoFrequencys();
      writeDescription("Intertechno");
      break;

    case protIntertechnoLong:
    
      /* set decimator configuration */
      setSymbolMargin('1', 1225, 0.33); /// High Level, 1000ys, decimator tollerance +- 1000*0.33 ys
      setSymbolMargin('2', -1225, 0.33); /// Low Level, 1000ys, decimator tollerance +- 1000*0.33 ys
      setSymbolMargin('3', 275, 0.33);
      setSymbolMargin('4', -275, 0.33);
      setSymbolRange('5', -10000, minRange, -6000); // decimator low level range from low 6000 ys to 14000 ys, output duration low 10000 ys
      setSequence('a', '3', '4'); //// 300 ys High 1000 ys Low
      setSequence('b', '3', '2');
      
      setSequence('v','b','a');
      setSequence('w','a','b');
      
      /* assign protocol decode/encode function */
      //setDecodeProtocol(decodeIntertechno);
      //setEncodeProtocol(encodeIntertechno);
      /*set radio configuration*/
      setIntertechnoFrequencys();
      writeDescription("Intertechno Long");
      break;
  }





  ////changeProtocol(protID); :ToDo
}
