//////////////////////
// COIN TICKER v1.2
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
#include <WiFiClientSecure.h>
#include <U8g2lib.h>

// NASTAVENI TICKERU >
// Nazev wifi pro pripojeni tickeru
#define WIFI_NAME "WIFI_name"
// Heslo wifi
#define WIFI_PASS "WIFI_password"
// Rychlost serial portu pro vypisy
#define SERIAL_SPEED 9600
// Urceni Clock pinu (SCL) pro displej
#define SCL D4
// Urceni Data pinu (SDA) pro displej
#define SDA D3
// Urceni pinu pro prepnuti meny
#define COIN_BUTTON D8
// Urceni pinu pro prepnuti historie
#define HISTORY_BUTTON D7
// Rychlost obnovy dat ze serveru po 100ms intervalech (600*100=60000/1000=60 sekund)
#define REFRESH_TIME 600
// Adresa API pro nacitani dat
#define API_URL "api.coinmarketcap.com"
// Konecne adresy jednotlivych men
#define URI1 "/v1/ticker/bitcoin/"
#define URI2 "/v1/ticker/ethereum/"
#define URI3 "/v1/ticker/binance-coin/"
#define URI4 "/v1/ticker/iota/"
// Definice JSON symbolu k ziskani dat
#define JSON_COIN_SYMBOL "symbol"
#define JSON_PRICE "price_usd"
#define JSON_HISTORY1 "percent_change_1h"
#define JSON_HISTORY2 "percent_change_24h"
#define JSON_HISTORY3 "percent_change_7d"
// Font pouzity pro zobrazeni symbolu meny
#define SYMBOL_FONT_NAME u8g2_font_crox3h_tf//u8g2_font_timB14_tf
// Font pouzity pro zobrazeni ceny za menu (font knihovny U8G2)
#define COIN_FONT_NAME u8g2_font_logisoso18_tf
// Font pouzity pro zobrazeni historie ceny men (font knihovny U8G2)
#define HISTORY_FONT_NAME u8g2_font_crox3h_tf//u8g2_font_helvB12_tf

//////////////// TEXTY
// Pripojovani k wifi
#define TEXT3 "Connecting to WIFI"
#define TEXT4 "please wait"
// Chyba pripojeni k wifi
#define TEXT5 "WIFI unavailable"
#define TEXT6 "next try.."
// Prvni nacteni dat z netu
#define TEXT7 "First reading!"
#define TEXT8 "One moment.."
//
#define PREFIX_PRICE "$"
#define POSTFIX_PRICE ""
//
#define HISTORY1 "%/h"
#define HISTORY2 "%/d"
#define HISTORY3 "%/w"
//////////////
//////////////

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

// Stav tlacitka pro zmenu meny
int buttonCoin = LOW;
// Stav tlacitka pro zmenu historie
int buttonHistory = LOW;

///////////////////////////////
///////////////////////////////
void setup() 
{
  // Zde se ukonci predchozi wifi relace
  WiFi.disconnect();
  // Nastaveni rychlosti serial portu
  Serial.begin(SERIAL_SPEED);
  Serial.println("\n\n* CoinTicker v1.2");
  // Inicializace tickeru
  servis.Init( WIFI_NAME, WIFI_PASS, API_URL, 
                URI1, URI2, URI3, URI4, 
                JSON_COIN_SYMBOL, JSON_PRICE, 
                JSON_HISTORY1,JSON_HISTORY2,JSON_HISTORY3 );
  // INTRO TEXT
  servis.ShowIntro();
  //// KONEC INTRO TEXT
  Serial.println("* Init done!");
  // Nastaveni PINu tlacitka pro zmenu meny na VSTUP
  pinMode(COIN_BUTTON, INPUT);
  Serial.print("* PIN ");
  Serial.print(COIN_BUTTON);
  Serial.println(" init OK!");
  // Nastaveni PINu tlacitka pro zmenu historie na VSTUP
  pinMode(HISTORY_BUTTON, INPUT);
  Serial.print("* PIN ");
  Serial.print(HISTORY_BUTTON);
  Serial.println(" init OK!");
  // Pri startu tickeru bude zobrazena prvni mena
  iMode = 0;
  // Pri startu tickeru bude zobrazena prvni historie
  iTime = 0;
  // mala pauza pro intro
  delay(2000);
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
      // Zobrazim info na displej o nadchazejicim pripojovani
      servis.ShowDialogInfo(281/*247*/,TEXT3,TEXT4);
      // mala pauza pro info
      delay(2000);
      Serial.println("* Connect mode starting.");
      // Pokud se nelze pripojit k wifi, proved dalsi kolo...
      // Aktualne je pouzito 30 pokusu o pripojeni pred vypsanim chyboveho hlaseni
      // Po chybovem hlaseni opet probiha dalsi kolo pokusu
      if ( !servis.ConnectMode(30) )
      {
        Serial.println("\n* Connection to wifi failed! We'll try again...");
        servis.ShowDialogInfo(267/*188*/,TEXT5,TEXT6);
        // pauza pro info
        delay(5000);
        break;
      }
      Serial.println("* Connect mode done, switching to Normal mode.");
      // Pokud je ticker pripojen, prepne se do rezimu Normal mode
      iMode = 1;
      // Vynulovani casovace
      iTime = 0;
      // Smazeme displej, nebot fce ShowCoin toto neprovadi
      u8g2.clear();
      // Prvni zobrazeni nactenych men
      servis.ShowCoin(ShowCoin,ShowHistory);
      break; // BREAK MODU 0
    } // Konec case 0
    case 1:
    {
      delay(100);
      // Kazdych 100ms zvys o 1
      iTime += 1;

      // Zde se kontroluje a obsluhuje stisk tlacitka pro zmenu kryptomeny
      buttonCoin = LOW;
      buttonCoin = digitalRead(COIN_BUTTON);
      if ( buttonCoin == HIGH )
      {
        ShowCoin++;
        if ( ShowCoin > 3 )
        {
          ShowCoin = 0;        
        }
        Serial.print("* Coin switched to ");
        Serial.println(ShowCoin);
        servis.ShowCoin(ShowCoin,ShowHistory);
      }

      // Zde se kontroluje a obsluhuje stisk tlacitka pro zmenu historie kryptomeny
      buttonHistory = LOW;
      buttonHistory = digitalRead(HISTORY_BUTTON);
      if ( buttonHistory == HIGH )
      {
        ShowHistory++;
        if ( ShowHistory > 2 )
        {
          ShowHistory = 0;        
        }
        Serial.print("* History switched to ");
        Serial.println(ShowHistory);
        servis.ShowCoin(ShowCoin,ShowHistory);
      }

      // Pokud je dosazeno casu pro obnoveni, reloadne data se serveru
      // casovac se opet vynuluje a pak vse zacne odznova
      if ( iTime >= REFRESH_TIME )
      {
        iTime = 0;
        Serial.println("\n* Start reloading data...");
        // !! Reload dat ze serveru
        servis.RefreshData();
        Serial.println("* RELOADED!");
        // Zobrazeni dat na displej
        servis.ShowCoin(ShowCoin,ShowHistory);
        // Do konzoly se vypise stav nactenych men vcetne historie
        for ( int i = 0; i < 4; i++ )
        {
          Coin coin = servis.GetCoinData(i);
          Serial.print("* Coin #");
          Serial.print(i);
          Serial.print(" |>Symbol: ");
          Serial.print(coin.ID);
          Serial.print(" |>Price:");
          Serial.print(coin.Price);
          Serial.print(" |>History: H[");
          Serial.print(coin.History[0]);
          Serial.print("] D[");
          Serial.print(coin.History[1]);
          Serial.print("] W[");
          Serial.print(coin.History[2]);
          Serial.println("]");
        }
      }
      break; // BREAK modu 1
    } // Konec case 1
  } // Konec smycky switch
} // Konec funkce loop()

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
