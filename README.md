# CoinTicker
CoinTicker - Arduino - Wemos D1 mini,lite,pro

Pracovní verze CoinTicker psaná v Arduino
Zkoušeno na Wemos D1 mini Lite a Wemos D1 mini

Pri prvnim spusteni se Ticker automaticky prepne do modu nastaveni.
Mod nastaveni promeni Ticker ve wifi server, pripoj se tedy k teto wifi:

Nazev: MyTicker

Heslo: 121212121212

Po pripojeni na tuto wifi prejdi do prohlizece a zde otevri adresu 
IP: 192.168.4.1

POZOR! Pri zmene nazvu wifi na kterou se bude ticker pripojovat nesmi
       byt pouzito nazvu MyWifi.
       Ticker pri kazdem spusteni testuje zda-li je provedeno jeho
       nastaveni prave tim, ze zkontroluje nazev wifi pro pripojeni.
