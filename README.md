# rf army knife (Achtung Alpha Stadium!!!)

* Ein schweizer Taschenmesser für Funkprotokolle

* plattformübergreifend, code unabhängig vom Transceivermodul, leistungsstarke Analysetools, modular und erweiterbar aufgebaute Definition neuer Funkprotokolle, getrennte Handhabung von Timingwerten und der Protokollimplementierung

rf army knife ist ein embedded Softwareframework zur einfachen Implementierung und Debugging von benutzerdefinieren Funkprotokollen. Getestet habe ich die Implementierung auf den Arduino Board's Mega2560 und DUE, in Verbindung mit einem CC1101 Transceiver(Funkmodul) von Texas Instruments. Es können natürlich sehr einfach mit einer geringfügigen Anpassung auch andere Transceivermodule eingebunden werden, da die Codebasis Hardwareunabhängig ist. Das Framework eignet sich insbesondere für Anwendungen im 433/868 mhz ISM Band und vielen weiteren Protokollen, welche eine Bi-State Modulation (am,fm) einsetzen. Darunter fallen sämtliche Funk - Steckdosen/Türklingeln/Jalosien-Schalter/Garagentoröffner/ Wetterstationen/Heizungsthermostate/Alarmanlagen/Autoschlüssel/ Garagentoröffner und viele weitere Anwendungen im Berreich drahtloser Haus- und Industrieautomation.  

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

**L** - List
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

#### Sende - Empfangsbefehle

**<** - Daten empfangen

**>** - Daten senden

**<>** - Daten senden, dannach empfangen

#### Datenbefehle

**\{**Daten 010101010....**\}** - Definiert eine Zeichenkette für die Datenausgabe. Die Syntax ist abhängig vom ausgewähltem Protokoll.

####Konfigurationsbefehle - Transceiver

***P*** - Protokoll auswählen. Syntax: P=Protokollnummer

***F*** - Setzt die Frequenz in Herz Syntax F=Frequenz in Herz

***M*** - Setzt die Modulationsart. Syntax P=am oder P=fm

***B*** - Bandbreite in Herz

***D*** - Datenrate in Baud

***H*** - Frequenzhub in Herz

###Konfigurationsbefehle - Decimator

***0-9*** - Syntax:0=minTime:maxTime:duration - Definiert ein Symbol mit den entsprechenden Timingwerten.  Symbole werden als eine Zahl zwischen 0 und 9 definiert. Wir wollen z.B. das Symbol 0 mit einem HIGH Pegel von 150ys definieren. Außerdem soll das Symbol erkannt werden, wenn sich der Pegel zwischen 100ys HIGH  und 200ys HIGH befindet. Dazu setzen wir : 0=100:200:150. LOW Pegel werden mit einem negativem Vorzeichen gesetzt. Beispiel: 0=-200:-100:-150. Bei LOW-Definitionen ist zu beachten, das die Begrenzungswerte vertauscht werden, weil mit negative Vorzeichen umgekehrt gerechnet wird.  

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
Achtung: Ab hier geht der gesetzte Parameter 444000000 mhz verloren, weil das Protokoll initialisiert wurde
```
Info: Jede Eingabe über die Terminalkonsole setzt das Funkmodul in den IDLE Modus, fall es sich im RX Modus befindet. Dies ist sehr parktisch, weil man will ja nicht während der Eingabe ständig die eingehenden Datenpakete um die Ohren geschmissen bekommen. 


##Protokoll definieren in HighLevelProtocols.h

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

```
#include "ProtIntertechno.h"
void onSetProtocol(char protID)
{

  switch (protocolID)
  {
    
    case protDefault:
    decodeProtocol = decodeDefault;
    encodeProtocol = encodeDefault;
    writeDescription("default");
    
    break;
    case protIntertechno:
      /* set decimator configuration */
      setSymbolMargin('1', 1000, 0.33); /// High Level, 1000ys, decimator tollerance +- 1000*0.33 ys
      setSymbolMargin('2', -1000, 0.33); /// Low Level, 1000ys, decimator tollerance +- 1000*0.33 ys
      setSymbolMargin('3', 300, 0.33); 
      setSymbolMargin('4', -300, 0.33);
      setSymbolRange('5', -14000, -6000, -10000); // decimator low level range from low 6000 ys to 14000 ys, output duration low 10000 ys
      setSequence('a', 3, 2); //// 300 ys High 1000 ys Low
      setSequence('b', 1, 4);
      /* assign protocol decode/encode function */
      decodeProtocol = decodeIntertechno;
      encodeProtocol = encodeIntertechno;
      /*set radio configuration*/
      modulationType=modAM;  
      frequency = 433000000;  
      bandwidth = 320000;
      drate = 1200;
      fhub = 0;
      writeDescription("Intertechno");
      break;
      break;
  }
  }
  ```





##Protokoll implementieren ProtIntertechno.h

  Diese Funktion wird zeichenweise automatisch aufgerufen.
  ```
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
```

[Quelle]https://www.sweetpi.de/blog/329/ein-ueberblick-ueber-433mhz-funksteckdosen-und-deren-protokolle

Jeder ist dazu eingeladen weitere Funkprotokolle hinzuzufügen. Schickt mir die beiden Funktionen an armyknife@gmx.de. Ich baue die dann ein und Ihr werdet als Dankeschön in den Credits erwähnt.



