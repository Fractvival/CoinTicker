CoinTicker psaný v Arduino

Zkoušeno na Wemos D1 mini Lite a Wemos D1 mini

Displej je použit SSD1306 s rozlišením 128x64p přes linku I2C (4pinový displej).
V příkladu se pro připojení displeje používá pinů D4(clock) a D3(data).

Příklad zapojení tlačítek pro ovládání (bez displeje):

<p align="left">
  <img width="128" height="116" src="https://github.com/Fractvival/CoinTicker/blob/master/Images/ct-layout.png">
</p>

https://www.youtube.com/watch?v=-ElJNoKtW-M


KNIHOVNY
--------

K dosažení úspěšného přelinkování je nejprve nutné přidat do seznamu knihoven tyto následující:
- Název: ArduinoJson
- Verze: 5.13.2 (Číslo verze NUTNO DODRŽET !)
- Autor: Benoit Blanchon
- Link: https://github.com/bblanchon/ArduinoJson
------------
- Název: U8G2
- Verze: 2.22.18
- Autor: Oliver
- Link: https://github.com/olikraus/u8g2

KOMPILACE
---------
Zajistete aby soubory Ticky.ino + CServis.h byly v jedne slozce

Pred linkováním a uploadem do Wemosu nutno provest nastaveni Tickeru v uvodu souboru Ticky.ino

Pro nastavení wifi do které se bude ticker připojovat se musí upravit tyto položky
  ```ruby
  #define WIFI_NAME "wifi_name"
  #define WIFI_PASS "wifi_password"
  ```
Pro nastavení vlastních měn je nejprve potřeba své měny najít v seznamu api na této adrese:
  https://api.coinmarketcap.com/v2/listings/
K vyhledání měny je potřeba znát nejlépe její mezinárodní zkratku, např.: BTC a tuto použít ve vyhledávání v prohlížeči.
Tímto se dostaneme přímo na část kde je uvedena konečná podoba adresy, která se bude měnit v nastavení tickeru některého z URI.
V našem případě nás bude zajímat hodnota ve ```"website_slug"```, u bitcoinu ```"website_slug": "bitcoin"```. 
Právě hodnotu bitcoin poté uvedeme na konec některé z URI, například takto:
  ```ruby
  #define URI1 "/v1/ticker/bitcoin/"
  ```
Je nutné před textem ```bitcoin/``` zachovat část adresy ```/v1/ticker/``` pokud ji chceme načítat z api.coinmarketcap.com.
Každé API však může mít rozdílnou podobu URI.

