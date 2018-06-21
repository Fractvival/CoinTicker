///////////////////////////////////
// Jadro Tickeru v1.0
// * 04/2018
//
#include <EEPROM.h>

// Velikost ROM v bajtech
#define ROM_SIZE 512

// Objekt pro obsluhu servere
ESP8266WebServer server( 80 );
// IP adresa serveru
IPAddress myIP;


// Struktura kryptomeny
typedef struct __Coin
{
  // Mezinarodni zkratka (symbol) meny, 
  // napr.: "BTC"
  String ID;
  // Historie meny od nejnizsiho casoveho obdobi, 
  // napr.: 0=hodina,1=den,2=vikend  
  int History[3]; 
  
}Coin;


// Obsluzna trida pro spravu Tickeru
class CServis
{
  public:

    // NUTNO provest inicializaci pri setupu!
    void Init();
    // TRUE pokud nazev wifi neni v tovarnim nastaveni
    bool IsSet();
    // Mod nastaveni tickeru, vraci cislo modu do ktereho ma byt ticker prepnuty
    void SettingMode();
    String GetStrPage();
    void CServis::SaveData(int cArg, String Data);


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

    // Obsah webove stranky pro nastaveni WiFi parametru
    String    __strPage ="<HTML> <HEAD> <META NAME=\"GENERATOR\" CONTENT=\"PROGMaxi software\"> <META NAME=\"AUTHOR\" CONTENT=\"PROGMaxi software\"> <META HTTP-EQUIV=\"CONTENT-TYPE\" CONTENT=\"text/html; charset=Windows-1250\"> <TITLE>Ticker Settings</TITLE> </HEAD> <BODY TEXT=\"#000000\" LINK=\"#0000FF\" ALINK=\"#FF0000\" VLINK=\"#0000FF\" BGCOLOR=\"#C0C0C0\"> <H3 ALIGN=\"LEFT\">Welcome to Ticker settings</H3> <H4 ALIGN=\"LEFT\"><I></I><FONT COLOR=\"#FF0000\">*unchanged fields leaves blank</FONT></I></H4> <FORM METHOD=\"GET\"> <LABEL>SSID WIFI</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"ssid\" MAXLENGTH=32 SIZE=35><BR> <LABEL>PASSWORD WIFI</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"pass\" MAXLENGTH=32 SIZE=35><BR> <BR> <LABEL>API URL (e.g.: api.coinmarketcap.com)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"apiurl\" MAXLENGTH=62 SIZE=65><BR> <BR> <H4 ALIGN=\"LEFT\"><I></I><FONT COLOR=\"#FF0000\">**do not forget to attach the necessary JSON symbols</FONT></I></H4> <LABEL>COIN JSON SYMBOL<BR> (e.g.: \"symbol\")</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"coinsymbol\" MAXLENGTH=24 SIZE=27><BR> <LABEL>PRICE JSON SYMBOL<BR> (e.g.: \"price\" or \"price_usd\")</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"pricesymbol\" MAXLENGTH=24 SIZE=27><BR> <LABEL>HISTORY HOUR JSON SYMBOL<BR> (e.g.: \"percent_change_1h\")</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"hoursymbol\" MAXLENGTH=24 SIZE=27><BR> <LABEL>HISTORY DAY JSON SYMBOL<BR> (e.g.: \"percent_change_24h\")</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"daysymbol\" MAXLENGTH=24 SIZE=27><BR> <LABEL>HISTORY WEEK JSON SYMBOL<BR> (e.g.: \"percent_change_7d\")</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"weeksymbol\" MAXLENGTH=24 SIZE=27><BR> <BR> <LABEL>END URL FOR COIN 1<BR> (e.g.: /v1/ticker/bitcoin/)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"endurl1\" MAXLENGTH=62 SIZE=65><BR> <LABEL>END URL FOR COIN 2<BR> (e.g.: /v1/ticker/ethereum/)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"endurl2\" MAXLENGTH=62 SIZE=65><BR> <LABEL>END URL FOR COIN 3<BR> (e.g.: /v1/ticker/ripple/)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"endurl3\" MAXLENGTH=62 SIZE=65><BR> <LABEL>END URL FOR COIN 4<BR> (e.g.: /v1/ticker/iota/)</LABEL><BR> <INPUT TYPE=\"TEXT\" NAME=\"endurl4\" MAXLENGTH=62 SIZE=65><BR> <BR> <INPUT TYPE=\"SUBMIT\" NAME=\"END\" VALUE=\"Save&Restart\" SIZE=15> <BR> <BR> </FORM> </BODY> </HTML>";

};


String CServis::GetStrPage()
{
  return __strPage;
}

void CServis::SettingMode()
{
  WiFi.mode(WIFI_AP_STA);
  Serial.println("[S] Ticker switched to WIFI_AP_STA");
  WiFi.softAP(__ssidTicker.c_str(),__passTicker.c_str());
  IPAddress myIP = WiFi.softAPIP();  
  Serial.print("[S] Ticker IP: ");
  Serial.println(myIP);
  Serial.print("[S] Ticker name: ");
  Serial.println(__ssidTicker);
  Serial.print("[S] Ticker pass: ");
  Serial.println(__passTicker);
}


bool CServis::IsSet()
{
  if ( __ssidWifi.equals("MyWifi") && __passWifi.equals("121212121212") )
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
// Zakomentovana cast lze pouzit pro overeni nactenych dat
void CServis::ReadEprom()
{
  String buffWifiName = ReadEprom(0,32);
  String buffWifiPass = ReadEprom(32,32);
  String buffAPIURL = ReadEprom(64,62);
  String buffEndURL1 = ReadEprom(126,62);
  String buffEndURL2 = ReadEprom(188,62);
  String buffEndURL3 = ReadEprom(250,62);
  String buffEndURL4 = ReadEprom(312,62);
  String buffSymbol = ReadEprom(374,24);
  String buffPrice = ReadEprom(398,24);
  String buffHHour = ReadEprom(422,24);
  String buffHDay = ReadEprom(446,24);
  String buffHWeek = ReadEprom(470,24);
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
      WriteEprom(64,62,__API);
      break;
    }
    case 3:
    {
      WriteEprom(126,62,__EndAPI[0]);
      break;
    }
    case 4:
    {
      WriteEprom(188,62,__EndAPI[1]);
      break;
    }
    case 5:
    {
      WriteEprom(250,62,__EndAPI[2]);
      break;
    }
    case 6:
    {
      WriteEprom(312,62,__EndAPI[3]);
      break;
    }
    case 7:
    {
      WriteEprom(374,24,__Symbol);
      break;
    }
    case 8:
    {
      WriteEprom(398,24,__Price);
      break;
    }
    case 9:
    {
      WriteEprom(422,24,__HHour);
      break;
    }
    case 10:
    {
      WriteEprom(446,24,__HDay);
      break;
    }
    case 11:
    {
      WriteEprom(470,24,__HWeek);
      break;
    }
  }
}



void CServis::Init()
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

  __Symbol = "\"symbol\"";
  __Price = "\"price_usd\"";
  __HHour = "\"percent_change_1h\"";
  __HDay = "\"percent_change_24h\"";
  __HWeek = "\"percent_change_7d\"";
  
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
  
  ReadEprom();
  //WriteEprom();
};



