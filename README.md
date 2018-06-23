# CoinTicker
CoinTicker // Wemos D1 mini,lite,pro

CoinTicker psaný v Arduino
Zkoušeno na Wemos D1 mini Lite a Wemos D1 mini

K dosažení úspěšného přelinkování je nejprve nutné přidat do seznamu knihoven tuto následující:
- Název: ArduinoJson
- Verze: 5.13.2
- Autor: Benoit Blanchon
- Link: https://github.com/bblanchon/ArduinoJson

Číslo verze NUTNO DODRŽET !

Po přelinkování a nahrání do Wemosky se Ticker automaticky přepne do módu nastavení.
Mód nastavení promění Ticker ve wifi server, na ten se lze připojit mobilem nebo notebookem.
Dej tedy vyhledat wifi sítě a zvol následující:

- Nazev:MyTicker
- Heslo:121212121212

Po připojení na tuto wifi si otevři prohlížeč a do adresního řádku napiš následující IP a potvrď:

- IP: 192.168.4.1

Nyní se otevře stránka s nastavením Tickeru.
Pro začátek stačí uvést první dva parametry k nastavení wifi na kterou se bude Ticker připojovat a
přes kterou bude komunikovat se serverem pro získávání dat.

POZOR! Při změně názvu wifi na kterou se bude ticker připojovat nesmí
       být použito názvu "MyWifi".
       Ticker při každém spustění testuje, zda-li je provedeno jeho
       nastaveni právě tím, že zkontroluje název wifi pro připojování, a to je defaultně "MyWifi".

Nyní klinkout na tlačítko Save&Reset které je umístěno ve spodní části stránky a je hotovo.

Pokud je to nutné, nastaveni dat v tickeru na defaultni hodnoty lze provest v části Setup(),
ve funkci Init() třídy CServis změnou parametru na true. 
- servis.Init(true);
Po tomto kroku se ticker po spusteni opet prepne do modu nastaveni.

*Planuje se pridani tlacitka RESET, po jehoz stisknuti se taktez
 provede zapis defaulnich hodnot.

----------------------------------------
KDE TO AKTUALNE STOJI ?
----------------------------------------
LCD výstupy
*Funkce ShowCoin a vsechny jeji nalezitosti*

----------------------------------------
PLÁNY
----------------------------------------
Veliké
