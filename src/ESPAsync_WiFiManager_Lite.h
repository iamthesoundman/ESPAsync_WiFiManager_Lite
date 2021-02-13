/****************************************************************************************************************************
  ESPAsync_WiFiManager_Lite.h
  For ESP8266 / ESP32 boards

  ESPAsync_WiFiManager_Lite (https://github.com/khoih-prog/ESPAsync_WiFiManager_Lite) is a library 
  for the ESP32/ESP8266 boards to enable store Credentials in EEPROM/SPIFFS/LittleFS for easy 
  configuration/reconfiguration and autoconnect/autoreconnect of WiFi and other services without Hardcoding.

  Built by Khoi Hoang https://github.com/khoih-prog/ESPAsync_WiFiManager_Lite
  Licensed under MIT license
  
  Version: 1.1.0
   
  Version Modified By   Date        Comments
  ------- -----------  ----------   -----------
  1.0.0   K Hoang      09/02/2021  Initial coding for ESP32/ESP8266
  1.1.0   K Hoang      12/02/2021  Add support to new ESP32-S2
 *****************************************************************************************************************************/

#pragma once

#ifndef ESPAsync_WiFiManager_Lite_h
#define ESPAsync_WiFiManager_Lite_h

#if !( ESP32 || ESP8266)
  #error This code is intended to run on the ESP32/ESP8266 platform! Please check your Tools->Board setting.  
#endif

#define ESP_ASYNC_WIFI_MANAGER_LITE_VERSION        "ESPAsync_WiFiManager_Lite v1.1.0"

#ifdef ESP8266

  #include <ESP8266WiFi.h>
  #include <ESP8266WiFiMulti.h>
  #include <ESPAsyncWebServer.h>
  
  //default to use EEPROM, otherwise, use LittleFS or SPIFFS
  #if ( USE_LITTLEFS || USE_SPIFFS )

    #if USE_LITTLEFS
      #define FileFS        LittleFS
      #define FS_Name       "LittleFS"
      #warning Using LittleFS in ESPAsync_WiFiManager_Lite.h
    #else
      #define FileFS        SPIFFS
      #define FS_Name       "SPIFFS"
      #warning Using SPIFFS in ESPAsync_WiFiManager_Lite.h
    #endif

    #include <FS.h>
    #include <LittleFS.h>
  #else
    #include <EEPROM.h>
    #define FS_Name       "EEPROM"
    #warning Using EEPROM in ESPAsync_WiFiManager_Lite.h
  #endif
 
#else		//ESP32

  #include <WiFi.h>
  #include <WiFiMulti.h>
  #include <ESPAsyncWebServer.h>
  
  
  // LittleFS has higher priority than SPIFFS. 
  // But if not specified any, use SPIFFS to not forcing user to install LITTLEFS library
  #if ! (defined(USE_LITTLEFS) || defined(USE_SPIFFS) )
    #define USE_SPIFFS      true
  #endif

  #if USE_LITTLEFS
    // Use LittleFS
    #include "FS.h"

    // The library will be depreciated after being merged to future major Arduino esp32 core release 2.x
    // At that time, just remove this library inclusion
    #include <LITTLEFS.h>             // https://github.com/lorol/LITTLEFS
    
    FS* filesystem =      &LITTLEFS;
    #define FileFS        LITTLEFS
    #define FS_Name       "LittleFS"
    #warning Using LittleFS in ESPAsync_WiFiManager_Lite.h
  #elif USE_SPIFFS
    #include "FS.h"
    #include <SPIFFS.h>
    FS* filesystem =      &SPIFFS;
    #define FileFS        SPIFFS
    #define FS_Name       "SPIFFS"
    #warning Using SPIFFS in ESPAsync_WiFiManager_Lite.h
  #else
    #include <EEPROM.h>
    #define FS_Name       "EEPROM"
    #warning Using EEPROM in ESPAsync_WiFiManager_Lite.h
  #endif
  
#endif

#define HTTP_PORT     80

#include <DNSServer.h>
#include <memory>
#undef min
#undef max
#include <algorithm>

//KH, for ESP32
#ifdef ESP8266
  extern "C"
  {
    #include "user_interface.h"
  }
  
  #define ESP_getChipId()   (ESP.getChipId())
#else		//ESP32
  #include <esp_wifi.h>
  #define ESP_getChipId()   ((uint32_t)ESP.getEfuseMac())
#endif

#include <ESPAsync_WiFiManager_Lite_Debug.h>

#if !defined(USING_MRD)
  #define USING_MRD       false
#endif

#if USING_MRD

  ///////// NEW for MRD /////////////
  // These defines must be put before #include <ESP_DoubleResetDetector.h>
  // to select where to store DoubleResetDetector's variable.
  // For ESP32, You must select one to be true (EEPROM or SPIFFS/LittleFS)
  // For ESP8266, You must select one to be true (RTC, EEPROM or SPIFFS/LittleFS)
  // Otherwise, library will use default EEPROM storage
  #define ESP8266_MRD_USE_RTC     false   //true

  #if USE_LITTLEFS
    #define ESP_MRD_USE_LITTLEFS    true
    #define ESP_MRD_USE_SPIFFS      false
    #define ESP_MRD_USE_EEPROM      false
  #elif USE_SPIFFS
    #define ESP_MRD_USE_LITTLEFS    false
    #define ESP_MRD_USE_SPIFFS      true
    #define ESP_MRD_USE_EEPROM      false
  #else
    #define ESP_MRD_USE_LITTLEFS    false
    #define ESP_MRD_USE_SPIFFS      false
    #define ESP_MRD_USE_EEPROM      true
  #endif

  #ifndef MULTIRESETDETECTOR_DEBUG
    #define MULTIRESETDETECTOR_DEBUG     false
  #endif
  
  // These definitions must be placed before #include <ESP_MultiResetDetector.h> to be used
  // Otherwise, default values (MRD_TIMES = 3, MRD_TIMEOUT = 10 seconds and MRD_ADDRESS = 0) will be used
  // Number of subsequent resets during MRD_TIMEOUT to activate
  #ifndef MRD_TIMES
    #define MRD_TIMES               3
  #endif

  // Number of seconds after reset during which a
  // subsequent reset will be considered a double reset.
  #ifndef MRD_TIMEOUT
    #define MRD_TIMEOUT 10
  #endif

  // EEPROM Memory Address for the MultiResetDetector to use
  #ifndef MRD_TIMEOUT
    #define MRD_ADDRESS 0
  #endif
  
  #include <ESP_MultiResetDetector.h>      //https://github.com/khoih-prog/ESP_MultiResetDetector

  //MultiResetDetector mrd(MRD_TIMEOUT, MRD_ADDRESS);
  MultiResetDetector* mrd;

  ///////// NEW for MRD /////////////
  
#else

  ///////// NEW for DRD /////////////
  // These defines must be put before #include <ESP_DoubleResetDetector.h>
  // to select where to store DoubleResetDetector's variable.
  // For ESP32, You must select one to be true (EEPROM or SPIFFS/LittleFS)
  // For ESP8266, You must select one to be true (RTC, EEPROM or SPIFFS/LittleFS)
  // Otherwise, library will use default EEPROM storage
  #define ESP8266_DRD_USE_RTC     false   //true

  #if USE_LITTLEFS
    #define ESP_DRD_USE_LITTLEFS    true
    #define ESP_DRD_USE_SPIFFS      false
    #define ESP_DRD_USE_EEPROM      false
  #elif USE_SPIFFS
    #define ESP_DRD_USE_LITTLEFS    false
    #define ESP_DRD_USE_SPIFFS      true
    #define ESP_DRD_USE_EEPROM      false
  #else
    #define ESP_DRD_USE_LITTLEFS    false
    #define ESP_DRD_USE_SPIFFS      false
    #define ESP_DRD_USE_EEPROM      true
  #endif

  #ifndef DOUBLERESETDETECTOR_DEBUG
    #define DOUBLERESETDETECTOR_DEBUG     false
  #endif

  // Number of seconds after reset during which a
  // subsequent reset will be considered a double reset.
  #define DRD_TIMEOUT 10

  // RTC Memory Address for the DoubleResetDetector to use
  #define DRD_ADDRESS 0
  
  #include <ESP_DoubleResetDetector.h>      //https://github.com/khoih-prog/ESP_DoubleResetDetector

  //DoubleResetDetector drd(DRD_TIMEOUT, DRD_ADDRESS);
  DoubleResetDetector* drd;

  ///////// NEW for DRD /////////////

#endif


//NEW
#define MAX_ID_LEN                5
#define MAX_DISPLAY_NAME_LEN      16

typedef struct
{
  char id             [MAX_ID_LEN + 1];
  char displayName    [MAX_DISPLAY_NAME_LEN + 1];
  char *pdata;
  uint8_t maxlen;
} MenuItem;
//

#if USE_DYNAMIC_PARAMETERS
  #warning Using Dynamic Parameters
  ///NEW
  extern uint16_t NUM_MENU_ITEMS;
  extern MenuItem myMenuItems [];
  bool *menuItemUpdated = NULL;
#else
  #warning Not using Dynamic Parameters
#endif


#define SSID_MAX_LEN      32
// WPA2 passwords can be up to 63 characters long.
#define PASS_MAX_LEN      64

typedef struct
{
  char wifi_ssid[SSID_MAX_LEN];
  char wifi_pw  [PASS_MAX_LEN];
}  WiFi_Credentials;

#define NUM_WIFI_CREDENTIALS      2

// Configurable items besides fixed Header, just add board_name 
#define NUM_CONFIGURABLE_ITEMS    ( ( 2 * NUM_WIFI_CREDENTIALS ) + 1 )
////////////////

#define HEADER_MAX_LEN            16
#define BOARD_NAME_MAX_LEN        24

typedef struct Configuration
{
  char header         [HEADER_MAX_LEN];
  WiFi_Credentials  WiFi_Creds  [NUM_WIFI_CREDENTIALS];
  char board_name     [BOARD_NAME_MAX_LEN];
  int  checkSum;
} ESP_WM_LITE_Configuration;

// Currently CONFIG_DATA_SIZE  =   236  = (16 + 96 * 2 + 4 + 24)
uint16_t CONFIG_DATA_SIZE = sizeof(ESP_WM_LITE_Configuration);

///New from v1.0.4
extern bool LOAD_DEFAULT_CONFIG_DATA;
extern ESP_WM_LITE_Configuration defaultConfig;

// -- HTML page fragments
const char ESP_WM_LITE_HTML_HEAD[]     /*PROGMEM*/ = "<!DOCTYPE html><html><head><title>ESP_WM_LITE</title><style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{text-align: center;}button{background-color:#16A1E7;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.3rem;margin:0px;}</style></head><div style=\"text-align:left;display:inline-block;min-width:260px;\">\
<fieldset><div><label>WiFi SSID</label><input value=\"[[id]]\"id=\"id\"><div></div></div>\
<div><label>PWD</label><input value=\"[[pw]]\"id=\"pw\"><div></div></div>\
<div><label>WiFi SSID1</label><input value=\"[[id1]]\"id=\"id1\"><div></div></div>\
<div><label>PWD1</label><input value=\"[[pw1]]\"id=\"pw1\"><div></div></div></fieldset>\
<fieldset><div><label>Board Name</label><input value=\"[[nm]]\"id=\"nm\"><div></div></div></fieldset>";

const char ESP_WM_LITE_FLDSET_START[]  /*PROGMEM*/ = "<fieldset>";
const char ESP_WM_LITE_FLDSET_END[]    /*PROGMEM*/ = "</fieldset>";
const char ESP_WM_LITE_HTML_PARAM[]    /*PROGMEM*/ = "<div><label>{b}</label><input value='[[{v}]]'id='{i}'><div></div></div>";
const char ESP_WM_LITE_HTML_BUTTON[]   /*PROGMEM*/ = "<button onclick=\"sv()\">Save</button></div>";
const char ESP_WM_LITE_HTML_SCRIPT[]   /*PROGMEM*/ = "<script id=\"jsbin-javascript\">\
function udVal(key,val){var request=new XMLHttpRequest();var url='/?key='+key+'&value='+encodeURIComponent(val);\
request.open('GET',url,false);request.send(null);}\
function sv(){udVal('id',document.getElementById('id').value);udVal('pw',document.getElementById('pw').value);\
udVal('id1',document.getElementById('id1').value);udVal('pw1',document.getElementById('pw1').value);\
udVal('nm',document.getElementById('nm').value);";

const char ESP_WM_LITE_HTML_SCRIPT_ITEM[]  /*PROGMEM*/ = "udVal('{d}',document.getElementById('{d}').value);";
const char ESP_WM_LITE_HTML_SCRIPT_END[]   /*PROGMEM*/ = "alert('Updated');}</script>";
const char ESP_WM_LITE_HTML_END[]          /*PROGMEM*/ = "</html>";
///


String IPAddressToString(IPAddress _address)
{
  String str = String(_address[0]);
  str += ".";
  str += String(_address[1]);
  str += ".";
  str += String(_address[2]);
  str += ".";
  str += String(_address[3]);
  return str;
}

class ESPAsync_WiFiManager_Lite
{
    public:
    
    ESPAsync_WiFiManager_Lite()
    {
 
    }

    ~ESPAsync_WiFiManager_Lite()
    {
      if (server)
        delete server;
    }
        
    void connectWiFi(const char* ssid, const char* pass)
    {
      ESP_WML_LOGINFO1(F("Con2:"), ssid);
      WiFi.mode(WIFI_STA);

      if (static_IP != IPAddress(0, 0, 0, 0))
      {
        ESP_WML_LOGINFO(F("UseStatIP"));
        WiFi.config(static_IP, static_GW, static_SN, static_DNS1, static_DNS2);
      }

      setHostname();

      if (WiFi.status() != WL_CONNECTED)
      {
        if (pass && strlen(pass))
        {
          WiFi.begin(ssid, pass);
        } else
        {
          WiFi.begin(ssid);
        }
      }
      while (WiFi.status() != WL_CONNECTED)
      {
        delay(500);
      }

      ESP_WML_LOGINFO(F("Conn2WiFi"));
      displayWiFiData();
    }
   
    void begin(const char* ssid,
               const char* pass )
    {
      ESP_WML_LOGERROR(F("conW"));
      connectWiFi(ssid, pass);
    }

#if ESP8266

  // For ESP8266
  #ifndef LED_BUILTIN
    #define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
  #endif
  
  #define LED_ON      LOW
  #define LED_OFF     HIGH

#else

  // For ESP32
  #ifndef LED_BUILTIN
    #define LED_BUILTIN       2         // Pin D2 mapped to pin GPIO2/ADC12 of ESP32, control on-board LED
  #endif
   
  #define LED_OFF     LOW
  #define LED_ON      HIGH

#endif

    void begin(const char *iHostname = "")
    {
#define TIMEOUT_CONNECT_WIFI			30000

      //Turn OFF
      pinMode(LED_BUILTIN, OUTPUT);
      digitalWrite(LED_BUILTIN, LED_OFF);
      
#if USING_MRD
      //// New MRD ////
      mrd = new MultiResetDetector(MRD_TIMEOUT, MRD_ADDRESS);  
      bool noConfigPortal = true;
   
      if (mrd->detectMultiReset())
#else      
      //// New DRD ////
      drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);  
      bool noConfigPortal = true;
   
      if (drd->detectDoubleReset())
#endif
      {
        ESP_WML_LOGINFO(F("Multi or Double Reset Detected"));    
        noConfigPortal = false;
      }
      //// New DRD/MRD ////
      
      if (LOAD_DEFAULT_CONFIG_DATA) 
      {   
        ESP_WML_LOGDEBUG(F("======= Start Default Config Data ======="));
        displayConfigData(defaultConfig);
      }

      WiFi.mode(WIFI_STA);

      if (iHostname[0] == 0)
      {
        String _hostname = "ESP8266-" + String(ESP_getChipId(), HEX);
        _hostname.toUpperCase();

        getRFC952_hostname(_hostname.c_str());
      }
      else
      {
        // Prepare and store the hostname only not NULL
        getRFC952_hostname(iHostname);
      }

      ESP_WML_LOGINFO1(F("Hostname="), RFC952_hostname);
          
      hadConfigData = getConfigData();
      
      isForcedConfigPortal = isForcedCP();
      
      //// New DRD/MRD ////
      //  noConfigPortal when getConfigData() OK and no MRD/DRD'ed
      //if (getConfigData() && noConfigPortal)
      if (hadConfigData && noConfigPortal && (!isForcedConfigPortal) )
      {
        hadConfigData = true;
           
        ESP_WML_LOGDEBUG(noConfigPortal? F("bg: noConfigPortal = true") : F("bg: noConfigPortal = false"));

        for (uint16_t i = 0; i < NUM_WIFI_CREDENTIALS; i++)
        {
          wifiMulti.addAP(ESP_WM_LITE_config.WiFi_Creds[i].wifi_ssid, ESP_WM_LITE_config.WiFi_Creds[i].wifi_pw);
        }

        if (connectMultiWiFi() == WL_CONNECTED)
        {
          ESP_WML_LOGINFO(F("bg: WiFi OK."));
        }
        else
        {
          ESP_WML_LOGINFO(F("bg: Fail2connect WiFi"));
          // failed to connect to WiFi, will start configuration mode
          startConfigurationMode();
        }
      }
      else
      {     
        ESP_WML_LOGDEBUG(isForcedConfigPortal? F("bg: isForcedConfigPortal = true") : F("bg: isForcedConfigPortal = false"));
                                
        // If not persistent => clear the flag so that after reset. no more CP, even CP not entered and saved
        if (persForcedConfigPortal)
        {
          ESP_WML_LOGINFO1(F("bg:Stay forever in CP:"), isForcedConfigPortal ? F("Forced-Persistent") : (noConfigPortal ? F("No ConfigDat") : F("DRD/MRD")));
        }
        else
        {
          ESP_WML_LOGINFO1(F("bg:Stay forever in CP:"), isForcedConfigPortal ? F("Forced-non-Persistent") : (noConfigPortal ? F("No ConfigDat") : F("DRD/MRD")));
          clearForcedCP();
          
        }
          
        hadConfigData = isForcedConfigPortal ? true : (noConfigPortal ? false : true);
        
        // failed to connect to WiFi, will start configuration mode
        startConfigurationMode();
      }
    }

#ifndef TIMEOUT_RECONNECT_WIFI
  #define TIMEOUT_RECONNECT_WIFI   10000L
#else
    // Force range of user-defined TIMEOUT_RECONNECT_WIFI between 10-60s
  #if (TIMEOUT_RECONNECT_WIFI < 10000L)
    #warning TIMEOUT_RECONNECT_WIFI too low. Reseting to 10000
    #undef TIMEOUT_RECONNECT_WIFI
    #define TIMEOUT_RECONNECT_WIFI   10000L
  #elif (TIMEOUT_RECONNECT_WIFI > 60000L)
    #warning TIMEOUT_RECONNECT_WIFI too high. Reseting to 60000
    #undef TIMEOUT_RECONNECT_WIFI
    #define TIMEOUT_RECONNECT_WIFI   60000L
  #endif
#endif

#ifndef RESET_IF_CONFIG_TIMEOUT
  #define RESET_IF_CONFIG_TIMEOUT   true
#endif

#ifndef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
  #define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET          10
#else
  // Force range of user-defined TIMES_BEFORE_RESET between 2-100
  #if (CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET < 2)
    #warning CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET too low. Reseting to 2
    #undef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
    #define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET   2
  #elif (CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET > 100)
    #warning CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET too high. Reseting to 100
    #undef CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET
    #define CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET   100
  #endif
#endif

    void run()
    {
      static int retryTimes = 0;
      
#if USING_MRD
      //// New MRD ////
      // Call the mulyi reset detector loop method every so often,
      // so that it can recognise when the timeout expires.
      // You can also call mrd.stop() when you wish to no longer
      // consider the next reset as a multi reset.
      mrd->loop();
      //// New MRD ////
#else      
      //// New DRD ////
      // Call the double reset detector loop method every so often,
      // so that it can recognise when the timeout expires.
      // You can also call drd.stop() when you wish to no longer
      // consider the next reset as a double reset.
      drd->loop();
      //// New DRD ////
#endif

      // Lost connection in running. Give chance to reconfig.
      if ( WiFi.status() != WL_CONNECTED )
      {
        // If configTimeout but user hasn't connected to configWeb => try to reconnect WiFi
        // But if user has connected to configWeb, stay there until done, then reset hardware
        if ( configuration_mode && ( configTimeout == 0 ||  millis() < configTimeout ) )
        {
          retryTimes = 0;
          
          // Fix ESP32-S2 issue with WebServer (https://github.com/espressif/arduino-esp32/issues/4348)
          if ( String(ARDUINO_BOARD) == "ESP32S2_DEV" )
          {
            delay(1);
          }

          return;
        }
        else
        {
#if RESET_IF_CONFIG_TIMEOUT
          // If we're here but still in configuration_mode, permit running TIMES_BEFORE_RESET times before reset hardware
          // to permit user another chance to config.
          if ( configuration_mode && (configTimeout != 0) )
          {             
            ESP_WML_LOGDEBUG(F("r:Check RESET_IF_CONFIG_TIMEOUT"));
          
            if (++retryTimes <= CONFIG_TIMEOUT_RETRYTIMES_BEFORE_RESET)
            {
              ESP_WML_LOGINFO1(F("run: WiFi lost, configTimeout. Connect WiFi. Retry#:"), retryTimes);
            }
            else
            {
              resetFunc();
            }
          }
#endif

          // Not in config mode, try reconnecting before forcing to config mode
          //if ( WiFi.status() != WL_CONNECTED )
          {
            ESP_WML_LOGINFO(F("run: WiFi lost. Reconnect WiFi"));
            
            if (connectMultiWiFi() == WL_CONNECTED)
            {
              // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
              digitalWrite(LED_BUILTIN, LED_OFF);

              ESP_WML_LOGINFO(F("run: WiFi reconnected"));
            }
          }

          //ESP_WML_LOGINFO(F("run: Lost connection => configMode"));
          //startConfigurationMode();
        }
      }
      else if (configuration_mode)
      {
        configuration_mode = false;
        ESP_WML_LOGINFO(F("run: got WiFi back"));
        // turn the LED_BUILTIN OFF to tell us we exit configuration mode.
        digitalWrite(LED_BUILTIN, LED_OFF);
      }
    }
    
    //////////////////////////////////////////////
    
    void setHostname()
    {
      if (RFC952_hostname[0] != 0)
      {
#if ESP8266      
        WiFi.hostname(RFC952_hostname);
#else

      // Still have bug in ESP32_S2. If using WiFi.setHostname() => WiFi.localIP() always = 255.255.255.255
      if ( String(ARDUINO_BOARD) != "ESP32S2_DEV" )
      {
        // See https://github.com/espressif/arduino-esp32/issues/2537
        WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE);
        WiFi.setHostname(RFC952_hostname);
      } 
#endif        
      }
    }
    
    //////////////////////////////////////////////

    void setConfigPortalIP(IPAddress portalIP = IPAddress(192, 168, 4, 1))
    {
      portal_apIP = portalIP;
    }
    
    //////////////////////////////////////////////
    
    void setConfigPortal(String ssid = "", String pass = "")
    {
      portal_ssid = ssid;
      portal_pass = pass;
    }
    
    //////////////////////////////////////////////

    #define MIN_WIFI_CHANNEL      1
    #define MAX_WIFI_CHANNEL      11    // Channel 13 is flaky, because of bad number 13 ;-)

    int setConfigPortalChannel(int channel = 1)
    {
      // If channel < MIN_WIFI_CHANNEL - 1 or channel > MAX_WIFI_CHANNEL => channel = 1
      // If channel == 0 => will use random channel from MIN_WIFI_CHANNEL to MAX_WIFI_CHANNEL
      // If (MIN_WIFI_CHANNEL <= channel <= MAX_WIFI_CHANNEL) => use it
      if ( (channel < MIN_WIFI_CHANNEL - 1) || (channel > MAX_WIFI_CHANNEL) )
        WiFiAPChannel = 1;
      else if ( (channel >= MIN_WIFI_CHANNEL - 1) && (channel <= MAX_WIFI_CHANNEL) )
        WiFiAPChannel = channel;

      return WiFiAPChannel;
    }
    
    //////////////////////////////////////////////
    
    void setSTAStaticIPConfig(IPAddress ip, IPAddress gw, IPAddress sn = IPAddress(255, 255, 255, 0),
                              IPAddress dns_address_1 = IPAddress(0, 0, 0, 0),
                              IPAddress dns_address_2 = IPAddress(0, 0, 0, 0))
    {
      static_IP     = ip;
      static_GW     = gw;
      static_SN     = sn;

      // Default to local GW
      if (dns_address_1 == IPAddress(0, 0, 0, 0))
        static_DNS1   = gw;
      else
        static_DNS1   = dns_address_1;

      // Default to Google DNS (8, 8, 8, 8)
      if (dns_address_2 == IPAddress(0, 0, 0, 0))
        static_DNS2   = IPAddress(8, 8, 8, 8);
      else
        static_DNS2   = dns_address_2;
    }
    
    //////////////////////////////////////////////
    
    String getWiFiSSID(uint8_t index)
    { 
      if (index >= NUM_WIFI_CREDENTIALS)
        return String("");
        
      if (!hadConfigData)
        getConfigData();

      return (String(ESP_WM_LITE_config.WiFi_Creds[index].wifi_ssid));
    }
    
    //////////////////////////////////////////////

    String getWiFiPW(uint8_t index)
    {
      if (index >= NUM_WIFI_CREDENTIALS)
        return String("");
        
      if (!hadConfigData)
        getConfigData();

      return (String(ESP_WM_LITE_config.WiFi_Creds[index].wifi_pw));
    }
    
    //////////////////////////////////////////////
    
    String getBoardName()
    {
      if (!hadConfigData)
        getConfigData();

      return (String(ESP_WM_LITE_config.board_name));
    }
    
    //////////////////////////////////////////////

    bool getWiFiStatus()
    {
      return wifi_connected;
    }
    
    //////////////////////////////////////////////
    
    ESP_WM_LITE_Configuration* getFullConfigData(ESP_WM_LITE_Configuration *configData)
    {
      if (!hadConfigData)
        getConfigData();

      // Check if NULL pointer
      if (configData)
        memcpy(configData, &ESP_WM_LITE_config, sizeof(ESP_WM_LITE_Configuration));

      return (configData);
    }
    
    //////////////////////////////////////////////

    String localIP()
    {
      ipAddress = IPAddressToString(WiFi.localIP());

      return ipAddress;
    }
    
    //////////////////////////////////////////////

    void clearConfigData()
    {
      memset(&ESP_WM_LITE_config, 0, sizeof(ESP_WM_LITE_config));
      
#if USE_DYNAMIC_PARAMETERS      
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
      }
#endif

      saveConfigData();
    }
    
    //////////////////////////////////////////////
    
    bool isConfigDataValid()
    {
      return hadConfigData;
    }
    
    //////////////////////////////////////////////
    
    // Forced CP => Flag = 0xBEEFBEEF. Else => No forced CP
    // Flag to be stored at (EEPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE) 
    // to avoid corruption to current data
    //#define FORCED_CONFIG_PORTAL_FLAG_DATA              ( (uint32_t) 0xDEADBEEF)
    //#define FORCED_PERS_CONFIG_PORTAL_FLAG_DATA         ( (uint32_t) 0xBEEFDEAD)
    
    const uint32_t FORCED_CONFIG_PORTAL_FLAG_DATA       = 0xDEADBEEF;
    const uint32_t FORCED_PERS_CONFIG_PORTAL_FLAG_DATA  = 0xBEEFDEAD;
    
    #define FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE     4
    
    void resetAndEnterConfigPortal()
    {
      persForcedConfigPortal = false;
      
      setForcedCP(false);
      
      // Delay then reset the ESP8266 after save data
      delay(1000);
      resetFunc();
    }
    
    //////////////////////////////////////////////
    
    // This will keep CP forever, until you successfully enter CP, and Save data to clear the flag.
    void resetAndEnterConfigPortalPersistent()
    {
      persForcedConfigPortal = true;
      
      setForcedCP(true);
      
      // Delay then reset the ESP8266 after save data
      delay(1000);
      resetFunc();
    }
    
    //////////////////////////////////////////////

    void resetFunc()
    {
      delay(1000);
      
#if ESP8266      
      ESP.reset();
#else
      ESP.restart();
#endif      
    }

  //////////////////////////////////////////////
  
  private:
    String ipAddress = "0.0.0.0";
    
    AsyncWebServer *server = NULL;

    //KH, for ESP32
#ifdef ESP8266
    ESP8266WiFiMulti wifiMulti;
#else		//ESP32
    WiFiMulti wifiMulti;
#endif
    
    bool configuration_mode = false;

    unsigned long configTimeout;
    bool hadConfigData = false;
    
    bool isForcedConfigPortal   = false;
    bool persForcedConfigPortal = false;

    ESP_WM_LITE_Configuration ESP_WM_LITE_config;
    
    uint16_t totalDataSize = 0;

    String macAddress = "";
    bool wifi_connected = false;

    IPAddress portal_apIP = IPAddress(192, 168, 4, 1);
    int WiFiAPChannel = 10;

    String portal_ssid = "";
    String portal_pass = "";

    IPAddress static_IP   = IPAddress(0, 0, 0, 0);
    IPAddress static_GW   = IPAddress(0, 0, 0, 0);
    IPAddress static_SN   = IPAddress(255, 255, 255, 0);
    IPAddress static_DNS1 = IPAddress(0, 0, 0, 0);
    IPAddress static_DNS2 = IPAddress(0, 0, 0, 0);

#define RFC952_HOSTNAME_MAXLEN      24
    
    char RFC952_hostname[RFC952_HOSTNAME_MAXLEN + 1];

    char* getRFC952_hostname(const char* iHostname)
    {
      memset(RFC952_hostname, 0, sizeof(RFC952_hostname));

      size_t len = ( RFC952_HOSTNAME_MAXLEN < strlen(iHostname) ) ? RFC952_HOSTNAME_MAXLEN : strlen(iHostname);

      size_t j = 0;

      for (size_t i = 0; i < len - 1; i++)
      {
        if ( isalnum(iHostname[i]) || iHostname[i] == '-' )
        {
          RFC952_hostname[j] = iHostname[i];
          j++;
        }
      }
      // no '-' as last char
      if ( isalnum(iHostname[len - 1]) || (iHostname[len - 1] != '-') )
        RFC952_hostname[j] = iHostname[len - 1];

      return RFC952_hostname;
    }
    
    void displayConfigData(ESP_WM_LITE_Configuration configData)
    {
      ESP_WML_LOGERROR5(F("Hdr="),   configData.header, F(",SSID="), configData.WiFi_Creds[0].wifi_ssid,
                   F(",PW="),   configData.WiFi_Creds[0].wifi_pw);
      ESP_WML_LOGERROR3(F("SSID1="), configData.WiFi_Creds[1].wifi_ssid, F(",PW1="),  configData.WiFi_Creds[1].wifi_pw);     
      ESP_WML_LOGERROR1(F("BName="), configData.board_name);     
                 
#if USE_DYNAMIC_PARAMETERS     
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {
        ESP_WML_LOGINFO5("i=", i, ",id=", myMenuItems[i].id, ",data=", myMenuItems[i].pdata);
      }
#endif               
    }

    void displayWiFiData()
    {
      ESP_WML_LOGERROR3(F("SSID="), WiFi.SSID(), F(",RSSI="), WiFi.RSSI());
      ESP_WML_LOGERROR1(F("IP="), localIP() );
    }

#define ESP_WM_LITE_BOARD_TYPE   "ESP_WM_LITE"
#define WM_NO_CONFIG             "blank"

    int calcChecksum()
    {
      int checkSum = 0;
      for (uint16_t index = 0; index < (sizeof(ESP_WM_LITE_config) - sizeof(ESP_WM_LITE_config.checkSum)); index++)
      {
        checkSum += * ( ( (byte*) &ESP_WM_LITE_config ) + index);
      }

      return checkSum;
    }
    
//////////////////////////////////////////////    

#if ( USE_LITTLEFS || USE_SPIFFS )
    
// Use LittleFS/InternalFS for nRF52
#define  CONFIG_FILENAME                  ("/wm_config.dat")
#define  CONFIG_FILENAME_BACKUP           ("/wm_config.bak")

#define  CREDENTIALS_FILENAME             ("/wm_cred.dat")
#define  CREDENTIALS_FILENAME_BACKUP      ("/wm_cred.bak")

#define  CONFIG_PORTAL_FILENAME           ("/wm_cp.dat")
#define  CONFIG_PORTAL_FILENAME_BACKUP    ("/wm_cp.bak")

    //////////////////////////////////////////////
    
    void saveForcedCP(uint32_t value)
    {
      File file = FileFS.open(CONFIG_PORTAL_FILENAME, "w");
      
      ESP_WML_LOGINFO(F("SaveCPFile "));

      if (file)
      {
        file.write((uint8_t*) &value, sizeof(value));
        file.close();
        ESP_WML_LOGINFO(F("OK"));
      }
      else
      {
        ESP_WML_LOGINFO(F("failed"));
      }

      // Trying open redundant CP file
      file = FileFS.open(CONFIG_PORTAL_FILENAME_BACKUP, "w");
      
      ESP_WML_LOGINFO(F("SaveBkUpCPFile "));

      if (file)
      {
        file.write((uint8_t *) &value, sizeof(value));
        file.close();
        ESP_WML_LOGINFO(F("OK"));
      }
      else
      {
        ESP_WML_LOGINFO(F("failed"));
      }
    }
    
    //////////////////////////////////////////////
    
    void setForcedCP(bool isPersistent)
    {
      uint32_t readForcedConfigPortalFlag = isPersistent? FORCED_PERS_CONFIG_PORTAL_FLAG_DATA : FORCED_CONFIG_PORTAL_FLAG_DATA;
  
      ESP_WML_LOGDEBUG(isPersistent ? F("setForcedCP Persistent") : F("setForcedCP non-Persistent"));

      saveForcedCP(readForcedConfigPortalFlag);
    }
    
    //////////////////////////////////////////////
    
    void clearForcedCP()
    {
      uint32_t readForcedConfigPortalFlag = 0;
   
      ESP_WML_LOGDEBUG(F("clearForcedCP"));
      
      saveForcedCP(readForcedConfigPortalFlag);
    }
    
    //////////////////////////////////////////////

    bool isForcedCP()
    {
      uint32_t readForcedConfigPortalFlag;
   
      ESP_WML_LOGDEBUG(F("Check if isForcedCP"));
      
      File file = FileFS.open(CONFIG_PORTAL_FILENAME, "r");
      ESP_WML_LOGINFO(F("LoadCPFile "));

      if (!file)
      {
        ESP_WML_LOGINFO(F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CONFIG_PORTAL_FILENAME_BACKUP, "r");
        ESP_WML_LOGINFO(F("LoadBkUpCPFile "));

        if (!file)
        {
          ESP_WML_LOGINFO(F("failed"));
          return false;
        }
       }

      file.readBytes((char *) &readForcedConfigPortalFlag, sizeof(readForcedConfigPortalFlag));

      ESP_WML_LOGINFO(F("OK"));
      file.close();
      
      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false     
      if (readForcedConfigPortalFlag == FORCED_CONFIG_PORTAL_FLAG_DATA)
      {       
        persForcedConfigPortal = false;
        return true;
      }
      else if (readForcedConfigPortalFlag == FORCED_PERS_CONFIG_PORTAL_FLAG_DATA)
      {       
        persForcedConfigPortal = true;
        return true;
      }
      else
      {       
        return false;
      }
    }
    
    //////////////////////////////////////////////

#if USE_DYNAMIC_PARAMETERS

    bool checkDynamicData()
    {
      int checkSum = 0;
      int readCheckSum;
      char* readBuffer;
           
      File file = FileFS.open(CREDENTIALS_FILENAME, "r");
      ESP_WML_LOGINFO(F("LoadCredFile "));

      if (!file)
      {
        ESP_WML_LOGINFO(F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CREDENTIALS_FILENAME_BACKUP, "r");
        ESP_WML_LOGINFO(F("LoadBkUpCredFile "));

        if (!file)
        {
          ESP_WML_LOGINFO(F("failed"));
          return false;
        }
      }
      
      // Find the longest pdata, then dynamically allocate buffer. Remember to free when done
      // This is used to store tempo data to calculate checksum to see of data is valid
      // We dont like to destroy myMenuItems[i].pdata with invalid data
      
      uint16_t maxBufferLength = 0;
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        if (myMenuItems[i].maxlen > maxBufferLength)
          maxBufferLength = myMenuItems[i].maxlen;
      }
      
      if (maxBufferLength > 0)
      {
        readBuffer = new char[ maxBufferLength + 1 ];
        
        // check to see NULL => stop and return false
        if (readBuffer == NULL)
        {
          ESP_WML_LOGERROR(F("ChkCrR: Error can't allocate buffer."));
          return false;
        }      
        else
        {
          ESP_WML_LOGDEBUG1(F("ChkCrR: Buffer allocated, sz="), maxBufferLength + 1);
        }          
      }
     
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = readBuffer;

        // Actual size of pdata is [maxlen + 1]
        memset(readBuffer, 0, myMenuItems[i].maxlen + 1);
        
        file.readBytes(_pointer, myMenuItems[i].maxlen);
   
        ESP_WML_LOGDEBUG3(F("ChkCrR:pdata="), readBuffer, F(",len="), myMenuItems[i].maxlen);      
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;  
        }       
      }

      file.readBytes((char *) &readCheckSum, sizeof(readCheckSum));
      
      ESP_WML_LOGINFO(F("OK"));
      file.close();
      
      ESP_WML_LOGINFO3(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
      
      // Free buffer
      if (readBuffer != NULL)
      {
        free(readBuffer);
        ESP_WML_LOGDEBUG(F("Buffer freed"));
      }
      
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;    
    }
    
    //////////////////////////////////////////////

    bool loadDynamicData()
    {
      int checkSum = 0;
      int readCheckSum;
      totalDataSize = sizeof(ESP_WM_LITE_config) + sizeof(readCheckSum);
      
      File file = FileFS.open(CREDENTIALS_FILENAME, "r");
      ESP_WML_LOGINFO(F("LoadCredFile "));

      if (!file)
      {
        ESP_WML_LOGINFO(F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CREDENTIALS_FILENAME_BACKUP, "r");
        ESP_WML_LOGINFO(F("LoadBkUpCredFile "));

        if (!file)
        {
          ESP_WML_LOGINFO(F("failed"));
          return false;
        }
      }
     
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
        totalDataSize += myMenuItems[i].maxlen;

        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
        
        file.readBytes(_pointer, myMenuItems[i].maxlen);
 
        ESP_WML_LOGDEBUG3(F("CrR:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);       
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;  
        }       
      }

      file.readBytes((char *) &readCheckSum, sizeof(readCheckSum));
      
      ESP_WML_LOGINFO(F("OK"));
      file.close();
      
      ESP_WML_LOGINFO3(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
      
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;    
    }

    //////////////////////////////////////////////
    
    void saveDynamicData()
    {
      int checkSum = 0;
    
      File file = FileFS.open(CREDENTIALS_FILENAME, "w");
      ESP_WML_LOGINFO(F("SaveCredFile "));

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
   
        ESP_WML_LOGDEBUG3(F("CW1:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
        
        if (file)
        {
          file.write((uint8_t*) _pointer, myMenuItems[i].maxlen);         
        }
        else
        {
          ESP_WML_LOGINFO(F("failed"));
        }        
                     
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;     
         }
      }
      
      if (file)
      {
        file.write((uint8_t*) &checkSum, sizeof(checkSum));     
        file.close();
        ESP_WML_LOGINFO(F("OK"));    
      }
      else
      {
        ESP_WML_LOGINFO(F("failed"));
      }   
           
      ESP_WML_LOGINFO1(F("CrWCSum=0x"), String(checkSum, HEX));
      
      // Trying open redundant Auth file
      file = FileFS.open(CREDENTIALS_FILENAME_BACKUP, "w");
      ESP_WML_LOGINFO(F("SaveBkUpCredFile "));

      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
  
        ESP_WML_LOGDEBUG3(F("CW2:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
        
        if (file)
        {
          file.write((uint8_t*) _pointer, myMenuItems[i].maxlen);         
        }
        else
        {
          ESP_WML_LOGINFO(F("failed"));
        }        
                     
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;     
         }
      }
      
      if (file)
      {
        file.write((uint8_t*) &checkSum, sizeof(checkSum));     
        file.close();
        ESP_WML_LOGINFO(F("OK"));    
      }
      else
      {
        ESP_WML_LOGINFO(F("failed"));
      }   
    }
#endif

    //////////////////////////////////////////////
    
    void NULLTerminateConfig()
    {
      //#define HEADER_MAX_LEN      16
      //#define SERVER_MAX_LEN      32
      //#define TOKEN_MAX_LEN       36
      
      // NULL Terminating to be sure
      ESP_WM_LITE_config.header[HEADER_MAX_LEN - 1] = 0;
      ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid[SSID_MAX_LEN - 1] = 0;
      ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw  [PASS_MAX_LEN - 1] = 0;
      ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid[SSID_MAX_LEN - 1] = 0;
      ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw  [PASS_MAX_LEN - 1] = 0;
      ESP_WM_LITE_config.board_name[BOARD_NAME_MAX_LEN - 1]  = 0;
    }

    //////////////////////////////////////////////    

    void loadConfigData()
    {
      File file = FileFS.open(CONFIG_FILENAME, "r");
      ESP_WML_LOGINFO(F("LoadCfgFile "));

      if (!file)
      {
        ESP_WML_LOGINFO(F("failed"));

        // Trying open redundant config file
        file = FileFS.open(CONFIG_FILENAME_BACKUP, "r");
        ESP_WML_LOGINFO(F("LoadBkUpCfgFile "));

        if (!file)
        {
          ESP_WML_LOGINFO(F("failed"));
          return;
        }
      }

      file.readBytes((char *) &ESP_WM_LITE_config, sizeof(ESP_WM_LITE_config));

      ESP_WML_LOGINFO(F("OK"));
      file.close();
    }
    
    //////////////////////////////////////////////

    void saveConfigData()
    {
      File file = FileFS.open(CONFIG_FILENAME, "w");
      ESP_WML_LOGINFO(F("SaveCfgFile "));

      int calChecksum = calcChecksum();
      ESP_WM_LITE_config.checkSum = calChecksum;
      ESP_WML_LOGINFO1(F("WCSum=0x"), String(calChecksum, HEX));

      if (file)
      {
        file.write((uint8_t*) &ESP_WM_LITE_config, sizeof(ESP_WM_LITE_config));
        file.close();
        ESP_WML_LOGINFO(F("OK"));
      }
      else
      {
        ESP_WML_LOGINFO(F("failed"));
      }

      // Trying open redundant Auth file
      file = FileFS.open(CONFIG_FILENAME_BACKUP, "w");
      ESP_WML_LOGINFO(F("SaveBkUpCfgFile "));

      if (file)
      {
        file.write((uint8_t *) &ESP_WM_LITE_config, sizeof(ESP_WM_LITE_config));
        file.close();
        ESP_WML_LOGINFO(F("OK"));
      }
      else
      {
        ESP_WML_LOGINFO(F("failed"));
      }
    }
    
    //////////////////////////////////////////////
    
    void saveAllConfigData()
    {
      saveConfigData();     
      
#if USE_DYNAMIC_PARAMETERS      
      saveDynamicData();
#endif      
    }
    
    //////////////////////////////////////////////
    
    void loadAndSaveDefaultConfigData()
    {
      // Load Default Config Data from Sketch
      memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
      strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);
      
      // Including config and dynamic data, and assume valid
      saveConfigData();
          
      ESP_WML_LOGERROR(F("======= Start Loaded Config Data ======="));
      displayConfigData(ESP_WM_LITE_config);    
    }
    
    //////////////////////////////////////////////
    
    // Return false if init new EEPROM or SPIFFS. No more need trying to connect. Go directly to config mode
    bool getConfigData()
    {
      bool dynamicDataValid = true; 
      int calChecksum; 
      
      hadConfigData = false;

#if ESP8266
      // Format SPIFFS if not yet
      if (!FileFS.begin())
      {
        FileFS.format();
#else
      // Format SPIFFS if not yet
      if (!FileFS.begin(true))
      {
        ESP_WML_LOGERROR(F("SPIFFS/LittleFS failed! Formatting."));
#endif        
        if (!FileFS.begin())
        {
#if USE_LITTLEFS
          ESP_WML_LOGERROR(F("LittleFS failed!. Please use SPIFFS or EEPROM."));
#else
          ESP_WML_LOGERROR(F("SPIFFS failed!. Please use LittleFS or EEPROM."));
#endif 
          return false;
        }
      }

      if (LOAD_DEFAULT_CONFIG_DATA)
      {
        // Load Config Data from Sketch
        memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
        strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);
        
        // Including config and dynamic data, and assume valid
        saveAllConfigData();
         
        ESP_WML_LOGINFO(F("======= Start Loaded Config Data ======="));
        displayConfigData(ESP_WM_LITE_config);

        // Don't need Config Portal anymore
        return true; 
      }
#if USE_DYNAMIC_PARAMETERS      
      else if ( ( FileFS.exists(CONFIG_FILENAME)      || FileFS.exists(CONFIG_FILENAME_BACKUP) ) &&
                ( FileFS.exists(CREDENTIALS_FILENAME) || FileFS.exists(CREDENTIALS_FILENAME_BACKUP) ) )
#else
      else if ( FileFS.exists(CONFIG_FILENAME) || FileFS.exists(CONFIG_FILENAME_BACKUP) )
#endif   
      {
        // if config file exists, load
        loadConfigData();
        
        ESP_WML_LOGINFO(F("======= Start Stored Config Data ======="));
        displayConfigData(ESP_WM_LITE_config);

        calChecksum = calcChecksum();

        ESP_WML_LOGINFO3(F("CCSum=0x"), String(calChecksum, HEX),
                   F(",RCSum=0x"), String(ESP_WM_LITE_config.checkSum, HEX));

#if USE_DYNAMIC_PARAMETERS                 
        // Load dynamic data
        dynamicDataValid = loadDynamicData();
        
        if (dynamicDataValid)
        { 
          ESP_WML_LOGINFO(F("Valid Stored Dynamic Data"));    
        }
        else
        {
          ESP_WML_LOGINFO(F("Invalid Stored Dynamic Data. Ignored"));
        }
#endif
      }
      else    
      {
        // Not loading Default config data, but having no config file => Config Portal
        return false;
      }    

      if ( (strncmp(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE, strlen(ESP_WM_LITE_BOARD_TYPE)) != 0) ||
           (calChecksum != ESP_WM_LITE_config.checkSum) || !dynamicDataValid )
                      
      {         
        // Including Credentials CSum
        ESP_WML_LOGINFO1(F("InitCfgFile,sz="), sizeof(ESP_WM_LITE_config));

        // doesn't have any configuration        
        if (LOAD_DEFAULT_CONFIG_DATA)
        {
          memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
        }
        else
        {
          memset(&ESP_WM_LITE_config, 0, sizeof(ESP_WM_LITE_config));
              
          strcpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid,   WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw,     WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid,   WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw,     WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.board_name, WM_NO_CONFIG);
          
#if USE_DYNAMIC_PARAMETERS       
          for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
            strncpy(myMenuItems[i].pdata, WM_NO_CONFIG, myMenuItems[i].maxlen);
          }
#endif          
        }
    
        strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);
        
#if USE_DYNAMIC_PARAMETERS
        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          ESP_WML_LOGDEBUG3(F("g:myMenuItems["), i, F("]="), myMenuItems[i].pdata );
        }
#endif
        
        // Don't need
        ESP_WM_LITE_config.checkSum = 0;

        saveAllConfigData();
        
        return false;        
      }
      else if ( !strncmp(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid,   WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strncmp(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw,     WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strncmp(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid,   WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strncmp(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw,     WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strlen(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid) || 
                !strlen(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid) ||
                !strlen(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw)   ||
                !strlen(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw)  )
      {
        // If SSID, PW ="nothing", stay in config mode forever until having config Data.
        return false;
      }
      else
      {
        displayConfigData(ESP_WM_LITE_config);
      }

      return true;
    }
    
    //////////////////////////////////////////////

#else

  #ifndef EEPROM_SIZE
    #define EEPROM_SIZE     2048
  #else
    #if (EEPROM_SIZE > 2048)
      #warning EEPROM_SIZE must be <= 2048. Reset to 2048
      #undef EEPROM_SIZE
      #define EEPROM_SIZE     2048
    #endif
    // FLAG_DATA_SIZE is 4, to store DRD/MRD flag
    #if (EEPROM_SIZE < FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      #warning EEPROM_SIZE must be > CONFIG_DATA_SIZE. Reset to 512
      #undef EEPROM_SIZE
      #define EEPROM_SIZE     2048
    #endif
  #endif

  #ifndef EEPROM_START
    #define EEPROM_START     0      //define 256 in DRD/MRD
  #else
    #if (EEPROM_START + FLAG_DATA_SIZE + CONFIG_DATA_SIZE + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE > EEPROM_SIZE)
      #error EPROM_START + FLAG_DATA_SIZE + CONFIG_DATA_SIZE + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE > EEPROM_SIZE. Please adjust.
    #endif
  #endif

// Stating positon to store ESP_WM_LITE_config
#define CONFIG_EEPROM_START    (EEPROM_START + FLAG_DATA_SIZE)


    //////////////////////////////////////////////
    
    void setForcedCP(bool isPersistent)
    {
      uint32_t readForcedConfigPortalFlag = isPersistent? FORCED_PERS_CONFIG_PORTAL_FLAG_DATA : FORCED_CONFIG_PORTAL_FLAG_DATA;
    
      ESP_WML_LOGINFO(F("setForcedCP"));
      
      EEPROM.put(CONFIG_EEPROM_START + CONFIG_DATA_SIZE, readForcedConfigPortalFlag);
      EEPROM.commit();
    }
    //////////////////////////////////////////////
    
    void clearForcedCP()
    { 
      ESP_WML_LOGINFO(F("clearForcedCP"));

      EEPROM.put(CONFIG_EEPROM_START + CONFIG_DATA_SIZE, 0);
      EEPROM.commit();
    }
    
    //////////////////////////////////////////////

    bool isForcedCP()
    {
      uint32_t readForcedConfigPortalFlag;

      ESP_WML_LOGINFO(F("Check if isForcedCP"));

      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false
      EEPROM.get(CONFIG_EEPROM_START + CONFIG_DATA_SIZE, readForcedConfigPortalFlag);
      
      // Return true if forced CP (0xDEADBEEF read at offset EPROM_START + DRD_FLAG_DATA_SIZE + CONFIG_DATA_SIZE)
      // => set flag noForcedConfigPortal = false     
      if (readForcedConfigPortalFlag == FORCED_CONFIG_PORTAL_FLAG_DATA)
      {       
        persForcedConfigPortal = false;
        return true;
      }
      else if (readForcedConfigPortalFlag == FORCED_PERS_CONFIG_PORTAL_FLAG_DATA)
      {       
        persForcedConfigPortal = true;
        return true;
      }
      else
      {       
        return false;
      }
    }
    
    //////////////////////////////////////////////
    
#if USE_DYNAMIC_PARAMETERS
    
    bool checkDynamicData()
    {
      int checkSum = 0;
      int readCheckSum;
      
      #define BUFFER_LEN      128
      char readBuffer[BUFFER_LEN + 1];
      
      uint16_t offset = CONFIG_EEPROM_START + sizeof(ESP_WM_LITE_config) + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE;
                
      // Find the longest pdata, then dynamically allocate buffer. Remember to free when done
      // This is used to store tempo data to calculate checksum to see of data is valid
      // We dont like to destroy myMenuItems[i].pdata with invalid data
      
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        if (myMenuItems[i].maxlen > BUFFER_LEN)
        {
          // Size too large, abort and flag false
          ESP_WML_LOGERROR(F("ChkCrR: Error Small Buffer."));
          return false;
        }
      }
         
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = readBuffer;
        
        // Prepare buffer, more than enough
        memset(readBuffer, 0, sizeof(readBuffer));
        
        // Read more than necessary, but OK and easier to code
        EEPROM.get(offset, readBuffer);
        // NULL terminated
        readBuffer[myMenuItems[i].maxlen] = 0;

        ESP_WML_LOGDEBUG3(F("ChkCrR:pdata="), readBuffer, F(",len="), myMenuItems[i].maxlen);      
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++)
        {         
          checkSum += *_pointer;  
        }   
        
        offset += myMenuItems[i].maxlen;    
      }

      EEPROM.get(offset, readCheckSum);
           
      ESP_WML_LOGINFO3(F("ChkCrR:CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
           
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;    
    }

    //////////////////////////////////////////////
    
    bool EEPROM_getDynamicData()
    {
      int readCheckSum;
      int checkSum = 0;
      uint16_t offset = CONFIG_EEPROM_START + sizeof(ESP_WM_LITE_config) + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE;
           
      totalDataSize = sizeof(ESP_WM_LITE_config) + sizeof(readCheckSum);
      
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
        totalDataSize += myMenuItems[i].maxlen;
        
        // Actual size of pdata is [maxlen + 1]
        memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
               
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++,offset++)
        {
          *_pointer = EEPROM.read(offset);
          
          checkSum += *_pointer;  
        }
         
        ESP_WML_LOGDEBUG3(F("CR:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);         
      }
      
      EEPROM.get(offset, readCheckSum);
      
      ESP_WML_LOGINFO3(F("CrCCsum=0x"), String(checkSum, HEX), F(",CrRCsum=0x"), String(readCheckSum, HEX));
      
      if ( checkSum != readCheckSum)
      {
        return false;
      }
      
      return true;
    }
    
    //////////////////////////////////////////////

    void EEPROM_putDynamicData()
    {
      int checkSum = 0;
      uint16_t offset = CONFIG_EEPROM_START + sizeof(ESP_WM_LITE_config) + FORCED_CONFIG_PORTAL_FLAG_DATA_SIZE;
                
      for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
      {       
        char* _pointer = myMenuItems[i].pdata;
           
        ESP_WML_LOGDEBUG3(F("CW:pdata="), myMenuItems[i].pdata, F(",len="), myMenuItems[i].maxlen);
                            
        for (uint16_t j = 0; j < myMenuItems[i].maxlen; j++,_pointer++,offset++)
        {
          EEPROM.write(offset, *_pointer);
          
          checkSum += *_pointer;     
         }
      }
      
      EEPROM.put(offset, checkSum);
      //EEPROM.commit();
      
      ESP_WML_LOGINFO1(F("CrWCSum=0x"), String(checkSum, HEX));
    }
#endif

    //////////////////////////////////////////////

    void saveConfigData()
    {
      int calChecksum = calcChecksum();
      ESP_WM_LITE_config.checkSum = calChecksum;
      ESP_WML_LOGINFO3(F("SaveEEPROM,sz="), EEPROM_SIZE, F(",CSum=0x"), String(calChecksum, HEX))

      EEPROM.put(CONFIG_EEPROM_START, ESP_WM_LITE_config);
      
      EEPROM.commit();
    }
    
    //////////////////////////////////////////////
    
    void saveAllConfigData()
    {
      int calChecksum = calcChecksum();
      ESP_WM_LITE_config.checkSum = calChecksum;
      ESP_WML_LOGINFO3(F("SaveEEPROM,sz="), EEPROM_SIZE, F(",CSum=0x"), String(calChecksum, HEX))

      EEPROM.put(CONFIG_EEPROM_START, ESP_WM_LITE_config);
      
#if USE_DYNAMIC_PARAMETERS         
      EEPROM_putDynamicData();
#endif
      
      EEPROM.commit();
    }

    //////////////////////////////////////////////
    
    void loadAndSaveDefaultConfigData()
    {
      // Load Default Config Data from Sketch
      memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
      strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);
      
      // Including config and dynamic data, and assume valid
      saveConfigData();
       
      ESP_WML_LOGINFO(F("======= Start Loaded Config Data ======="));
      displayConfigData(ESP_WM_LITE_config);  
    }
        
    //////////////////////////////////////////////
    
    bool getConfigData()
    {
      bool dynamicDataValid = true;
      int calChecksum;
      
      hadConfigData = false; 
      
      EEPROM.begin(EEPROM_SIZE);
      ESP_WML_LOGINFO1(F("EEPROMsz:"), EEPROM_SIZE);
      
      if (LOAD_DEFAULT_CONFIG_DATA)
      {
        // Load Config Data from Sketch
        memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
        strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);
        
        // Including config and dynamic data, and assume valid
        saveAllConfigData();
                    
        ESP_WML_LOGINFO(F("======= Start Loaded Config Data ======="));
        displayConfigData(ESP_WM_LITE_config);

        // Don't need Config Portal anymore
        return true;             
      }
      else
      {
        // Load data from EEPROM
        EEPROM.get(CONFIG_EEPROM_START, ESP_WM_LITE_config);
          
        ESP_WML_LOGINFO(F("======= Start Stored Config Data ======="));
        displayConfigData(ESP_WM_LITE_config);

        calChecksum = calcChecksum();

        ESP_WML_LOGINFO3(F("CCSum=0x"), String(calChecksum, HEX),
                   F(",RCSum=0x"), String(ESP_WM_LITE_config.checkSum, HEX));

#if USE_DYNAMIC_PARAMETERS
                 
        // Load dynamic data from EEPROM
        dynamicDataValid = EEPROM_getDynamicData();
        
        if (dynamicDataValid)
        {  
          ESP_WML_LOGINFO(F("Valid Stored Dynamic Data"));       
        }
        else
        {
          ESP_WML_LOGINFO(F("Invalid Stored Dynamic Data. Ignored"));
        }
#endif
      }
        
      if ( (strncmp(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE, strlen(ESP_WM_LITE_BOARD_TYPE)) != 0) ||
           (calChecksum != ESP_WM_LITE_config.checkSum) || !dynamicDataValid )
      {       
        // Including Credentials CSum
        ESP_WML_LOGINFO3(F("InitEEPROM,sz="), EEPROM_SIZE, F(",DataSz="), totalDataSize);

        // doesn't have any configuration        
        if (LOAD_DEFAULT_CONFIG_DATA)
        {
          memcpy(&ESP_WM_LITE_config, &defaultConfig, sizeof(ESP_WM_LITE_config));
        }
        else
        {
          memset(&ESP_WM_LITE_config, 0, sizeof(ESP_WM_LITE_config));
             
          strcpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid,   WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw,     WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid,   WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw,     WM_NO_CONFIG);
          strcpy(ESP_WM_LITE_config.board_name, WM_NO_CONFIG);
          
#if USE_DYNAMIC_PARAMETERS       
          for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);
            strncpy(myMenuItems[i].pdata, WM_NO_CONFIG, myMenuItems[i].maxlen);
          }
#endif          
        }
    
        strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);
        
#if USE_DYNAMIC_PARAMETERS
        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          ESP_WML_LOGDEBUG3(F("g:myMenuItems["), i, F("]="), myMenuItems[i].pdata );
        }
#endif
        
        // Don't need
        ESP_WM_LITE_config.checkSum = 0;

        saveAllConfigData();
        
        return false;        
      }
      else if ( !strncmp(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid,   WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strncmp(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw,     WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strncmp(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid,   WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strncmp(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw,     WM_NO_CONFIG, strlen(WM_NO_CONFIG) )  ||
                !strlen(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid) || 
                !strlen(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid) ||
                !strlen(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw)   ||
                !strlen(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw)  )
      {
        // If SSID, PW ="nothing", stay in config mode forever until having config Data.
        return false;
      }
      else
      {
        displayConfigData(ESP_WM_LITE_config);
      }

      return true;
    }
    
#endif

    //////////////////////////////////////////////

    uint8_t connectMultiWiFi()
    {
      // For ESP8266, this better be 3000 to enable connect the 1st time
#define WIFI_MULTI_CONNECT_WAITING_MS      3000L

      uint8_t status;
      
      ESP_WML_LOGINFO(F("Connecting MultiWifi..."));

      WiFi.mode(WIFI_STA);
      
      setHostname();
           
      int i = 0;
      status = wifiMulti.run();
      delay(WIFI_MULTI_CONNECT_WAITING_MS);

      while ( ( i++ < 10 ) && ( status != WL_CONNECTED ) )
      {
        status = wifiMulti.run();

        if ( status == WL_CONNECTED )
          break;
        else
          delay(WIFI_MULTI_CONNECT_WAITING_MS);
      }

      if ( status == WL_CONNECTED )
      {
        ESP_WML_LOGWARN1(F("WiFi connected after time: "), i);
        ESP_WML_LOGWARN3(F("SSID="), WiFi.SSID(), F(",RSSI="), WiFi.RSSI());
        ESP_WML_LOGWARN3(F("Channel="), WiFi.channel(), F(",IP="), WiFi.localIP() );
      }
      else
        ESP_WML_LOGERROR(F("WiFi not connected"));

      return status;
    }

    //////////////////////////////////////////////
    
    void createHTML(String& root_html_template)
    {
      String pitem;
      
      root_html_template = String(ESP_WM_LITE_HTML_HEAD);

#if USE_DYNAMIC_PARAMETERS     
      if (NUM_MENU_ITEMS > 0)
      {
        root_html_template += String(ESP_WM_LITE_FLDSET_START);
           
        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          pitem = String(ESP_WM_LITE_HTML_PARAM);

          pitem.replace("{b}", myMenuItems[i].displayName);
          pitem.replace("{v}", myMenuItems[i].id);
          pitem.replace("{i}", myMenuItems[i].id);
          
          root_html_template += pitem;      
        }
            
        root_html_template += String(ESP_WM_LITE_FLDSET_END);
      }
#endif
        
      root_html_template += String(ESP_WM_LITE_HTML_BUTTON) + ESP_WM_LITE_HTML_SCRIPT;     

#if USE_DYNAMIC_PARAMETERS           
      if (NUM_MENU_ITEMS > 0)
      {        
        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          pitem = String(ESP_WM_LITE_HTML_SCRIPT_ITEM);
          
          pitem.replace("{d}", myMenuItems[i].id);
          
          root_html_template += pitem;        
        }
      }
#endif      
      
      root_html_template += String(ESP_WM_LITE_HTML_SCRIPT_END) + ESP_WM_LITE_HTML_END;
      
      return;     
    }
    
    //////////////////////////////////////////////

    void handleRequest(AsyncWebServerRequest *request)
    {
      if (request)
      {
        String key = request->arg("key");
        String value = request->arg("value");

        static int number_items_Updated = 0;

        if (key == "" && value == "")
        {
          String result;
          createHTML(result);

          //ESP_WML_LOGDEBUG1(F("h:Repl:"), result);

          // Reset configTimeout to stay here until finished.
          configTimeout = 0;
          
          if ( RFC952_hostname[0] != 0 )
          {
            // Replace only if Hostname is valid
            result.replace("nRF52_WM_NINA_Lite", RFC952_hostname);
          }
          else if ( ESP_WM_LITE_config.board_name[0] != 0 )
          {
            // Or replace only if board_name is valid.  Otherwise, keep intact
            result.replace("nRF52_WM_NINA_Lite", ESP_WM_LITE_config.board_name);
          }

          result.replace("[[id]]",     ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid);
          result.replace("[[pw]]",     ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw);
          result.replace("[[id1]]",    ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid);
          result.replace("[[pw1]]",    ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw);
          result.replace("[[nm]]",     ESP_WM_LITE_config.board_name);
          
#if USE_DYNAMIC_PARAMETERS          
          for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
          {
            String toChange = String("[[") + myMenuItems[i].id + "]]";
            result.replace(toChange, myMenuItems[i].pdata);
          }
#endif

          // Check if HTML size is larger than 2K, warn that WebServer won't work
          // only with notorious ESP8266-AT 2K buffer limitation.          
          //ESP_WML_LOGDEBUG1(F("h:HTML page size:"), result.length());
          
          request->send(200, "text/html", result);
          
          return;
        }

        if (number_items_Updated == 0)
        {
          memset(&ESP_WM_LITE_config, 0, sizeof(ESP_WM_LITE_config));
          strcpy(ESP_WM_LITE_config.header, ESP_WM_LITE_BOARD_TYPE);
        }

#if USE_DYNAMIC_PARAMETERS
        if (!menuItemUpdated)
        {
          // Don't need to free
          menuItemUpdated = new bool[NUM_MENU_ITEMS];
          
          if (menuItemUpdated)
          {
            for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
            {           
              // To flag item is not yet updated
              menuItemUpdated[i] = false;           
            }
            
            ESP_WML_LOGDEBUG(F("h: Init menuItemUpdated" ));                    
          }
          else
          {
            ESP_WML_LOGERROR(F("h: Error can't alloc memory for menuItemUpdated" ));
          }
        }  
#endif

        static bool id_Updated  = false;
        static bool pw_Updated  = false;
        static bool id1_Updated = false;
        static bool pw1_Updated = false;
        static bool nm_Updated  = false;
          
        if (!id_Updated && (key == String("id")))
        {   
          ESP_WML_LOGDEBUG(F("h:repl id"));
          id_Updated = true;
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid) - 1)
            strcpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid, value.c_str());
          else
            strncpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid, value.c_str(), sizeof(ESP_WM_LITE_config.WiFi_Creds[0].wifi_ssid) - 1);
        }
        else if (!pw_Updated && (key == String("pw")))
        {    
          ESP_WML_LOGDEBUG(F("h:repl pw"));
          pw_Updated = true;
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw) - 1)
            strcpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw, value.c_str());
          else
            strncpy(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw, value.c_str(), sizeof(ESP_WM_LITE_config.WiFi_Creds[0].wifi_pw) - 1);
        }
        else if (!id1_Updated && (key == String("id1")))
        {   
          ESP_WML_LOGDEBUG(F("h:repl id1"));
          id1_Updated = true;
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid) - 1)
            strcpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid, value.c_str());
          else
            strncpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid, value.c_str(), sizeof(ESP_WM_LITE_config.WiFi_Creds[1].wifi_ssid) - 1);
        }
        else if (!pw1_Updated && (key == String("pw1")))
        {    
          ESP_WML_LOGDEBUG(F("h:repl pw1"));
          pw1_Updated = true;
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw) - 1)
            strcpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw, value.c_str());
          else
            strncpy(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw, value.c_str(), sizeof(ESP_WM_LITE_config.WiFi_Creds[1].wifi_pw) - 1);
        }
        else if (!nm_Updated && (key == String("nm")))
        {
          ESP_WML_LOGDEBUG(F("h:repl nm"));
          nm_Updated = true;
          
          number_items_Updated++;
          if (strlen(value.c_str()) < sizeof(ESP_WM_LITE_config.board_name) - 1)
            strcpy(ESP_WM_LITE_config.board_name, value.c_str());
          else
            strncpy(ESP_WM_LITE_config.board_name, value.c_str(), sizeof(ESP_WM_LITE_config.board_name) - 1);
        }
        
#if USE_DYNAMIC_PARAMETERS        
        for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
        {
          if ( !menuItemUpdated[i] && (key == myMenuItems[i].id) )
          {
            ESP_WML_LOGDEBUG3(F("h:"), myMenuItems[i].id, F("="), value.c_str() );
            
            menuItemUpdated[i] = true;
            
            number_items_Updated++;

            // Actual size of pdata is [maxlen + 1]
            memset(myMenuItems[i].pdata, 0, myMenuItems[i].maxlen + 1);

            if ((int) strlen(value.c_str()) < myMenuItems[i].maxlen)
              strcpy(myMenuItems[i].pdata, value.c_str());
            else
              strncpy(myMenuItems[i].pdata, value.c_str(), myMenuItems[i].maxlen);
          }
        }
#endif

        ESP_WML_LOGDEBUG1(F("h:items updated ="), number_items_Updated);
        ESP_WML_LOGDEBUG3(F("h:key ="), key, ", value =", value);

        request->send(200, "text/html", "OK");
        
#if USE_DYNAMIC_PARAMETERS        
        if (number_items_Updated == NUM_CONFIGURABLE_ITEMS + NUM_MENU_ITEMS)
#else
        if (number_items_Updated == NUM_CONFIGURABLE_ITEMS)
#endif 
        {
#if USE_LITTLEFS
          ESP_WML_LOGERROR1(F("h:Updating LittleFS:"), CONFIG_FILENAME);     
#elif USE_SPIFFS
          ESP_WML_LOGERROR1(F("h:Updating SPIFFS:"), CONFIG_FILENAME);
#else
          ESP_WML_LOGERROR(F("h:Updating EEPROM. Please wait for reset"));
#endif

          saveAllConfigData();
          
          // Done with CP, Clear CP Flag here if forced
          if (isForcedConfigPortal)
            clearForcedCP();

          ESP_WML_LOGERROR(F("h:Rst"));

          // TO DO : what command to reset
          // Delay then reset the board after save data
          delay(1000);
          resetFunc();  //call reset
        }
      }   // if (server)
    }
    
    //////////////////////////////////////////////

#ifndef CONFIG_TIMEOUT
  #warning Default CONFIG_TIMEOUT = 60s
  #define CONFIG_TIMEOUT			60000L
#endif

    void startConfigurationMode()
    {
     // turn the LED_BUILTIN ON to tell us we are in configuration mode.
      digitalWrite(LED_BUILTIN, LED_ON);

      if ( (portal_ssid == "") || portal_pass == "" )
      {
        String chipID = String(ESP_getChipId(), HEX);
        chipID.toUpperCase();

        portal_ssid = "ESP_" + chipID;

        portal_pass = "MyESP_" + chipID;
      }

      WiFi.mode(WIFI_AP);
      
      // New
      delay(100);

      static int channel;
      // Use random channel if WiFiAPChannel == 0
      if (WiFiAPChannel == 0)
      {
        //channel = random(MAX_WIFI_CHANNEL) + 1;
        channel = (millis() % MAX_WIFI_CHANNEL) + 1;        
      }
      else
        channel = WiFiAPChannel;

      WiFi.softAP(portal_ssid.c_str(), portal_pass.c_str(), channel);
      
      ESP_WML_LOGERROR3(F("\nstConf:SSID="), portal_ssid, F(",PW="), portal_pass);
      ESP_WML_LOGERROR3(F("IP="), portal_apIP.toString(), ",ch=", channel);
      
      delay(100); // ref: https://github.com/espressif/arduino-esp32/issues/985#issuecomment-359157428
      WiFi.softAPConfig(portal_apIP, portal_apIP, IPAddress(255, 255, 255, 0));

      if (!server)
      {
        server = new AsyncWebServer(HTTP_PORT);
      }

      //See https://stackoverflow.com/questions/39803135/c-unresolved-overloaded-function-type?rq=1
      if (server)
      {
        server->on("/", HTTP_GET, [this](AsyncWebServerRequest * request)  { handleRequest(request); });        
        server->begin();
      }

      // If there is no saved config Data, stay in config mode forever until having config Data.
      // or SSID, PW, Server,Token ="nothing"
      if (hadConfigData)
      {
        configTimeout = millis() + CONFIG_TIMEOUT;
                       
        ESP_WML_LOGDEBUG3(F("s:millis() = "), millis(), F(", configTimeout = "), configTimeout);
      }
      else
      {
        configTimeout = 0;             
        ESP_WML_LOGDEBUG(F("s:configTimeout = 0"));   
      }  

      configuration_mode = true;
    }
};


#endif    //ESPAsync_WiFiManager_Lite_h
