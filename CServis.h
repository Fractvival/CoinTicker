///////////////////////////////////
// Jadro Tickeru v1.0
// * 04/2018
//
// NUTNO pridat ve spravci knihoven externi hnihovnu ArduinoJson 5.13.2 by Benoit Blanchon: 
// https://github.com/bblanchon/ArduinoJson
//
/////////

#include <EEPROM.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClientSecure.h>

// Velikost ROM v bajtech
#define ROM_SIZE 512

// Objekt pro obsluhu serveru na tickeru
ESP8266WebServer server( 80 );
// IP adresa serveru na tickeru
IPAddress myIP;


// Struktura kryptomeny
typedef struct __Coin
{
  // Mezinarodni zkratka (symbol) meny, 
  // napr.: "BTC"
  String ID;
  // Cena za kryptomenu v penezich dle API (napr. USD)
  int Price;
  // Historie meny od nejnizsiho casoveho obdobi, 
  // napr.: 0=hodina,1=den,2=vikend  
  int History[3]; 
  
}Coin;


// Obsluzna trida pro spravu Tickeru
class CServis
{
  public:

    // NUTNO provest inicializaci pri setupu!
    void Init(bool SaveEprom);
    // TRUE pokud nazev wifi neni v tovarnim nastaveni
    bool IsSet();
    // Mod nastaveni tickeru, vraci cislo modu do ktereho ma byt ticker prepnuty
    void SettingMode();
    // Vrati textovou podobu webove stranky pro nastaveni tickeru
    String GetStrPage();
    // Uklada uzivatelem nove nebo zmenene data
    // cArg je cislo textoveho pole, pocitano od 0
    // cArg 0 = Prvni editacni pole na strance nastaveni
    void SaveData(int cArg, String Data);
    // Mod pripojeni tickeru k wifi
    // Argument je pocet pokusu k pripojeni po sekundach
    // Vraci TRUE v pripade uspechu
    bool ConnectMode(int cAttempts);
    // Nacteni dat o menach z netu
    // Vraci TRUE v pripade uspechu
    // Argument je cislo konecne URL API, pocitane od 0 do 3 (EndAPI=4)
    bool ReadDataFromSite(int endAPI);


  private:
    // Provede smazani nastaveni
    void ClearEprom();
    // Nacterni vsech nastaveni
    void ReadEprom();
    // Zapis vsech nastaveni
    void WriteEprom();

    // Cteni konkretnich dat z epromky
    String ReadEprom(int zbAddress, int iSize);
    // Vymaz konkretni casti epromky
    void ClearEprom(int zbAddress, int iSize );
    // Zapis dat do konkretni casti epromky
    bool WriteEprom(int zbAddress, int iSize, const String & Data);

    // Nazev a heslo WiFi pro ziskavani dat
    String    __ssidWifi;
    String    __passWifi;

    // Nazev a heslo WiFi Tickeru
    String    __ssidTicker;
    String    __passTicker;

    // URL adresa API
    String    __API;
    // Konecna URL kryptomen vcetne lomitek (__API+__EndAPI=cela URL pro ziskani dat)
    String    __EndAPI[4];
    // Zkratky (symboly) kryptomen, nacitaji se automaticky z API
    Coin      __sCoin[4];

    // JSON znacky pro nacitani dat z API
    String    __Symbol;
    String    __Price;
    String    __HHour;
    String    __HDay;
    String    __HWeek;

    // Obsah webove stranky pro nastaveni WiFi tickeru
    String    __strPage ="<HTML> <HEAD> <META NAME=\"GENERATOR\" CONTENT=\"PROGMaxi software\"> <META NAME=\"AUTHOR\" CONTENT=\"PROGMaxi software\"> <META HTTP-EQUIV=\"CONTENT-TYPE\" CONTENT=\"text/html; charset=Windows-1250\"> <TITLE>Ticker Settings</TITLE> </HEAD> <BODY TEXT=\"#000000\" LINK=\"#0000FF\" ALINK=\"#FF0000\" VLINK=\"#0000FF\" BGCOLOR=\"#C0C0C0\"> <H3 ALIGN=\"LEFT\">Welcome to Ticker settings</H3> <H4 ALIGN=\"LEFT\"><I></I><FONT COLOR=\"#FF0000\">*unchanged fields leaves blank</FONT></I></H4> <FORM METHOD=\"GET\"> <LABEL>SSID WIFI</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"ssid\" MAXLENGTH=32 SIZE=35><BR> <LABEL>PASSWORD WIFI</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"pass\" MAXLENGTH=32 SIZE=35><BR> <BR> <LABEL>API URL (e.g.: api.coinmarketcap.com)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"apiurl\" MAXLENGTH=62 SIZE=65><BR> <BR> <H4 ALIGN=\"LEFT\"><I></I><FONT COLOR=\"#FF0000\">**do not forget to attach the necessary JSON symbols</FONT></I></H4> <LABEL>COIN JSON SYMBOL<BR> (e.g.: symbol)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"coinsymbol\" MAXLENGTH=24 SIZE=27><BR> <LABEL>PRICE JSON SYMBOL<BR> (e.g.: price or price_usd)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"pricesymbol\" MAXLENGTH=24 SIZE=27><BR> <LABEL>HISTORY HOUR JSON SYMBOL<BR> (e.g.: percent_change_1h)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"hoursymbol\" MAXLENGTH=24 SIZE=27><BR> <LABEL>HISTORY DAY JSON SYMBOL<BR> (e.g.: percent_change_24h)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"daysymbol\" MAXLENGTH=24 SIZE=27><BR> <LABEL>HISTORY WEEK JSON SYMBOL<BR> (e.g.: percent_change_7d)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"weeksymbol\" MAXLENGTH=24 SIZE=27><BR> <BR> <LABEL>END URL FOR COIN 1<BR> (e.g.: /v1/ticker/bitcoin/)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"endurl1\" MAXLENGTH=62 SIZE=65><BR> <LABEL>END URL FOR COIN 2<BR> (e.g.: /v1/ticker/ethereum/)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"endurl2\" MAXLENGTH=62 SIZE=65><BR> <LABEL>END URL FOR COIN 3<BR> (e.g.: /v1/ticker/ripple/)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"endurl3\" MAXLENGTH=62 SIZE=65><BR> <LABEL>END URL FOR COIN 4<BR> (e.g.: /v1/ticker/iota/)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"endurl4\" MAXLENGTH=62 SIZE=65><BR> <BR> <INPUT TYPE=\"SUBMIT\" NAME=\"END\" VALUE=\"Save&Restart\" SIZE=15> <BR> <BR> </FORM> </BODY> </HTML>";

};


String CServis::GetStrPage()
{
  return __strPage;
}

bool CServis::ReadDataFromSite(int endAPI)
{
  // Vypocet velikost bufferu + 380 bajtu pro duplikaty
  const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(15) + 380;
  // Definice dynamickeho (automatickeho) bufferu
  DynamicJsonBuffer jsonBuffer(bufferSize);
  //**StaticJsonBuffer<bufferSize> jsonBuffer;
  // Pomocna promenna pro ulozeni nactene stranky z netu
  String payload;
  // Vytvoreni SSL klienta pro komunikaci se serverem
  // Zabezpecene pripojeni je vetsinou vyzadovano serverem
  WiFiClientSecure client;
  // V pripade ze se na server nelze pripojit, konci neuspechem
  if (!client.connect(__API, 443)) 
  {
    Serial.print("[client.connect() failed] ");
    return false;
  }
  // Posleme pozadavek na server pro ziskani dat
  client.print(String("GET ") + __EndAPI[endAPI] + " HTTP/1.1\r\n" +
               "Host: " + __API + "\r\n" +
               "Connection: close\r\n\r\n");
  // Pouza pro server na spracovani pozadavku
  delay(100);
  // V pripade dostupnosti dat je nacteme radek po radku
  while ( client.available() )
  {
    payload += client.readStringUntil('\n');
  }
  // Zastavime pripojeni
  client.stop();
  // Z nactene stranky vytahneme cisty JSON vcetne hranatych zavorek
  // Pokud se hranate zavorky nenachazi, skonci neuspechem
  // Toto lze do budoucna lepe osetrit
  int indexLeft = payload.indexOf("[");
  if ( indexLeft == -1 )
  {
    Serial.print("[indexLeft failed] ");
    return false;
  }
  int indexRight = payload.indexOf("]",indexLeft );
  if ( indexRight == -1 )
  {
    Serial.print("[indexRight failed] ");
    return false;
  }
  // Nyni uz provedeme vytazek cisteho JSONu
  payload = payload.substring( indexLeft, indexRight+1 );
  JsonArray& root = jsonBuffer.parseArray(payload.c_str());
  // Otestujeme spravnost JSONu
  // V pripade neuspechu koncime
  if (!root.success()) 
  {
    Serial.print("[root.succes() failed] ");
    return false;
  }  
  JsonObject& root_0 = root[0];
  // Nyni uz si roztridime jednotlive polozky do promennych  
  __sCoin[endAPI].ID = root_0.get<String>(__Symbol);
  __sCoin[endAPI].Price = root_0.get<int>(__Price);
  __sCoin[endAPI].History[0] = root_0.get<int>(__HHour);
  __sCoin[endAPI].History[1] = root_0.get<int>(__HDay);
  __sCoin[endAPI].History[2] = root_0.get<int>(__HWeek);
  // Koncime uspechem :)
  return true;
}


bool CServis::ConnectMode(int cAttempts)
{
  WiFi.mode(WIFI_STA);
  Serial.println("* Ticker switched to WIFI_STA");
  Serial.print("* Start connecting to wifi: ");
  Serial.print(__ssidWifi);
  Serial.print(" [");
  Serial.print(__passWifi);
  Serial.println("]");

  Serial.print("* Connecting.");
  int cLoop = 0;

  WiFi.begin(__ssidWifi.c_str(),__passWifi.c_str());
  while (WiFi.status() != WL_CONNECTED) 
  { 
    delay(1000);
    Serial.print(".");
    cLoop++;
    if ( cLoop >= cAttempts )
    {
      return false;
    }
  } 

  Serial.println("\n* Connected.");
  Serial.println("* Now, attempt to first retrieve data from the site.");

  int Status = 0;

  while ( Status != 4 )
  {
    for ( int i = 0; i < 4; i++ )
    {
      Serial.print("* Reading data ");
      Serial.print(i);
      Serial.print("...");
      if ( ReadDataFromSite(i) )
      {
        Status++;
        Serial.println("OK");
      }
      else
      {
        Serial.println("FAIL");
        break;
      }
      delay(1000);
    }
    if ( Status == 4 )
    {
      Serial.println("* ALL reading success.");
    }
    else
    {
      Serial.println("* Reading FAILED, try again...");
    }
  }
  
  return true;
}


void CServis::SettingMode()
{
  WiFi.mode(WIFI_AP);
  Serial.println("* Ticker switched to WIFI_AP");
  WiFi.softAP(__ssidTicker.c_str(),__passTicker.c_str());
  IPAddress myIP = WiFi.softAPIP();  
  Serial.print("* Ticker IP: ");
  Serial.println(myIP);
  Serial.print("* Ticker name: ");
  Serial.println(__ssidTicker);
  Serial.print("* Ticker pass: ");
  Serial.println(__passTicker);
}


bool CServis::IsSet()
{
  if ( __ssidWifi.equals("MyWifi") )
  {
    return false;     
  }
  return true;
}

void CServis::ClearEprom()
{
  EEPROM.begin(ROM_SIZE);
  for ( int i = 0; i < ROM_SIZE; i++ )
  {
    EEPROM.write(i,0);    
  }
  EEPROM.commit();
  EEPROM.end();
}

void CServis::ClearEprom(int zbAddress, int iSize)
{
  EEPROM.begin(ROM_SIZE);
  for ( int i = 0; i < iSize; i++ )
  {
    EEPROM.write(zbAddress+i,0);    
  }
  EEPROM.commit();
  EEPROM.end();
}

String CServis::ReadEprom(int zbAddress, int iSize)
{
  EEPROM.begin(ROM_SIZE);
  String Data = "";
  for ( int i = 0; i < iSize; i++ )
  {
    char chRead = (char)EEPROM.read(zbAddress+i);
    if ( chRead != '\0' )
    {
      Data += chRead;
    }    
  }
  EEPROM.end();
  return Data;
}

// Tato fce nacte data ulozene v epromce
void CServis::ReadEprom()
{
  __ssidWifi = ReadEprom(0,32);
  __passWifi = ReadEprom(32,32);
  __API = ReadEprom(64,62);
  __EndAPI[0] = ReadEprom(126,62);
  __EndAPI[1] = ReadEprom(188,62);
  __EndAPI[2] = ReadEprom(250,62);
  __EndAPI[3] = ReadEprom(312,62);
  __Symbol = ReadEprom(374,24);
  __Price = ReadEprom(398,24);
  __HHour = ReadEprom(422,24);
  __HDay = ReadEprom(446,24);
  __HWeek = ReadEprom(470,24);
}


bool CServis::WriteEprom(int zbAddress, int iSize, const String & Data)
{
  if ( Data.length() == 0 )
  {
    return false;
  }
  ClearEprom(zbAddress,iSize);
  EEPROM.begin(ROM_SIZE);
  for ( int i = 0; i < Data.length(); i++ )
  {
    EEPROM.write(zbAddress+i,Data[i]);
  }
  EEPROM.commit();
  EEPROM.end();
  return true;
}


void CServis::WriteEprom()
{
  WriteEprom(0,32,__ssidWifi);
  WriteEprom(32,32,__passWifi);
  WriteEprom(64,62,__API);
  WriteEprom(126,62,__EndAPI[0]);
  WriteEprom(188,62,__EndAPI[1]);
  WriteEprom(250,62,__EndAPI[2]);
  WriteEprom(312,62,__EndAPI[3]);
  WriteEprom(374,24,__Symbol);
  WriteEprom(398,24,__Price);
  WriteEprom(422,24,__HHour);
  WriteEprom(446,24,__HDay);
  WriteEprom(470,24,__HWeek);
}

void CServis::SaveData(int cArg, String Data)
{
  switch( cArg )
  {
    case 0:
    {
      __ssidWifi = Data;
      break;
    }
    case 1:
    {
      __passWifi = Data;
      break;
    }
    case 2:
    {
      __API = Data;
      break;
    }
    case 8:
    {
      __EndAPI[0] = Data;
      break;
    }
    case 9:
    {
      __EndAPI[1] = Data;
      break;
    }
    case 10:
    {
      __EndAPI[2] = Data;
      break;
    }
    case 11:
    {
      __EndAPI[3] = Data;
      break;
    }
    case 3:
    {
      __Symbol = Data;
      break;
    }
    case 4:
    {
      __Price = Data;
      break;
    }
    case 5:
    {
      __HHour = Data;
      break;
    }
    case 6:
    {
      __HDay = Data;
      break;
    }
    case 7:
    {
      __HWeek = Data;
      break;
    }
  }
  WriteEprom();
}



void CServis::Init( bool SaveEprom )
{
  __ssidWifi = "MyWifi";
  __passWifi = "121212121212";

  __ssidTicker = "MyTicker";
  __passTicker = "121212121212";
  
  __API = "api.coinmarketcap.com";
  
  __EndAPI[0] = "/v1/ticker/bitcoin/";
  __EndAPI[1] = "/v1/ticker/ethereum/";
  __EndAPI[2] = "/v1/ticker/ripple/";
  __EndAPI[3] = "/v1/ticker/iota/";

  __Symbol = "symbol";
  __Price = "price_usd";
  __HHour = "percent_change_1h";
  __HDay = "percent_change_24h";
  __HWeek = "percent_change_7d";
  
  __sCoin[0].ID = "BTC";
  __sCoin[1].ID = "ETH";
  __sCoin[2].ID = "XRP";
  __sCoin[3].ID = "MIOTA";

  for ( int i = 0; i < 4; i++ )
  {
    for (int j = 0; j < 3; j++ )
    {
      __sCoin[i].History[j] = 0;
    }
  }
  if ( SaveEprom )
  {
    Serial.println("* WRITE DEFAULT VALUES TO EPROM !");
    WriteEprom();
  }
  ReadEprom();
};



