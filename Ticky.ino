// COIN TICKER v1.0
// PROGMaxi software
// 
// Na serial portu probihaji vypisy danych akci o rychlosti 9600
// PROSIM dodrzet tuto rychlost a pokud pribude zmena u ktere lze
// provadet textovou kontrolu, zaved jeji vypis na konzolu

// Knihovna obsahujici obsluznou tridu CServis
#include "CServis.h"

// Definice objektu tridy jadra CServis
CServis servis;

// Mody Tickeru -> nastavuje Ticker
// 0 = Setting mode, nastaveni tickeru
// 1 = Connect mode, pripojeni k Wifi
// 2 = Normal mode, nacitani dat z netu a vypis na LCD
int iMode;


// Pocitadlo pro obnovu/nacteni dat ze serveru 
int iTime = 0;
// Pocitadlo pro obnovu displeje
int iDisplay = 0;
// Urcuje ktera mena se ma zobrazit na displej
// Platne hodnoty jsou 0,1,2,3
int ShowCoin = 0;
// Urcuje ktera historie aktualni meny se ma zobrazit na displej
// Platne hodnoty jsou 0,1,2
int ShowHistory = 0;

// Obsluha pro vypis webove stranky k nastaveni tickeru 
void handleRoot()
{
  server.send ( 200, "text/html", servis.GetStrPage() );
  Serial.println("* Client refresh page!");
}

///////////////////////////////
///////////////////////////////
void setup() 
{
  // Init konzoly
  Serial.begin(9600);
  Serial.println("\n\n* CoinTicker v1.0");
  // Init jadra tickeru
  // Argument true provede zapis defaultnich hodnot pred ctenim dat z epromky
  servis.Init(false);
  Serial.println("* Init done!");
  Serial.println("* Checking ROM data for connecting to WiFi.");
  // Kontrola zda/li je uz nastavena Wifi
  // Pokud ne, spusti se Setting mode
  // Pokud ano, spusti se mod pripojeni k Wifi
  if ( !servis.IsSet() )
  {
    Serial.println("* Wifi is NOT set! Switching to setting mode.");
    Serial.println("* Setting mode starting.");
    servis.SettingMode();
    server.on ( "/", handleRoot );
    server.begin();
    iMode = 0;
  }
  else
  {
    Serial.println("* Wifi name detecting! Switching to connect mode.");
    iMode = 1;
    iTime = 0;
  }
}

////////////////////////////////////
////////////////////////////////////
void loop() 
{
  // 0 = Setting mode
  // 1 = Connect mode
  // 2 = Normal mode
  switch( iMode )
  {
    case 0:
    {
      server.handleClient();
      // Pokud bude nejaky argument ze strany klienta vyplnen a potvrzen, provedeme jeho zapis 
      if ( server.args() )
      {
        String message;
        for ( int i = 0; i < 12; i++ )
        {
          message = server.arg(i);
          if ( message.length() > 0 )
          {
            servis.SaveData(i,message);
            Serial.print("* Setting item no.");
            Serial.print(i);
            Serial.print(" changed to: ");
            Serial.println(message);
          }
        }
        Serial.println("* ALL setting done, switching to Connect mode.");
        iMode = 1;
        iTime = 0;
      }
      break;
    }
    case 1:
    {
      Serial.println("* Connect mode starting.");
      // Pokud se nelze pripojit k wifi, proved dalsi kolo...
      if ( !servis.ConnectMode(30) )
      {
        Serial.println("\n* Connection to wifi failed! We'll try again...");
        break;
      }
      Serial.println("* Connect mode done, switching to Normal mode.");
      iMode = 2;
      iTime = 0;
      iDisplay = 10;
      break;
    }
    case 2:
    {
      delay(100);
      
      iTime += 1;
      iDisplay +=1;

      if ( iDisplay >= 10 )
      {
        iDisplay = 0;
        servis.ShowCoin(ShowCoin,ShowHistory);
      }

      
      if ( iTime >= 600 )
      {
        iTime = 0;
        Serial.println("\n* Start reloading data...");
        servis.RefreshData();
        Serial.println("* RELOADED!");

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
