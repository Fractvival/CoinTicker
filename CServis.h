///////////////////////////////////
// Jadro Tickeru v1.1
// * 06/2018
// 
////////////////////////

// LCD, vytvoreni objektu pro praci s displejem
// SSD1306
// 128x64
// I2C = 4pinovy displej, GND,VCC,SCL,SDA
// Hodnoty SCL(clock) a SDA(data) lze menit za cisla pinu, napr D3 a D4
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

// Struktura kryptomeny
typedef struct __Coin
{
  // Mezinarodni zkratka (symbol) meny, 
  // napr.: "BTC"
  String ID;
  // Cena za kryptomenu v penezich dle API (napr. USD)
  float Price;
  // Historie meny od nejnizsiho casoveho obdobi, 
  // napr.: 0=hodina,1=den,2=vikend  
  float History[3]; 
}Coin;


// Obsluzna trida pro spravu Tickeru
class CServis
{
  public:

    // NUTNO provest inicializaci pri setupu!
    void Init( const String & wifiName, const String & passName, const String & apiURL, const String & apiURI1, const String & apiURI2, const String & apiURI3, const String & apiURI4,
                    const String & jsonCoinSymbol, const String & jsonPrice, const String & jsonHistory1, const String & jsonHistory2, const String & jsonHistory3 );
    // Mod pripojeni tickeru k wifi
    // Argument je pocet pokusu k pripojeni po sekundach
    // Vraci true v pripade uspechu
    bool ConnectMode(const int & cAttempts);
    // Nacteni dat o menach z netu
    // Vraci true v pripade uspechu
    // Argument je cislo konecne URL API, pocitane od 0 do 3
    bool ReadDataFromSite(const int & endAPI);
    // Obnoveni dat ze serveru
    // Vrati true v pripade uspechu
    bool RefreshData(void);
    // Vraci info o mene pomoci struktury Coin
    // Argument je cislo meny pocitane od nuly
    Coin GetCoinData(const int & iCoin);
    // Upravuje ciselnou podobu do pozadovaneho tvaru vcetne tisicinek a desetinnych mist
    // Vraci upraveny retezec
    String FixCoinText(const float & Price);
    // Zobrazi aktualne zvolenou menu a jeji zvolenou historii na displeji
    void ShowCoin(const int & iCoin,const int & iHistory);
    // Uvodni obrazovku Tickeru
    void ShowIntro(const String & Text1);
    // Informace sdeleni o pripojovani
    void ShowConnect(const String & Text1);
    // Toto sdeleni je pouzito v pripade ze vypadne spojeni behem obnovy dat
    void ShowConnect(const String & Text1, const String & Text2);
    // Informace sdeleni o nacitani dat ze serveru
    void ShowReadData(const String & Text1);

  private:

    // Nazev a heslo WiFi pro ziskavani dat
    String    __ssidWifi;
    String    __passWifi;

    // URL adresa API
    String    __API;
    // Konecna URL kryptomen vcetne lomitek (__API+__EndAPI=cela URL pro ziskani dat)
    String    __EndAPI[4];
    // Informace o kryptomenach, nacitaji se automaticky z API
    Coin      __sCoin[4];

    // JSON znacky pro nacitani dat z API
    String    __Symbol;
    String    __Price;
    String    __HHour;
    String    __HDay;
    String    __HWeek;

};


Coin CServis::GetCoinData(const int & iCoin)
{
  return __sCoin[iCoin];      
}


String CServis::FixCoinText(const float & Price)
{
  String fixString = "";
  // Prevod meny i s desetinnou carkou na text
  String conString = (String)Price;
  // Touto promennou zajistime prevod z float do int, 
  // tedy prevod na cele cislo
  int dNum = Price;
  // A toto cele cislo prevedeme na text
  String dString = (String)dNum;
  // Zde se vytahneme pouze tu cast cisla za desetinnou carkou
  // Diky tomu pak muzu zjistit pocet desetinnych mist
  String decString = conString.substring(conString.indexOf(".")+1);
  // Tu jde hlavne o to, srovnat pocet desetinnych mist na 2
  // V dalsi casti teto fce se uz bude jen snizovat dle potreby.
  switch( decString.length() )
  {
    case 0:
    {
      decString += "00";
      break;
    }
    case 1:
    {
      decString += "0";
      break;
    }
  }
  // Kdyby snad nahodou bylo desetinnych mist vic jak 2, skrouhni to na 2
  if ( decString.length() > 2 )
  {
    decString = decString.substring(0,2);
  }
  // A zde se uz prideluji tisicinne tecky a desetinne mista dle potreby i v minusovych hodnotach
  ////////////////////////////
  if ( dNum >= 0 && dNum < 10 )
  {
    fixString += (String)dNum;
    fixString += ",";
    fixString += decString;
    //Serial.println("0-9");      
  }
  if ( dNum < 0 && dNum > -10 )
  {
    fixString += (String)dNum;
    fixString += ",";
    fixString += decString;
    //Serial.println("0-9");      
  }
  if ( dNum >= 10 && dNum < 100 )
  {
    fixString += (String)dNum;
    fixString += ",";
    fixString += decString;
    //Serial.println("10-99");      
  }
  if ( dNum <= -10 && dNum > -100 )
  {
    fixString += (String)dNum;
    fixString += ",";
    fixString += decString;
    //Serial.println("10-99");      
  }
  if ( dNum >= 100 && dNum < 1000 )
  {
    fixString += (String)dNum;
    fixString += ",";
    fixString += decString.substring(0,1);
    //Serial.println("100-999");      
  }
  if ( dNum <= -100 && dNum > -1000 )
  {
    fixString += (String)dNum;
    fixString += ",";
    fixString += decString.substring(0,1);
    //Serial.println("100-999");      
  }
  if ( dNum >= 1000 && dNum < 10000 )
  {
    fixString += dString.substring(0,1);
    fixString += ".";
    fixString += dString.substring(1);
    //Serial.println("1.000-9.999");      
  }
  if ( dNum <= -1000 && dNum > -10000 )
  {
    fixString += dString.substring(0,2);
    fixString += ".";
    fixString += dString.substring(2);
    //Serial.println("1.000-9.999");      
  }
  if ( dNum >= 10000 && dNum < 100000 )
  {
    fixString += dString.substring(0,2);
    fixString += ".";
    fixString += dString.substring(2);
    //Serial.println("10.000-99.000");      
  }
  if ( dNum <= -10000 && dNum > -100000 )
  {
    fixString += dString.substring(0,3);
    fixString += ".";
    fixString += dString.substring(3);
    //Serial.println("10.000-99.000");      
  }
  if ( dNum > 100000 && dNum < 1000000 )
  {
    fixString += dString.substring(0,3);
    fixString += ".";
    fixString += dString.substring(3);
    //Serial.println("100.000-999.999");      
  }
  if ( dNum <= -100000 && dNum > -1000000 )
  {
    fixString += dString.substring(0,4);
    fixString += ".";
    fixString += dString.substring(4);
    //Serial.println("100.000-999.999");      
  }
  if ( dNum >= 1000000 )
  {
    fixString += dString.substring(0,1);
    fixString += ".";
    fixString += dString.substring(1,4);
    fixString += ".";
    fixString += dString.substring(4);
    //Serial.println(">1.000.000");   
  }
  if ( dNum <= -1000000 )
  {
    fixString += dString.substring(0,2);
    fixString += ".";
    fixString += dString.substring(2,5);
    fixString += ".";
    fixString += dString.substring(5);
    //Serial.println(">1.000.000");   
  }
  return fixString;
}


void CServis::ShowIntro(const String & Text1)
{
  u8g2.clear();
  u8g2.setFont(u8g2_font_courB18_tf);
  u8g2.setFontPosCenter();
  int Width = u8g2.getDisplayWidth();
  int Height = u8g2.getDisplayHeight();
  String WelcomeMessage1 = "Coin";
  String WelcomeMessage2 = "Ticker";
  int widthText1 = (Width/3)-(u8g2.getStrWidth(WelcomeMessage1.c_str())/2);
  int widthText2 = (Width/2)-(u8g2.getStrWidth(WelcomeMessage2.c_str())/2);
  u8g2.drawStr( widthText1, 10, WelcomeMessage1.c_str() );
  u8g2.drawStr( widthText2, 30, WelcomeMessage2.c_str() ); 
  u8g2.setFont(u8g2_font_crox3h_tf);
  String WelcomeMessage3 = Text1;
  u8g2.drawStr( (Width/2)-(u8g2.getStrWidth(WelcomeMessage3.c_str())/2), 55, WelcomeMessage3.c_str() ); 
  u8g2.sendBuffer();
}

void CServis::ShowConnect(const String & Text1)
{
  u8g2.clear();
  u8g2.setFont(u8g2_font_open_iconic_www_4x_t);
  u8g2.setFontPosCenter();
  int Width = u8g2.getDisplayWidth();
  int Height = u8g2.getDisplayHeight();
  String WelcomeMessage1 = "Q";
  u8g2.drawStr( (Width/2)-(u8g2.getStrWidth(WelcomeMessage1.c_str())/2), 18, WelcomeMessage1.c_str() ); 
  u8g2.setFont(u8g2_font_crox3h_tf);
  String WelcomeMessage3 = Text1;
  u8g2.drawStr( (Width/2)-(u8g2.getStrWidth(WelcomeMessage3.c_str())/2), 55, WelcomeMessage3.c_str() ); 
  u8g2.sendBuffer();
}

void CServis::ShowReadData(const String & Text1)
{
  u8g2.clear();
  u8g2.setFont(u8g2_font_open_iconic_arrow_4x_t);
  u8g2.setFontPosCenter();
  int Width = u8g2.getDisplayWidth();
  int Height = u8g2.getDisplayHeight();
  String WelcomeMessage1 = "P";
  u8g2.drawStr( (Width/2)-(u8g2.getStrWidth(WelcomeMessage1.c_str())/2), 18, WelcomeMessage1.c_str() ); 
  u8g2.setFont(u8g2_font_crox3h_tf);
  String WelcomeMessage3 = Text1;
  u8g2.drawStr( (Width/2)-(u8g2.getStrWidth(WelcomeMessage3.c_str())/2), 55, WelcomeMessage3.c_str() ); 
  u8g2.sendBuffer();
}

void CServis::ShowConnect(const String & Text1, const String & Text2)
{
  u8g2.clear();
  u8g2.setFont(u8g2_font_open_iconic_www_2x_t);
  u8g2.setFontPosCenter();
  int Width = u8g2.getDisplayWidth();
  int Height = u8g2.getDisplayHeight();
  String WelcomeMessage1 = "Q";
  u8g2.drawStr( (Width/2)-(u8g2.getStrWidth(WelcomeMessage1.c_str())/2), 10, WelcomeMessage1.c_str() ); 
  u8g2.setFont(u8g2_font_crox3h_tf);
  String WelcomeMessage3 = Text1;
  String WelcomeMessage4 = Text2;
  u8g2.drawStr( (Width/2)-(u8g2.getStrWidth(WelcomeMessage3.c_str())/2), 35, WelcomeMessage3.c_str() ); 
  u8g2.drawStr( (Width/2)-(u8g2.getStrWidth(WelcomeMessage4.c_str())/2), 55, WelcomeMessage4.c_str() ); 
  u8g2.sendBuffer();
}


void CServis::ShowCoin(const int & iCoin,const int & iHistory)
{
  u8g2.clear();

  int Width = u8g2.getDisplayWidth();
  int Height = u8g2.getDisplayHeight();

  u8g2.setFontPosTop();
  u8g2.setFont(SYMBOL_FONT_NAME);
  u8g2.drawStr( (Width/2)-(u8g2.getStrWidth(__sCoin[iCoin].ID.c_str() )/2), ADD_Y_OFFSET_SYMBOL_FONT, __sCoin[iCoin].ID.c_str() );

  u8g2.setFont(COIN_FONT_NAME);
  String chPrice = FixCoinText(__sCoin[iCoin].Price);
  String chPrefix = "$";
  String chPostfix = "";
  String chShowPrice = "";
  chShowPrice += chPrefix;
  chShowPrice += chPrice;
  chShowPrice += chPostfix;
  
  u8g2.setFontPosCenter();
  u8g2.drawStr( (Width/2)-(u8g2.getStrWidth(chShowPrice.c_str() )/2), (Height/2)+ADD_Y_OFFSET_COIN_FONT, chShowPrice.c_str() );
  
  u8g2.setFont(HISTORY_FONT_NAME);
  String chHistory = FixCoinText(__sCoin[iCoin].History[iHistory]);
  String chHPrefix = "";
  if ( (int)__sCoin[iCoin].History[iHistory] >= 0 )
  {
    chHPrefix = "+";
  }
  String chHPostfix = "";
  if ( iHistory == 0 )
  {
    chHPostfix = "%/h";     
  }
  if ( iHistory == 1)
  {
    chHPostfix = "%/d";     
  }
  if ( iHistory == 2 )
  {
    chHPostfix = "%/w";     
  }
  String chShowHistory = "";
  chShowHistory += chHPrefix;
  chShowHistory += chHistory;
  chShowHistory += chHPostfix;
  u8g2.setFontPosBottom();
  u8g2.drawStr( (Width/2)-(u8g2.getStrWidth(chShowHistory.c_str() )/2), Height, chShowHistory.c_str() );
  
  u8g2.sendBuffer();
}

bool CServis::ReadDataFromSite(const int & endAPI)
{
  // Pomocna promenna pro ulozeni nactene stranky z netu
  String payload;
  // Vytvoreni SSL klienta pro komunikaci se serverem
  // Zabezpecene pripojeni je vetsinou vyzadovano serverem
  WiFiClientSecure client;
  // V pripade ze se na server nelze pripojit, konci neuspechem
  if (!client.connect(__API, 443)) 
  {
    Serial.print("[cli.connect() failed]");
    // Pokud pri obnove vypadne spojeni (neni wifi), vypis zpravu
    // a pote se znovu pokus o pripojeni
    ShowConnect(CONNECT_TEXT4,CONNECT_TEXT5);
    delay(5000);
    return false;
  }
  // Posleme pozadavek na server pro ziskani dat
  client.print(String("GET ") + __EndAPI[endAPI] + " HTTP/1.1\r\n" +
               "Host: " + __API + "\r\n" +
               "Connection: close\r\n\r\n");
  // Pauza pro server na zpracovani pozadavku
  delay(100);
  // Uvolnime pamet pozadavku na server
  client.flush();
  // Zjisti velikost dat pro prijem
  // Pokud data nejsou, konec a opakovat
  int sizeServerData = client.available();
  if ( sizeServerData <= 0 )
  {
    Serial.print("[Data failed]");
    return false;
  }
  // Alokace bufferu pro data
  char *bufferServerData = (char*)malloc(sizeof(char)*2048);
  // inicializace alokovane pameti
  memset( bufferServerData, 0, 2048 );
  // A ZDE data jednoduse nacteme
  client.readBytes(bufferServerData,sizeServerData);
  // A predame do bufferu pro JSON parsing
  payload = bufferServerData;
  // Uvolnime naalokovanou pamet
  free(bufferServerData);
  // Oznackujeme si json dle hranatych zavorek
  int indexLeft = payload.indexOf("[");
  int indexRight = payload.indexOf("]",indexLeft );
  // Nyni uz provedeme vytazek cisteho JSONu
  payload = payload.substring( indexLeft, indexRight+1 );

  // Vypocet velikost bufferu + 380 bajtu pro duplikaty
  const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(15) + 380;
  // Definice dynamickeho (automatickeho) bufferu
  DynamicJsonBuffer jsonBuffer(bufferSize);
  // Sprasujeme JSON z pomocne promenne
  JsonArray& root = jsonBuffer.parseArray(payload.c_str());
  // Otestujeme spravnost JSONu
  // V pripade neuspechu koncime
  if (!root.success()) 
  {
    Serial.print("[root.succes() failed]");
    return false;
  }  
  JsonObject& root_0 = root[0];
  // Nyni uz si roztridime jednotlive polozky do promennych  
  __sCoin[endAPI].ID = root_0.get<String>(__Symbol);
  __sCoin[endAPI].Price = root_0.get<float>(__Price);
  __sCoin[endAPI].History[0] = root_0.get<float>(__HHour);
  __sCoin[endAPI].History[1] = root_0.get<float>(__HDay);
  __sCoin[endAPI].History[2] = root_0.get<float>(__HWeek);
  // Koncime uspechem :)
  return true;
}


bool CServis::ConnectMode(const int & cAttempts)
{
  // Prepnuti tickeru do modu station
  WiFi.mode(WIFI_STA);
  // Vypis informaci o wifi na konzolu
  Serial.println("* Ticker switched to WIFI_STA");
  Serial.print("* Start connecting to wifi: ");
  Serial.print(__ssidWifi);
  Serial.print(" [");
  Serial.print(__passWifi);
  Serial.println("]");
  Serial.print("* Connecting.");
  // Pocitadlo neuspechu
  int cLoop = 0;
  // Zahajeni spojeni
  WiFi.begin(__ssidWifi.c_str(),__passWifi.c_str());
  // Pockame na spojeni s wifi s kontrolou neuspechu
  // Pokud spojeni neprobehne ve stanovenem poctu pokusu, ukonci smycku
  while (WiFi.status() != WL_CONNECTED) 
  { 
    delay(500);
    Serial.print(".");
    cLoop++;
    if ( cLoop >= cAttempts )
    {
      return false;
    }
  }
  // Po navazani spojeni vypis info do konzoly  
  Serial.println("OK!");
  Serial.println("* Attempt to first reading data from the server.");
  // A nyni jiz proved prvni nacteni dat ze serveru
  // Pred touto operaci zobraz info na displej
  ShowReadData(READ_TEXT1);
  delay(1000);
  for ( int i = 0; i < 4; i++ )
  {
    Serial.print("* Reading data ");
    Serial.print(i);
    Serial.print(".");
    while( !ReadDataFromSite(i) )
    {
      Serial.print(".");
      delay(500);
    }
    Serial.println("OK");
  }
  return true;
}

// Tato fce by nemela do budoucna mit info o neuspechu na displej
// Casto se stava ze data nejsou nactena na poprve, obtezovalo by to
bool CServis::RefreshData(void)
{
  for ( int i = 0; i < 4; i++ )
  {
    Serial.print("* Reading data ");
    Serial.print(i);
    Serial.print(".");
    // Provadej nacteni dat dokud nebude vse OK
    while( !ReadDataFromSite(i) )
    {
      Serial.print(".");
    }
    Serial.println("OK");
  }
  return true; 
}


void CServis::Init( const String & wifiName, const String & passName, const String & apiURL, const String & apiURI1, const String & apiURI2, const String & apiURI3, const String & apiURI4,
                    const String & jsonCoinSymbol, const String & jsonPrice, const String & jsonHistory1, const String & jsonHistory2, const String & jsonHistory3 )
{
  __ssidWifi = wifiName;
  __passWifi = passName;

  __API = apiURL;
  
  __EndAPI[0] = apiURI1;
  __EndAPI[1] = apiURI2;
  __EndAPI[2] = apiURI3;
  __EndAPI[3] = apiURI4;

  __Symbol  = jsonCoinSymbol;
  __Price   = jsonPrice;
  __HHour   = jsonHistory1;
  __HDay    = jsonHistory2;
  __HWeek   = jsonHistory3;
  
  __sCoin[0].ID = "";
  __sCoin[1].ID = "";
  __sCoin[2].ID = "";
  __sCoin[3].ID = "";

  for ( int i = 0; i < 4; i++ )
  {
    for (int j = 0; j < 3; j++ )
    {
      __sCoin[i].History[j] = 0;
    }
  }
  u8g2.begin();  
  u8g2.clear();
};

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////
