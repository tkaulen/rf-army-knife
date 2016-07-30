# rf army knife (Achtung Alpha Stadium!!!)

* Ein schweizer Taschenmesser für Funkprotokolle
* plattformübergreifend
* code unabhängig vom Transceivermodul
* leistungsstarke Analysetools
* modular und erweiterbar aufgebaute Definition neuer Funkprotokolle
* getrennte Handhabung von Timingwerten und der Protokollimplementierung
* paralelle Echtzeitdekodierung von mehreren Protokollen gleichzeitig ohne weitere Codeanpassung (auf der selben Frequenz,Modulationsart e.t.c.)

rf army knife ist ein embedded Softwareframework zur einfachen Implementierung und Debugging von benutzerdefinieren Funkprotokollen. Getestet habe ich die Implementierung auf den Arduino Board's Mega2560 und DUE, in Verbindung mit einem CC1101 Transceiver(Funkmodul) von Texas Instruments. Es können natürlich sehr einfach mit einer geringfügigen Anpassung auch andere Transceivermodule eingebunden werden, da die Codebasis Hardwareunabhängig ist. Das Framework eignet sich insbesondere für Anwendungen im 433/868 mhz ISM Band und vielen weiteren Protokollen, welche eine Bi-State Modulation (am,fm) einsetzen. Darunter fallen sämtliche Funk - Steckdosen/Türklingeln/Jalosien-Schalter/Garagentoröffner/ Wetterstationen/Heizungsthermostate/Alarmanlagen/Autoschlüssel/ Garagentoröffner und viele weitere Anwendungen im Berreich drahtloser Haus- und Industrieautomation. Das Konzept ist sehr generisch gehalten, sodass mit relativ wenigen Codezeilen neue Protokolle implementiert werden können. 



##Komandozeilenbefehle  

####Informationsbefehle

**I** - Info
```
----- Availible Protocols: -----
#0 raw mode
#1 default
#2 Intertechno

----- Examples: -----
1) send Intertechno(P=2) on 433 mhz repeat 5 with code 10f0000000f0
    > P = 2 F=433000000 R=5  {10f0 0000 00 f0}

2) do the same after call first example 
    > {10f0 0000 00 f0}

3) recive intertechno on 433 mhz (default frequency)
    < P = 2 

4) Send intertechno on 433 mhz and recive after send
 <> P = 2 F=433000000 {10f00 0000 00 f0}
```

**LS** - List symbols/sequences and cunfiguration state
```

----- Symbols -----
(min:max:duration example:1=100:200:150)
[0] min:0 max:0 duration:0
[1] min:670 max:1330 duration:1000
[2] min:-1330 max:-670 duration:-1000
[3] min:201 max:399 duration:300
[4] min:-399 max:-201 duration:-300
[5] min:-14000 max:-6000 duration:-10000

----- Sequences -----
(FirstSecond example:a=12)
[a] first:3 second:2
[b] first:1 second:4

----- RF Tunings -----
[F] frequency: 433000000 hz
modulation: am
bandwidth: 320000 hz
data rate: 1200 baud
frequency hub:  hz0
radio: idle

----- other Settings -----
[D] debug mode: off
[R] repeat: 1
[P] protocol: 2
decoder: custom
encoder: custom
```

LR - List Rohdatenrekorder

#### Sende - Empfangsbefehle

**<** - Daten empfangen

**>** - Daten senden

**<>** - Daten senden, dannach empfangen

**|<** - Daten Empfangen UND Rohdatenrekorder starten.

**|>** - Daten aus dem Rohdatenrekorder senden

**<|** - Daten aus dem Rohdatenrekorder durch den Decoder schicken

**>|** - Daten vom Encoder in den Rohdatenrekorder schreiben anstatt zu senden

#### Datenbefehle

**\{**Daten 010101010....**\}** - Definiert eine Zeichenkette für die Datenausgabe. Die Syntax ist abhängig vom ausgewähltem Protokoll.

####Konfigurationsbefehle - Transceiver

***P*** - Protokoll auswählen. Syntax: P=Protokollnummer

***F*** - Setzt die Frequenz in Herz Syntax F=Frequenz in Herz

***M*** - Setzt die Modulationsart. Syntax P=am oder P=fm

***B*** - Bandbreite in Herz

***D*** - Datenrate in Baud

***Q*** - Frequenzhub in Herz

###Konfigurationsbefehle - Decimator

***0-9*** - Syntax:0=duration:minTime:maxTime - Definiert ein Symbol mit den entsprechenden Timingwerten.  Symbole werden als eine Zahl zwischen 0 und 9 definiert. Wir wollen z.B. das Symbol 0 mit einem HIGH Pegel von 150ys definieren. Außerdem soll das Symbol erkannt werden, wenn sich der Pegel zwischen 100ys HIGH  und 200ys HIGH befindet. Dazu setzen wir : 0=150:100:200. LOW Pegel werden mit einem negativem Vorzeichen gesetzt. Beispiel: 0=-150:-200:-100. Bei LOW-Definitionen ist zu beachten, das die Begrenzungswerte vertauscht werden, weil mit negative Vorzeichen umgekehrt gerechnet wird. Des weiteren ist die Variante duration:margin möglich. Margin gibt den Tolleranzbereich um den Duration Wert an. Beispiel: 0=150:50 ist das selbe wie in der Ausgangsdefinition, weil 150-50= 100 und 150+20=200.

***a-f*** - Syntax:a=firstSymbol:secondSymbol - Definiert eine Sequenz aus 2 aufeinanderfolgenden Symbolen.  Sequenzen werden mit einem Buchstaben zwischen a und f gekennzeichnet. Es ist möglich 2 aufeinanderfolgende Symbole als ein Symbol(Sequenz) zusammenzufassen. Wurde z.B. zuvor ein Symbol 0 mit 300ys High und ein Symbol 1 mit 300ys Low definiert, dann lassen sich diese beiden Symbole zu einer Sequenz zusammenfassen. Um Symbol 0 und 1 zu einer Sequenz a zusammen zu fassen, wird folgende Syntax angewendet: a=01

Das praktische an dieser Sache ist, das man später in der Protokollimplementation sehr viel Code einsparen kann, weil man nicht mehr mühsam die Flankentimings über mehrere Flanken hinweg auswerten muss. Die Protokollhandlerfunktionen bekommen später anstatt der Timingwerte das fertig ausgewertete Symbol übergeben. Da die Timings somit vom Protokollquellcode getrennt sind, lassen sich zu einem späterem Zeitpunkt ganz einfach die Timings on the fly ändern, falls z.B. ein Gerät von den Standardtimingwerten leichte Abweichungen hat.

###Konfigurationsbefehle - Weitere

***R*** - Anzahl der Wiederholungen beim Senden des Datenpaketes. Syntax: R=Anzahl der Widerholungen

***D*** - Debugmodus an/aus. Syntax: D=0 oder D=1


##Syntax und Semantik

(Informationsbefehle)

oder

(Sende & Empfangsbefehl) (Konfiguartionsbefehle) (Datenbefehle)

Info: Die Konfiguration wird beim wechseln des Protokolls (P=Protokoll Nummer) zurückgesetzt. Deswegen sollte der Protokollparameter (P=Protokoll Nummer) immer an erster Stelle (nach den optionalen Sende und Empfangsbefehlen) stehen. Dies ist notwendig, da die Protokollimplementationen beim initialisieren selber Befehle absetzen um z.B. die Sende/Empfangsfrequenz zu definieren. Dannach können sämtliche Parameter nach belieben überschreiben und abgeändert werden.  

Beispiel:
```
P = 2 [ENTER] (setzt das Protokoll mit der Protokollnummer 2. Hier Intertechno)
F = 440000000  [ENTER] (setzt die Frequenz auf 444 mhz, z.B. weil jemand auf 444 mhz dieses Protokoll temporär testen will)
> {10f0 0001 00 f0} [ENTER] (Sendet ein Datenpaket mit Hauscode,Gerätecode,Schaltparameter)
< [ENTER] (Legt sich aus die Lauer und lauscht nach Schaltbefehlen)
> P =2 F = 444000000 {10f0 0001 00 f0} [ENTER]  (das ganze kann man natürlich auch in eine Zeile schreiben) 
<> {10f0 0001 00 f0} (Sendet ein Datenpaket und lauscht anschließend)
P = 2 [ENTER]
Achtung: Ab hier geht der gesetzte Parameter 444000000 mhz verloren, weil das Protokoll neu initialisiert wurde
```
Info: Jede Eingabe über die Terminalkonsole setzt das Funkmodul in den IDLE Modus, fall es sich im RX Modus befindet. Dies ist sehr parktisch, weil man will ja nicht während der Eingabe ständig die eingehenden Datenpakete um die Ohren geschmissen bekommen. 


##Protokoll&Timings definieren in HighLevelProtocols.h

```
Bit 0: 375μs hight + 1125μs low + 375μs hight + 1125μs low
  ____              ____
_|    |____________|    |____________

Bit 1: 1125μs hight + 375μs low + 1125μs hight + 375μs low
  ____________      ____________
_|            |____|            |____

Bit F (floating): 375μs hight + 1125μs low + 1125μs hight + 375μs low
  ____              ____________
_|    |____________|            |____

Bit sync:
  ____                               
_|    |______________________//_____
```  

Dazu einfach in der Switchanweisung einen neuen Case hinzufügen und wie im Beispiel protIntertechno die Parameter setzen. Die Werte sind selbstrklärend.

```
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
  ```





##Protokoll implementieren ProtIntertechno.h

 Analog dazu eine Decode- und Encodefunktion schreiben. Diese Funktion wird zeichenweise automatisch aufgerufen.
  ```
char decodeIntertechno(char symbol, long value, char protocolID)
{
 // writeDecode(symbol);
  
//  return 0;
  if (symbol == '5')
  {
    resetBuffersAndSetState(1);
    return 0;
  }

  switch (getState())
  {
    case 0:
      break;

    case 1:
      if (symbol == 'v') writeBuffer('0');
      if (symbol == 'w') writeBuffer('1');
      if (symbol == 'x') writeBuffer('f');

      if (getTickCounter() == 4) writeBuffer(' ');
      if (getTickCounter() == 8) writeBuffer(' ');
      if (getTickCounter() == 10) writeBuffer(' ');

      if ( getTickCounter() == 11)
      {
        if (symbol == 'x' || symbol == 'v') setState(2); else  resetBuffersAndSetState(0);
      }

      return 0;
      break;

    case 2:
     if (symbol == 'v') writeBuffer('0');
     if (symbol == 'x') writeBuffer('f');
      
      if (symbol == 'x' || symbol == 'v') flushDecodeBuffer();
      resetBuffersAndSetState(0);
      break;
  }
}


char encodeIntertechno(char symbol, char protocolID)
{
  if (symbol == '0') writeEncode('v'); // send symbol A (from set sequence) 300 ys High 1000 ys Low
  if (symbol == '1') writeEncode('w');
  if (symbol == 'f')  writeEncode('x');

  if (symbol == '{')  writeEncode('5'); // start condition
  if (symbol == '}')  // end conbdition
  {
    writeEncode('3'); writeEncode('5');
  }
  return 0;
}
```

[Quelle]https://www.sweetpi.de/blog/329/ein-ueberblick-ueber-433mhz-funksteckdosen-und-deren-protokolle

Jeder ist dazu eingeladen weitere Funkprotokolle hinzuzufügen. Schickt mir die beiden Funktionen an armyknife@gmx.de. Ich baue die dann ein und Ihr werdet als Dankeschön in den Credits erwähnt.

##ToDo's
* Plugin für SDR#
* Mehrere Protokolle an the fly gleichzeitg dekodieren
* Port für Raspberry PI
* Analog Frontend (IQ Mixer,Filter,Demodulatoren)
* Ziel dieses Projektes ist es, irgendwann einmal sämtliche Funkprotokolle abzudecken, die es gibt. 




