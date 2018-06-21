//#include <WiFi.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "CServis.h"

CServis servis;
int iMode;


void handleRoot()
{
  server.send ( 200, "text/html", servis.GetStrPage() );
  Serial.println(">> client refresh page!");
}


void setup() 
{
  Serial.begin(115200);
  Serial.println("\n\n* CoinTicker v1.0");
  servis.Init();
  Serial.println("* Init done!");
  Serial.println("* Checking ROM data for connecting to WiFi.");

  if ( !servis.IsSet() )
  {
    Serial.println("* Wifi is not set! Switching to setting mode.");
    Serial.println("* Setting mode starting.");
    servis.SettingMode();
    server.on ( "/", handleRoot );
    server.begin();
    iMode = 0;
  }
  else
  {
    Serial.println("* Wifi is set, start connecting to wifi.");
    iMode = 1;
  }
}

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
      if ( server.args() )
      {
        String message;
        message += server.arg(0) + "\n";
        message += server.arg(1) + "\n";
        Serial.println(message);
        iMode = 1;
      }
      break;
    }
    case 1:
    {
      

      break;
    }
    case 2:
    {
      

      break;
    }
  }

}
