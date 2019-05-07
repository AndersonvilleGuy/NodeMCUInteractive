/*******************************************************************
 Access Point IP Address, SSI, Password & Encryption
*******************************************************************/
  const byte AP_IP[4]       = {192,168,82,66};
  const String AP_SSID      = "NodeMCUInteractive";
  const String AP_PASSWORD  = "1234567890";
  const int AP_CHANNEL      = 6;

  const String WIFI_SSID      = "Rizal Research 2.4GHz";
  const String WIFI_PASSWORD  =  "1234567890";

/*******************************************************************
 Global Constants
*******************************************************************/
  const char* OTA_HOSTNAME  = "APR27";
  const char* OTA_PASSWORD  = "1234";

  const long CSS_CACHE_TIME = 0;
  const long IMG_CACHE_TIME = 120;
  const long JS_CACHE_TIME  = 0;

  const long AP_SCAN_FREQUENCY = 10000;

/* temporary for checking that things are working */
  const int network_online_pin = 1;
  const int otaBeginRequested_pin = 3;
  const int otaStarted_pin = 15;
  const int otaProgressing_pin = 13;
  const int otaEnded_pin = 12;
  const int ota_error_pin = 16;


/*******************************************************************
 Include Required Packages
*******************************************************************/
  #include <ESP8266WiFi.h>
  #include <ArduinoOTA.h>
  #include <ESP8266WebServer.h>

  
  
  #include <inttypes.h>
  #include <FS.h>
  #include <ArduinoJson.h>
    DynamicJsonBuffer         jsonBuffer(8000);
  #include <WebSocketsServer.h>
  #include <WebSocketsClient.h>
  #include <Pinger.h>
  #include <ESP8266WiFi.h>
  extern "C"
  {
    #include <lwip/icmp.h> // needed for icmp packet definitions
  }
  #include <map>

/*------------------------------------------------------------------ 
  Forward Declarations & Global Maps
------------------------------------------------------------------*/
  class Network;
  class AccessPoint;
  class AccessPoints;
  
  //std::map<long, AccessPoint*> AccessPoints;
  //int nextAccessPointID=1;

/********************************************************************** 
 *  "Inputs"
 **********************************************************************/
  /*
  String ssid        = "RRS_OFFICE_CAMERA";                         //>> get instead from config file with interactive set-up
  String password    = "dragonwilson";
  */
/********************************************************************** 
 *  IP Address, Host Name & URL to Use in Obtaining Real Time
 **********************************************************************/

/********************************************************************** 
 *  Constants
 **********************************************************************/
  enum e_otaStatus {OTA_NOT_BEGUN, OTA_BEGIN_REQUESTED, OTA_STARTED, OTA_PROGRESSING, OTA_ENDED, OTA_ERROR};


/********************************************************************** 
 *  Class Network
 **********************************************************************/
  class Network {
    private:
      IPAddress _default_AP_IP;
      String _ssid;
      String _password;
      String _otaPassword;
      IPAddress _timeIP;
      String _timeHost;
      String _timeURL;
      boolean _online;
      long _nextAPScanTime;
      boolean _latestAPScanEvaluated;
    public:
      const int status_DISCONNECTED               = 0;
      const int status_AP_ONLY                    = 1;
      const int status_CONNECTED_TO_PREFERRED_AP  = 2;
      const int status_CONNECTED_TO_ANOTHER_AP    = 3;
      const int status_CONNECTED_TO_FORBIDDEN_AP  = 4;
      int status;

      void connectAndConfigureAP();
      
      boolean scanning;
      void setToAPOnly();
      

      Network();
      void setup();
      void loop();
      String asJsonText();
      void wssBroadcast();

      Network(String ssid, String password, IPAddress timeIP, String timeHost, String timeURL);
      boolean online();
  };

/********************************************************************** 
 *  Class AccessPoint                                      see "AP" Tab
 **********************************************************************/
  class AccessPoint {
    public:
      long          id;
      String        SSID;
      uint8_t       encryptionType;
      int32_t       RSSI;
      uint8_t       macAddress[6];
      int32_t       channel;
      boolean       isHidden;
      boolean       passwordKnown;
      String        password;
      boolean       preferredAP;
      boolean       connectIfAvailable;
      long          currentScanIndex;
      boolean       connectedNow;
      boolean       passwordFailed;
      uint8_t       apTimeIP[4];
      String        apTimeHost;
      String        apTimeURL;
      boolean       attempted;
      IPAddress     ipAddress;

      // To Instantiate AP Mode AccessPoint
      AccessPoint(IPAddress IP, String SSID, String PASSWORD, int CHANNEL=6, boolean HIDDEN=false);

      AccessPoint(uint8_t BSSID0, uint8_t BSSID1, uint8_t BSSID2, uint8_t BSSID3, uint8_t BSSID4, uint8_t BSSID5, 
        String SSID, boolean connectIfAvailable, boolean passwordKnown, String password, boolean passwordFailed, boolean preferredAP,
        uint8_t apTimeIP0, uint8_t apTimeIP1, uint8_t apTimeIP2, uint8_t apTimeIP3, String apTimeHost, String apTimeURL);

      AccessPoint(String SSID, uint8_t encryptionType, int32_t RSSI, uint8_t* BSSID, int32_t channel, bool isHidden, long currentScanIndex=-1);
      AccessPoint(String BSSIDstr, String SSID, String password, boolean passwordKnown=false); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< only for manual addition, keep 4 now...
      String asJsonText(boolean abbreviatedForSaving=false);
      void update(long currentScanIndex, String SSID, uint8_t encryptionType, int32_t RSSI, int32_t channel, bool isHidden);
      void wssBroadcast();
      boolean viable() {return (this->connectIfAvailable && !this->passwordFailed);}
      boolean attemptToConnect();
  };
  
/********************************************************************** 
 *  Class AccessPoints                                     see "AP" Tab
 **********************************************************************/
  class AccessPoints {
    public:
      std::map<long, AccessPoint*> map;
      AccessPoints(boolean x);
      long nextID;
      void evaluateScan();
      AccessPoint* preferredAP();
      void wssBroadcast();
      void setPreferredAP(long newID);
      void setPassword(long id, String newPassword);
      void setConnectIfAvailable(long id, boolean newValue);
      void setAPtimeIP0(long id, uint8_t newValue);
      void setAPtimeIP1(long id, uint8_t newValue);
      void setAPtimeIP2(long id, uint8_t newValue);
      void setAPtimeIP3(long id, uint8_t newValue);
      void setAPtimeHost(long id, String newString);
      void setAPtimeURL(long id, String newString);
      void save();
      void load();
      void resetAllToNotAttempted();
      void change(long id, String SSID, uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3, int channel, String password); //<< changed by APSetupForm for AP
      void change(long id, String password, boolean connectIfAvailable, boolean preferredAP, uint8_t apTimeIP0, uint8_t apTimeIP1, uint8_t apTimeIP2, uint8_t apTimeIP3, String apTimeHost, String apTimeURL); // changed by APSetupForm for WiFi

      AccessPoint* AP_AccessPoint();
  };

/********************************************************************** 
 *  Global Variables
 **********************************************************************/
  String  ssid        = "Rizal Research 2.4GHz";                         //>> get instead from config file with interactive set-up
  String  password    = "1234567890";
  IPAddress  timeIP   = IPAddress(192, 168, 168, 1);                        //>> get instead from config file with interactive set-up
  String  timeHost    = "http://router.asus.com";
  String  timeURL     = "/Main_Login.asp";
  Network network(ssid, password, timeIP, timeHost, timeURL);

  AccessPoints accessPoints(true);
  
  e_otaStatus otaStatus;
  ESP8266WebServer webServer(80);
  File fsUploadFile;
  WebSocketsServer webSocketServer(81, "arduino");
  int thisStationID;

  
void setup() {
  /* temporary for checking that things are working */
  pinMode(16,OUTPUT); digitalWrite(16,LOW);
  pinMode(5,OUTPUT); digitalWrite(5,LOW);
  pinMode(4,OUTPUT); digitalWrite(4,LOW);
  pinMode(0,OUTPUT); digitalWrite(0,LOW);
  pinMode(2,OUTPUT); digitalWrite(2,HIGH);
  pinMode(14,OUTPUT); digitalWrite(14,LOW);
  pinMode(12,OUTPUT); digitalWrite(12,LOW);
  pinMode(13,OUTPUT); digitalWrite(13,LOW);
  pinMode(15,OUTPUT); digitalWrite(15,LOW);
  pinMode(3,OUTPUT); digitalWrite(3,LOW);
  pinMode(1,OUTPUT); digitalWrite(1,LOW);
  
  SPIFFS.begin();
  accessPoints.load();

  AccessPoint *AP_Access_Point = new AccessPoint(AP_IP, AP_SSID, AP_PASSWORD, AP_CHANNEL, false);
  
  //AccessPoint *AP_1 = new AccessPoint("34:97:F6:67:0D:14", "Rizal Research 2.4GHz", "1234567890");

  network.setup();

  otaStatus=OTA_NOT_BEGUN;
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA.onStart([]() {otaStatus=OTA_STARTED; updateLEDs();});
  ArduinoOTA.onEnd([]() {otaStatus=OTA_ENDED; updateLEDs();});
  ArduinoOTA.onProgress([](long progress, long total) {otaStatus=OTA_PROGRESSING; updateLEDs();});
  ArduinoOTA.onError([](ota_error_t error) {otaStatus=OTA_ERROR; updateLEDs();});
  
}

void updateLEDs() {
  /* temporary for checking that things are working */
  if (network.online()) digitalWrite(network_online_pin,LOW); else digitalWrite(network_online_pin,HIGH);
  if (otaStatus==OTA_BEGIN_REQUESTED) digitalWrite(otaBeginRequested_pin,HIGH); else digitalWrite(otaBeginRequested_pin,LOW);
  if (otaStatus==OTA_STARTED) digitalWrite(otaStarted_pin,HIGH); else digitalWrite(otaStarted_pin,LOW);
  if (otaStatus==OTA_PROGRESSING) digitalWrite(otaProgressing_pin,HIGH); else digitalWrite(otaProgressing_pin,LOW);
  if (otaStatus==OTA_ENDED) digitalWrite(otaEnded_pin,HIGH); else digitalWrite(otaEnded_pin,LOW);
  if (otaStatus==OTA_ERROR) digitalWrite(ota_error_pin,HIGH); else digitalWrite(ota_error_pin,LOW);
}

void loop() {
  network.loop();
  updateLEDs();
  webServer.handleClient();
  //if (network.online()) {
    ArduinoOTA.handle();
    webSocketServer.loop();
  //}

} // loop()
