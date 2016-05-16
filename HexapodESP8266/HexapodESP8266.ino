//=============================================================================
//Project HexAJpod - AJvdW
//Date    15 Apr 2016 - v0.2
//=============================================================================
#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <FS.h>

#include "index.html.h"
#include "calibrate.html.h"
#include "WebController.h"
#include "Phoenix.h"

const char WiFiAPPSK[] = "bestkeptsecret";

// Web Server
ESP8266WebServer server = ESP8266WebServer(80);
// Web Socket server
WebSocketsServer webSocket = WebSocketsServer(81);
// Web Controller 
WebController g_WebController; 
// Phoenix Robot Control Class
Phoenix g_Phoenix;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) 
{   // handler websocket data
    static int hx,hy,hz;
    
    switch(type) {
        case WStype_DISCONNECTED:
            g_WebController.SwitchOff();
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: 
          {
            IPAddress ip = webSocket.remoteIP(num);
            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Hexapod connected");
            g_WebController.SwitchOn();

          }
            break;
        case WStype_TEXT:      //bool btnBlue, bool btnRed, bool btnYellow, bool bntGreen, int joyX, int joyY, int joyZ  
            //Serial.printf("[%u] get Text: %s\n", num, payload);
            if(payload[0] == '#') 
            {
                // we get data
                if( payload[1] == 'R')
                {
                  g_WebController.ButtonRed();
                  webSocket.sendTXT(num,g_WebController.Status().c_str());
                }
                if( payload[1] == 'G')
                {
                  g_WebController.ButtonGreen();
                  webSocket.sendTXT(num,g_WebController.Status().c_str());
                }
                if( payload[1] == 'B')
                {
                  g_WebController.ButtonBlue();
                  webSocket.sendTXT(num,g_WebController.Status().c_str());
                }
                if( payload[1] == 'Y')
                {
                  g_WebController.ButtonYellow();
                  webSocket.sendTXT(num,g_WebController.Status().c_str());
                }
                if( payload[1] == 'x')
                {
                    hx = min(128,max(-127,atoi( (char *)(&(payload[3] )))));
                    g_WebController.JoystickMove( hx,hy,hz );
                }                  
                if( payload[1] == 'y')
                {
                    hy = min(128,max(-127,atoi( (char *)(&(payload[3] )))));
                    g_WebController.JoystickMove( hx,hy,hz );
                }                  
                if( payload[1] == 'z')
                {
                    hz = min(128,max(-127,atoi( (char *)(&(payload[3] )))));
                    g_WebController.JoystickMove( hx,hy,hz );
                }                  
            }
            break;
    }

}

void setupWiFi()
{ // setup Wifi
  WiFi.mode(WIFI_AP);

  uint8_t mac[6];
  char apStr[18] = {0};
  WiFi.macAddress(mac);
  sprintf(apStr, "Hexapod %02X%02X%02X%02X",  mac[2], mac[3], mac[4], mac[5]);

  WiFi.softAP(apStr, WiFiAPPSK);
}

void updateGait()
{ // Helper function
  g_Phoenix.GaitSelect();
}

String getContentType(String filename)
{ // determine content type
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path)
{ // read data from SPIFF
  Serial.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

String formatBytes(size_t bytes)
{ // pretty size
  if (bytes < 1024) return String(bytes)+"B";
  else if(bytes < (1024 * 1024)) return String(bytes/1024.0)+"KB";
  else if(bytes < (1024 * 1024 * 1024)) return String(bytes/1024.0/1024.0)+"MB";
  return String(bytes/1024.0/1024.0/1024.0)+"GB";
}

void EEPROMWritelong(int address, long value)
{ // Write the 4 bytes into the eeprom memory.
  EEPROM.write(address + 0, (byte)(value >> 0) & 0xFF);
  EEPROM.write(address + 1, (byte)(value >> 8) & 0xFF);
  EEPROM.write(address + 2, (byte)(value >> 16) & 0xFF);
  EEPROM.write(address + 3, (byte)(value >> 24) & 0xFF);
}

long EEPROMReadlong(long address)
{ // Return the recomposed long by using bitshift.
  return (((long)EEPROM.read(address) << 0) & 0xFF) + 
         (((long)EEPROM.read(address+1) << 8) & 0xFFFF) + 
         (((long)EEPROM.read(address+2) << 16) & 0xFFFFFF) +
         (((long)EEPROM.read(address+3) << 24) & 0xFFFFFFFF);
}

void SaveConfig()
{ // Save calibration config to EEPROM
  for( int legnr=0; legnr< 6; legnr++ )
  {
    EEPROMWritelong(legnr*48, g_Phoenix.cCoxaMin1[legnr]);
    EEPROMWritelong(legnr*48+4, g_Phoenix.cFemurMin1[legnr]);
    EEPROMWritelong(legnr*48+8, g_Phoenix.cTibiaMin1[legnr]);
    EEPROMWritelong(legnr*48+12, g_Phoenix.cCoxaMax1[legnr]);
    EEPROMWritelong(legnr*48+16, g_Phoenix.cFemurMax1[legnr]);
    EEPROMWritelong(legnr*48+20, g_Phoenix.cTibiaMax1[legnr]);
    EEPROMWritelong(legnr*48+24, g_Phoenix.cCoxaPWMC[legnr]);
    EEPROMWritelong(legnr*48+28, g_Phoenix.cFemurPWMC[legnr]);
    EEPROMWritelong(legnr*48+32, g_Phoenix.cTibiaPWMC[legnr]);
    EEPROMWritelong(legnr*48+36, g_Phoenix.cCoxaPWMD[legnr]);
    EEPROMWritelong(legnr*48+40, g_Phoenix.cFemurPWMD[legnr]);
    EEPROMWritelong(legnr*48+44, g_Phoenix.cTibiaPWMD[legnr]);
  }
  
  EEPROM.write(288,'H');
  EEPROM.write(289,'E');
  EEPROM.write(290,'X');
  EEPROM.write(291,'C');
  EEPROM.commit();
}

boolean LoadConfig()
{ // Load calibration config from EEPROM
  if (EEPROM.read(288) == 'H' && EEPROM.read(289) == 'E' && EEPROM.read(290) == 'X'  && EEPROM.read(291) == 'C' )
  {
    for( int legnr=0; legnr< 6; legnr++ )
    {
      g_Phoenix.cCoxaMin1[legnr] = EEPROMReadlong(legnr*48);
      g_Phoenix.cFemurMin1[legnr] = EEPROMReadlong(legnr*48+4);
      g_Phoenix.cTibiaMin1[legnr] = EEPROMReadlong(legnr*48+8);
      g_Phoenix.cCoxaMax1[legnr] = EEPROMReadlong(legnr*48+12);
      g_Phoenix.cFemurMax1[legnr] = EEPROMReadlong(legnr*48+16);
      g_Phoenix.cTibiaMax1[legnr] = EEPROMReadlong(legnr*48+20);
      g_Phoenix.cCoxaPWMC[legnr] = EEPROMReadlong(legnr*48+24);
      g_Phoenix.cFemurPWMC[legnr] = EEPROMReadlong(legnr*48+28);
      g_Phoenix.cTibiaPWMC[legnr] = EEPROMReadlong(legnr*48+32);
      g_Phoenix.cCoxaPWMD[legnr] = EEPROMReadlong(legnr*48+36);
      g_Phoenix.cFemurPWMD[legnr] = EEPROMReadlong(legnr*48+40);
      g_Phoenix.cTibiaPWMD[legnr] = EEPROMReadlong(legnr*48+44);
    }  
    return true;
  }
  else
    return false;
}

void send_status_json()
{ // status json
  //Serial.println("status");
  server.send( 200, "application/json", g_Phoenix.StatusJSON() );  
}

void send_calibrate_html()
{ // calibration page
  String leg[] = {"RR","RC","RF","LR","LC","LF"};
  String v;
  short  c;
  
  String html = calib_html;
  
  if (server.args() > 0  ) 
  {
    String temp = "";
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      int legnr =0;
      if( server.argName(i).substring(0,1) == "L" ) legnr += 3;
      if( server.argName(i).substring(1,2) == "C" ) legnr += 1;
      else if( server.argName(i).substring(1,2) == "F" ) legnr += 2;
      if( server.argName(i).substring(2) == "C_MINANG" ) g_Phoenix.cCoxaMin1[legnr] = server.arg(i).toInt();
      if( server.argName(i).substring(2) == "F_MINANG" ) g_Phoenix.cFemurMin1[legnr] = server.arg(i).toInt();
      if( server.argName(i).substring(2) == "T_MINANG" ) g_Phoenix.cTibiaMin1[legnr] = server.arg(i).toInt();
      if( server.argName(i).substring(2) == "C_MAXANG" ) g_Phoenix.cCoxaMax1[legnr] = server.arg(i).toInt();
      if( server.argName(i).substring(2) == "F_MAXANG" ) g_Phoenix.cFemurMax1[legnr] = server.arg(i).toInt();
      if( server.argName(i).substring(2) == "T_MAXANG" ) g_Phoenix.cTibiaMax1[legnr] = server.arg(i).toInt();
      if( server.argName(i).substring(2) == "C_PWMC" )   g_Phoenix.cCoxaPWMC[legnr] = server.arg(i).toInt();
      if( server.argName(i).substring(2) == "F_PWMC" )   g_Phoenix.cFemurPWMC[legnr] = server.arg(i).toInt();
      if( server.argName(i).substring(2) == "T_PWMC" )   g_Phoenix.cTibiaPWMC[legnr] = server.arg(i).toInt();
      if( server.argName(i).substring(2) == "C_PWMDEG" ) g_Phoenix.cCoxaPWMD[legnr] = server.arg(i).toInt();
      if( server.argName(i).substring(2) == "F_PWMDEG" ) g_Phoenix.cFemurPWMD[legnr] = server.arg(i).toInt();
      if( server.argName(i).substring(2) == "T_PWMDEG" ) g_Phoenix.cTibiaPWMD[legnr] = server.arg(i).toInt();
      if(  server.argName(i) == "move" )
      {
         // Save Settings 
         g_Phoenix.StartUpdateServos();
      }
      if(  server.argName(i) == "save" )
      {
         // Save Settings 
         SaveConfig();
      }
      if(  server.argName(i) == "load" )
      {
         // Load Settings 
         LoadConfig();
      }
    }
  }

  for( int LegIndex=0; LegIndex<6; LegIndex++ )
  {
    for( int t=0; t<15; t++ )
    {
      switch( t )
      {       
        case 0:  v = leg[LegIndex] + "C_ANG";    c = g_Phoenix.CoxaAngle1[LegIndex];  break;  
        case 1:  v = leg[LegIndex] + "F_ANG";    c = g_Phoenix.FemurAngle1[LegIndex]; break;  
        case 2:  v = leg[LegIndex] + "T_ANG";    c = g_Phoenix.TibiaAngle1[LegIndex]; break;  
        case 3:  v = leg[LegIndex] + "C_MINANG"; c = g_Phoenix.cCoxaMin1[LegIndex];   break;  
        case 4:  v = leg[LegIndex] + "F_MINANG"; c = g_Phoenix.cFemurMin1[LegIndex];  break;  
        case 5:  v = leg[LegIndex] + "T_MINANG"; c = g_Phoenix.cTibiaMin1[LegIndex];  break;  
        case 6:  v = leg[LegIndex] + "C_MAXANG"; c = g_Phoenix.cCoxaMax1[LegIndex];   break;  
        case 7:  v = leg[LegIndex] + "F_MAXANG"; c = g_Phoenix.cFemurMax1[LegIndex];  break;  
        case 8:  v = leg[LegIndex] + "T_MAXANG"; c = g_Phoenix.cTibiaMax1[LegIndex];  break;  
        case 9:  v = leg[LegIndex] + "C_PWMC";   c = g_Phoenix.cCoxaPWMC[LegIndex];   break;  
        case 10: v = leg[LegIndex] + "F_PWMC";   c = g_Phoenix.cFemurPWMC[LegIndex];  break;  
        case 11: v = leg[LegIndex] + "T_PWMC";   c = g_Phoenix.cTibiaPWMC[LegIndex];  break;  
        case 12: v = leg[LegIndex] + "C_PWMDEG"; c = g_Phoenix.cCoxaPWMD[LegIndex];   break;  
        case 13: v = leg[LegIndex] + "F_PWMDEG"; c = g_Phoenix.cFemurPWMD[LegIndex];  break;  
        case 14: v = leg[LegIndex] + "T_PWMDEG"; c = g_Phoenix.cTibiaPWMD[LegIndex];  break;     
      }
      html.replace( String("%") + v + String("%"), String(c) );
    }
  }
  server.send ( 200, "text/html", html);
}

void backgroundProcessing( int ms )
{ // don't stop the web
  int wait = millis()+ms/20; // We've got a timing issue
  server.handleClient();
  while( millis() < wait  ) // Need to catch cycle through
  {  
      webSocket.loop();
      yield();
  }
}

void setup() 
{   // main Setup
    Serial.begin(115200);
    Serial.printf("\n\n\nHexapod v0.2\n");
    
    Serial.printf("Setup SPIFFS\n");
    SPIFFS.begin();
    {
      Dir dir = SPIFFS.openDir("/");
      while (dir.next()) 
      {    
        String fileName = dir.fileName();
        size_t fileSize = dir.fileSize();
        Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
      }
        Serial.printf("\n");
    }
    
    Serial.printf("Setup Wifi\n");
    setupWiFi();
       

    Serial.printf("Init Controller\n");
    g_WebController.Init();
   
    EEPROM.begin(512);
    if( LoadConfig() ) // Load Calibration Config
      Serial.printf("Config loaded\n");
    else
      Serial.printf("Config not found (calibrate!)\n");
    
    // start webSocket server
    Serial.printf("Start Websocket\n"); 
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    if(MDNS.begin("esp8266")) 
        Serial.println("Start mDNS");

    server.on( "/", [](){ server.send ( 200, "text/html", index_html );} );
    server.on( "/calibrate", send_calibrate_html ); 
    server.on( "/status", send_status_json ); 
    
    //called when the url is not defined here
    //use it to load content from SPIFFS
    server.onNotFound([](){ if(!handleFileRead(server.uri())) server.send(404, "text/plain", "FileNotFound"); });

    Serial.printf("Start Webserver\n"); 
    server.begin();

    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);

    Serial.printf("Setup i2c\n");
    Wire.begin(D3, D4); //pins D3 and D4

    Serial.printf("Init Phoenix\n");
    g_Phoenix.Init();    
    
    backgroundProcessing(500);
    yield();
    
    Serial.printf("Running loop ...\n");
 }

void loop(void)
{   // main Loop function
    g_Phoenix.loop();
    webSocket.loop();
    server.handleClient();
    yield();
}
