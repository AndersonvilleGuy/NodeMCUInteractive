/********************************************************************** 
 *  Class Network
 **********************************************************************/

    /*-------------------------------------------------------------
      Network::Network(ssid, password, timeIP, timeHost, timeURL)
      <<<<<<  drop once all else is in place...
    --------------------------------------------------------------*/
    Network::Network(String ssid, String password, IPAddress timeIP, String timeHost, String timeURL) {
      this->_ssid=ssid;
      this->_password=password;
      
      this->_timeIP=timeIP;
      this->_timeHost=timeHost;
      this->_timeURL=timeURL;

    }

    /*-------------------------------------------------------------
      Network::Network()
    --------------------------------------------------------------*/
    Network::Network() {
    }

    /*-------------------------------------------------------------
      Network::setup()
    --------------------------------------------------------------*/
    void Network::setup() {
      this->scanning=false;
      
      
      
      if (1==0) {
        // start WIFI_STA MODE
        this->_online=false;
        WiFi.disconnect();
        this->status=status_DISCONNECTED;
        if (WiFi.getMode() != WIFI_STA) WiFi.mode(WIFI_STA);
        char buffS[50];
        char buffP[50];
        this->_ssid.toCharArray(buffS, 50);
        this->_password.toCharArray(buffP, 50);
        WiFi.begin(buffS, buffP);
        this->status=status_CONNECTED_TO_PREFERRED_AP;  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< not necessarily accurate
      } else {
        // start WIFI_AP MODE
        this->status=status_DISCONNECTED;
        if (WiFi.getMode() != WIFI_AP_STA) WiFi.mode(WIFI_AP_STA);

        this->connectAndConfigureAP();
        if (1==0) {
          char buffS[50];
          char buffP[50];
          AP_SSID.toCharArray(buffS, 50);
          AP_PASSWORD.toCharArray(buffP, 50);
          WiFi.softAP(buffS, buffP, AP_CHANNEL);
          delay(100);
          WiFi.softAPConfig(AP_IP, AP_IP, {255,255,255,0});
        }
        
        this->status=status_AP_ONLY;
        digitalWrite(16,HIGH);

        char buffSW[50];
        char buffPW[50];
        WIFI_SSID.toCharArray(buffSW, 50);
        WIFI_PASSWORD.toCharArray(buffPW, 50);
        WiFi.begin(buffSW, buffPW);
        this->status=status_CONNECTED_TO_PREFERRED_AP;  //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< not necessarily accurate
        digitalWrite(5,HIGH);
        delay(5000);
        if (WiFi.status()!=WL_CONNECTED) digitalWrite(5,LOW);

        server_setup();
        webSocketServer.begin();
        webSocketServer.onEvent(webSocketServerEvent);
      }

      this->_nextAPScanTime=millis()+AP_SCAN_FREQUENCY;

    } //Network::setup()

    /*-------------------------------------------------------------
      Network::loop()
    --------------------------------------------------------------*/
    void Network::loop() {
      if (millis()>=this->_nextAPScanTime) {
        if (WiFi.getMode() == WIFI_STA || WiFi.getMode() == WIFI_AP_STA) {
          WiFi.scanNetworks(true);
          this->scanning=true;
          this->wssBroadcast();
          this->_latestAPScanEvaluated=false;
          this->_nextAPScanTime=millis()+AP_SCAN_FREQUENCY;
        }
      }
      if (!this->_latestAPScanEvaluated) {
        if (WiFi.scanComplete()>0) {
          this->scanning=false;
          accessPoints.evaluateScan();
          this->wssBroadcast();
          this->_latestAPScanEvaluated=true;
          this->_nextAPScanTime=millis()+AP_SCAN_FREQUENCY;
        }
      }

      return;
      
      if (WiFi.status()!=WL_CONNECTED) {
          this->_online=false;
      } else {
        if (this->_online) {
          // still connected
          //  ArduinoOTA.handle();
          //  webServer.handleClient();
          //  webSocket_loop();
        } else {
          // re-connected
            this->_online=true;
            //WiFi.mode(WIFI_STA);
            //char buffS[50];
            //char buffP[50];
            //this->_ssid.toCharArray(buffS, 50);
            //this->_password.toCharArray(buffP, 50);
            //WiFi.begin(buffS, buffP);
            
            server_setup();
            ArduinoOTA.begin();
            otaStatus=OTA_BEGIN_REQUESTED;
            webSocketServer.begin();
            webSocketServer.onEvent(webSocketServerEvent);

            // set thisStationID
              thisStationID=WiFi.localIP()[3];
            // set all Stations' ipAddress's <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<  need to figure out a scenerio where stationID <>, necessarily ipAddress[3]
            //for (std::map<long,Station*>::iterator it=Stations.begin(); it!=Stations.end(); ++it) {
            //  Station* s = (Station*) it->second;
            //  s->setIPAddress(WiFi.localIP());
            //  s->ipAddress[3]=s->id;
            //}
            // Get System Time [now()]
            //  if (timeStatus()!=timeSet) {
            //    char buffU[NMCU_BUFFER_SIZE];
            //    char buffH[NMCU_BUFFER_SIZE];
            //    this->_timeURL.toCharArray(buffU, NMCU_BUFFER_SIZE);
            //    this->_timeHost.toCharArray(buffH, NMCU_BUFFER_SIZE);
            //    getTime(this->_timeIP, buffU, buffH);
            //    //if (getTime(this->_timeIP, buffU, buffH)) this->_timeAtStartUp=now(); <<<<<<<<<< move to a global variable...or an Info Class?
            //  }
            // Start webSocketServer
            //  webSocket_setup();
        }
      }
    }

    /*-------------------------------------------------------------
      Network::asJsonText()
    --------------------------------------------------------------*/
    String Network::asJsonText() {
      String jt="{";
      jt += "\"status\":" + String(this->status);
      jt += ",\"scanning\":" + String(this->scanning);
      jt += ",\"localIP\":\"" + WiFi.localIP().toString() + "\"";
      jt += ",\"macAddress\":\"" + WiFi.macAddress() + "\"";

      jt += "}";
      return jt;
    }

    /*-------------------------------------------------------------
      Network::wssBroadcast()
    --------------------------------------------------------------*/
    void Network::wssBroadcast() {
      webSocketServer.broadcastTXT("{\"Network\":"+this->asJsonText()+"}");
    }

    /*-------------------------------------------------------------
      Network::setToAPOnly() <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<drop
    --------------------------------------------------------------*/
      void Network::setToAPOnly() {
        this->status=status_DISCONNECTED;
        if (WiFi.getMode() != WIFI_AP) WiFi.mode(WIFI_AP);
        char buffS[50];
        char buffP[50];
        AP_SSID.toCharArray(buffS, 50);
        AP_PASSWORD.toCharArray(buffP, 50);
        WiFi.softAP(buffS, buffP, 6);  //<<< third parameter is channel, which MAYBE must match the STA channel...
        delay(100);
        WiFi.softAPConfig(AP_IP, AP_IP, {255,255,255,0});
        this->status=status_AP_ONLY;
        digitalWrite(network_online_pin,HIGH);
      }


    /*-------------------------------------------------------------
      Network::online()
    --------------------------------------------------------------*/
    boolean Network::online() {
      return this->_online;
    }

    /*-------------------------------------------------------------
      Network::connectAndConfigureAP()
    --------------------------------------------------------------*/
    void Network::connectAndConfigureAP() {
      AccessPoint* AP_AP = accessPoints.AP_AccessPoint();
      if (AP_AP) {
        char buffS[50];
        char buffP[50];
        AP_AP->SSID.toCharArray(buffS, 50);
        AP_AP->password.toCharArray(buffP, 50);
        WiFi.softAP(buffS, buffP, AP_AP->channel);
        delay(100);
        WiFi.softAPConfig(AP_AP->ipAddress, AP_AP->ipAddress, {255,255,255,0});
        //this->status=status_AP_ONLY; //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<unclear who is master of status
      }
    }
