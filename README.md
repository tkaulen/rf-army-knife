# rf army knife 

* Ein schweizer Taschenmesser für Funkprotokolle

* plattformübergreifend, unabhängig vom Transceivermodul, leistungsstarke Analysetools, modular und erweiterbar aufgebaute Definition neuer Funkprotokolle



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

#### Sende - Empfangsbehle

**<** - Daten empfangen

**>** - Daten senden

**<>** - Daten senden, dannach empfangen

#### Datenbefehle

**\{**Daten 010101010....**\}** - Definiert eine Zeichenkette für die Datenausgabe. Die Syntax ist abhängig vom ausgewältem Protokoll.

####Konfigurationsbefehle - Transceiver

***P*** - Protokoll auswählen. Syntax: P=Protokollnummer

***F*** - Setzt die Frequenz in Herz Syntax F=Frequenz in Herz

***M*** - Setzt die Modulationsart. Syntax P=am oder P=fm

***B*** - Bandbreite in Herz

***D*** - Datenrate in Baud

***H*** - Frequenzhub in Herz

###Konfigurationsbefehle - Decimator

***0-9*** - Setzt ein Symbol mit den entsprechenden Timingwerten. Syntax:0=minTime:maxTime:duration. z.B. setze einen HIGH Pegel mit 150 ys beim senden. Das Symbol wird erkannt, wenn sich der Pegel zwischen 100 und 200 ys befindet 0=100:200:150. LOW Pegel werden mit einem negativem Vorzeichen gesetzt. z.B. 0=-200:-100:-150. Bei LOW-Definitionen ist zu beachten, das die Begrenzungswerte vertauscht werden, weil negative Vorzeichen umgekehrt gerechnet wird.  

####Konfigurationsbefehle - Weitere

***R*** - Anzahl der Wiederholungen beim Senden des Datenpaketes. Syntax: R=Anzahl der Widerholungen

***D*** - Debugmodus an/aus. Syntax: D=0 oder D=1


##Syntax und Semantik

(Informationsbefehle)

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






