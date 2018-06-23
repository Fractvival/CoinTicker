# CoinTicker
CoinTicker // Wemos D1 mini,lite,pro

CoinTicker psaný v Arduino
Zkoušeno na Wemos D1 mini Lite a Wemos D1 mini

K dosažení úspěšného přelinkování je nejprve nutné přidat do seznamu knihoven tuto následující:
- Název:ArduinoJson
- Verze:5.13.2
- Autor:Benoit Blanchon
- Git odkaz:https://github.com/bblanchon/ArduinoJson

Po přelinkování a nahrání do Wemosky se Ticker automaticky přepne do módu nastavení.
Mód nastavení promění Ticker ve wifi server, na ten se lze připojit mobilem nebo notebookem.
Dej tedy vyhledat wifi sítě a zvol následující:

Nazev:MyTicker
Heslo:121212121212

Po pripojeni na tuto wifi prejdi do prohlizece a zde otevri adresu 
IP: 192.168.4.1

POZOR! Pri zmene nazvu wifi na kterou se bude ticker pripojovat nesmi
       byt pouzito nazvu MyWifi.
       Ticker pri kazdem spusteni testuje zda-li je provedeno jeho
       nastaveni prave tim, ze zkontroluje nazev wifi pro pripojeni.

Nastaveni dat v tickeru na defaultni hodnoty lze provest v casti Setup(),
ve funkci Init() tridy CServis zmenou parametru na true. 

       servis.Init(true);

Po tomto kroku se ticker po spusteni opet prepne do modu nastaveni.

*Planuje se pridani tlacitka RESET, po jehoz stisknuti se taktez
 provede zapis defaulnich hodnot.

----------------------------------------
KDE TO AKTUALNE STOJI ?
----------------------------------------
Funkce ShowCoin a vsechny jeji nalezitosti

----------------------------------------
PLÁNY
----------------------------------------
Veliké
