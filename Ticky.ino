//////////////////////
// COIN TICKER v1.1
// 
//////////////////////
//
// NUTNO pridat ve spravci knihoven externi hnihovnu ArduinoJson 5.13.2 by Benoit Blanchon: 
// https://github.com/bblanchon/ArduinoJson
// 
// NUTNO pridat ve spravci knihoven externi hnihovnu U8G2 2.22.18(verze pri sestavovani) by Oliver: 
// https://github.com/olikraus/u8g2
//
/////////

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>
#include <Arduino.h>
#include <U8g2lib.h>

// NASTAVENI TICKERU >

// Rychlost serial portu pro vypisy
#define SERIAL_SPEED 9600
// Urceni Clock pinu (SCL) pro displej
#define SCL D4
// Urceni Data pinu (SDA) pro displej
#define SDA D3
// Rychlost obnovy dat ze serveru po 100ms intervalech (600*100=60000/1000=60 sekund)
#define REFRESH_TIME 600
// Nazev wifi pro pripojeni tickeru
#define WIFI_NAME "wifi_name"
// Heslo wifi
#define WIFI_PASS "wifi_password"
// Adresa API pro nacitani dat
#define API_URL "api.coinmarketcap.com"
// Konecne adresy jednotlivych men (API_URL+URIx)
#define URI1 "/v1/ticker/bitcoin/"
#define URI2 "/v1/ticker/ethereum/"
#define URI3 "/v1/ticker/ripple/"
#define URI4 "/v1/ticker/iota/"
// Definice JSON symbolu k ziskani dat
#define JSON_COIN_SYMBOL "symbol"
#define JSON_PRICE "price_usd"
#define JSON_HISTORY1 "percent_change_1h"
#define JSON_HISTORY2 "percent_change_24h"
#define JSON_HISTORY3 "percent_change_7d"
// Font pouzity pro zobrazeni ceny za menu (font knihovny U8G2)
#define COIN_FONT_NAME u8g2_font_fur20_t_symbol
// Font pouzity pro zobrazeni historie ceny men (font knihovny U8G2)
#define HISTORY_FONT_NAME u8g2_font_fub11_tf


// Knihovna obsahujici obsluznou tridu CServis
#include "CServis.h"

// Definice objektu tridy jadra CServis
CServis servis;

// Mody Tickeru -> nastavuje Ticker
// 0 = Connect mode, pripojeni k Wifi
// 1 = Normal mode, nacitani dat z netu a vypis na LCD
int iMode;


// Pocitadlo pro obnovu/nacteni dat ze serveru 
int iTime = 0;
// Urcuje ktera mena se ma zobrazit na displej
// Platne hodnoty jsou 0,1,2,3
int ShowCoin = 0;
// Urcuje ktera historie aktualni meny se ma zobrazit na displej
// Platne hodnoty jsou 0,1,2
int ShowHistory = 0;

///////////////////////////////
///////////////////////////////
void setup() 
{
  // Init konzoly
  Serial.begin(SERIAL_SPEED);
  Serial.println("\n\n* CoinTicker v1.1");
  servis.Init( WIFI_NAME, WIFI_PASS, API_URL, 
                URI1, URI2, URI3, URI4, 
                JSON_COIN_SYMBOL, JSON_PRICE, 
                JSON_HISTORY1,JSON_HISTORY2,JSON_HISTORY3 );
  Serial.println("* Init done!");
  iMode = 0;
  iTime = 0;
}

////////////////////////////////////
////////////////////////////////////
void loop() 
{
  // 0 = Connect mode
  // 1 = Normal mode
  switch( iMode )
  {
    case 0:
    {
      Serial.println("* Connect mode starting.");
      // Pokud se nelze pripojit k wifi, proved dalsi kolo...
      if ( !servis.ConnectMode(30) )
      {
        Serial.println("\n* Connection to wifi failed! We'll try again...");
        break;
      }
      Serial.println("* Connect mode done, switching to Normal mode.");
      iMode = 1;
      iTime = 0;
      servis.ShowCoin(ShowCoin,ShowHistory);
      break;
    }
    case 1:
    {
      delay(100);
      iTime += 1;
      if ( iTime >= REFRESH_TIME )
      {
        iTime = 0;
        Serial.println("\n* Start reloading data...");
        servis.RefreshData();
        Serial.println("* RELOADED!");
        servis.ShowCoin(ShowCoin,ShowHistory);

        for ( int i = 0; i < 4; i++ )
        {
          Coin coin = servis.GetCoinData(i);
          Serial.print("* Coin no.");
          Serial.print(i);
          Serial.print(" Symbol: ");
          Serial.print(coin.ID);
          Serial.print(" Price:");
          Serial.print(coin.Price);
          Serial.print(" History: [");
          Serial.print(coin.History[0]);
          Serial.print("] [");
          Serial.print(coin.History[1]);
          Serial.print("] [");
          Serial.print(coin.History[2]);
          Serial.println("]");
        }
      }
      
      break;
    }
  }
}
