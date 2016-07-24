# rf army knife 

* *Ein schweizer Taschenmesser für Funkprotokolle* *

* *plattformübergreifend, unabhängig vom Transceivermodul, leistungsstarke Analysetools, modular und erweiterbar aufgebaute Definition neuer Funkprotokolle* *



##Komandozeilenbefehle  

####Informationsbefehle

**H** - Hilfe
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

#### Sende - Empfangsbehle

**<** - Daten empfangen

**>** - Daten senden

**<>** - Daten senden, dannach empfangen

#### Datenbefehle

**\{**Daten 010101010....**\}** - Definiert eine Zeichenkette für die Datenausgabe. Die Syntax ist abhängig vom ausgewältem Protokoll.

#### Konfigurationsbefehle

***P*** - Protokoll auswählen. Syntax: P=Protokollnummer

***R*** - Anzahl der Wiederholungen beim Senden des Datenpaketes. Syntax: R=Anzahl der Widerholungen

***D*** - Debugmodus an/aus. Syntax: D=0 oder D=1

***F*** - Setzt die Frequenz in Herz Syntax F=Frequenz in Herz

***M*** - Setzt die Modulationsart. Syntax P=am oder P=fm

***B*** - Bandbreite in Herz

***D*** - Datenrate in Baud

***H*** - Frequenzhub in Herz






