#include "HighLevelProc.h"
#include "ConstWriter.h"


#include "ProtIntertechno.h"
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
      setSymbolRange('5',-10000, -14000, -6000 ); // decimator low level range from low 6000 ys to 14000 ys, output duration low 10000 ys
      setSequence('a', '3', '2'); //// 300 ys High 1000 ys Low
      setSequence('b', '1', '4');
      /* assign protocol decode/encode function */
      setDecodeProtocol(decodeIntertechno);
      setEncodeProtocol(encodeIntertechno);
      /*set radio configuration*/
      setModulationType(modAM);
      setFrequency(433000000);
      setBandwidth(320000);
      setDrate(1200);
      setFhub(100);
      writeDescription("Intertechno");


      break;

    case protIntertechnoLong:
      /* set decimator configuration */
      setSymbolMargin('1', 1225, 0.33); /// High Level, 1000ys, decimator tollerance +- 1000*0.33 ys
      setSymbolMargin('2', -1225, 0.33); /// Low Level, 1000ys, decimator tollerance +- 1000*0.33 ys
      setSymbolMargin('3', 275, 0.33);
      setSymbolMargin('4', -275, 0.33);
      setSymbolRange('5',-10000, -14000, -6000); // decimator low level range from low 6000 ys to 14000 ys, output duration low 10000 ys
      setSequence('a', 3, 4); //// 300 ys High 1000 ys Low
      setSequence('b', 3, 2);
      /* assign protocol decode/encode function */
      //setDecodeProtocol(decodeIntertechno);
      //setEncodeProtocol(encodeIntertechno);
      /*set radio configuration*/
      setModulationType(modAM);
      setFrequency(433000000);
      setBandwidth(320000);
      setDrate(1200);
      setFhub(100);
      writeDescription("Intertechno Long");


      break;
  }

  ////changeProtocol(protID); :ToDo
}
