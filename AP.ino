/********************************************************************** 
 *  Class AccessPoint
 **********************************************************************/

    /*-------------------------------------------------------------
      AccessPoint::AccessPoint()
      - version to Instantiate AP Mode AccessPoint
      AccessPoint(IP, SSI, PASSWORD, CHANNEL, HIDDEN);
    --------------------------------------------------------------*/
      AccessPoint::AccessPoint(IPAddress IP, String SSID, String PASSWORD, int CHANNEL, boolean HIDDEN) {
        this->ipAddress       = IP;
        this->SSID            = SSID;
        this->password        = PASSWORD;
        this->channel         = CHANNEL;
        this->isHidden        = HIDDEN;
        this->id              = 0;
        this->passwordKnown   = (this->password==""); // note:  if true, means there is a password
        byte mac[6];
        WiFi.macAddress(mac);
        for (int i=0; i<6; i++)
          this->macAddress[i]=mac[i];
        accessPoints.map.emplace(this->id, this);
      }

    /*-------------------------------------------------------------
      AccessPoint::AccessPoint()
      - version for creating from SPIFFS using AccessPoints::load()
    --------------------------------------------------------------*/
    AccessPoint::AccessPoint(uint8_t BSSID0, uint8_t BSSID1, uint8_t BSSID2, uint8_t BSSID3, uint8_t BSSID4, uint8_t BSSID5, 
      String SSID, boolean connectIfAvailable, boolean passwordKnown, String password, boolean passwordFailed, boolean preferredAP,
      uint8_t apTimeIP0, uint8_t apTimeIP1, uint8_t apTimeIP2, uint8_t apTimeIP3, String apTimeHost, String apTimeURL)  {
        this->id=accessPoints.nextID++;
        // saved group of variables
          this->macAddress[0]=BSSID0;
          this->macAddress[1]=BSSID1;
          this->macAddress[2]=BSSID2;
          this->macAddress[3]=BSSID3;
          this->macAddress[4]=BSSID4;
          this->macAddress[5]=BSSID5;
          this->SSID=SSID;
          this->connectIfAvailable=connectIfAvailable;
          this->passwordKnown=passwordKnown;
          this->password=password;
          this->passwordFailed=passwordFailed;
          this->preferredAP=preferredAP;
          this->apTimeIP[0]=apTimeIP0;
          this->apTimeIP[1]=apTimeIP1;
          this->apTimeIP[2]=apTimeIP2;
          this->apTimeIP[3]=apTimeIP3;
          this->apTimeHost=apTimeHost;
          this->apTimeURL=apTimeURL;

        // group of variables obtained from network
          this->encryptionType=-1;
          this->RSSI=0;
          this->channel=-1;
          this->isHidden=false;
          this->currentScanIndex=-1;
          this->connectedNow=false;
        accessPoints.map.emplace(this->id, this);
      }

    /*-------------------------------------------------------------
      AccessPoint::AccessPoint(BSSIDstr, SSID, password, passwordKnown)
    --------------------------------------------------------------*/
    /* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< drop this version once using SPIFFs */
    AccessPoint::AccessPoint(String BSSIDstr, String SSID, String password, boolean passwordKnown) {
      this->id=accessPoints.nextID++;
      this->macAddress[0]=0;
      this->macAddress[1]=0;
      this->macAddress[2]=0;
      this->macAddress[3]=0;
      this->macAddress[4]=0;
      this->macAddress[5]=0;
      this->SSID=SSID;
      this->password=password;
      this->passwordKnown=passwordKnown;
      this->preferredAP=true;
      this->channel=-1;
      this->RSSI=-1;
      this->currentScanIndex=-1;
      this->isHidden=-1;
      this->connectedNow=false;
      this->passwordFailed=false;
      this->connectIfAvailable=false;
      accessPoints.map.emplace(this->id, this);
    }

    AccessPoint::AccessPoint(String SSID, uint8_t encryptionType, int32_t RSSI, uint8_t* BSSID, int32_t channel, bool isHidden, long currentScanIndex) {
      this->id=accessPoints.nextID++;
      if (this->id==2) this->passwordFailed=true; /* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< test of passwordFailed */
      this->SSID=SSID;
      this->encryptionType=encryptionType;
      this->RSSI=RSSI;
      this->macAddress[0]=BSSID[0];
      this->macAddress[1]=BSSID[1];
      this->macAddress[2]=BSSID[2];
      this->macAddress[3]=BSSID[3];
      this->macAddress[4]=BSSID[4];
      this->macAddress[5]=BSSID[5];
      this->channel=channel;
      this->isHidden=isHidden;
      this->currentScanIndex=currentScanIndex;
      this->password="";
      this->passwordKnown=false;
      this->preferredAP=false;
      this->connectIfAvailable=false;
      this->connectedNow=false;
      accessPoints.map.emplace(this->id, this);
    }

    /*-------------------------------------------------------------
      AccessPoint::asJsonText()
    --------------------------------------------------------------*/
    String AccessPoint::asJsonText(boolean abbreviatedForSaving) {
      char buf[24];
      sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", this->macAddress[0], this->macAddress[1], this->macAddress[2], this->macAddress[3], this->macAddress[4], this->macAddress[5]);
      String macAddressString = buf;
      String jt = "{";
      jt += "\"id\":" + String(this->id);
      jt += ",\"macAddress\":\"" + macAddressString + "\"";
      jt += ",\"mac0\":" + String(macAddress[0]);
      jt += ",\"mac1\":" + String(macAddress[1]);
      jt += ",\"mac2\":" + String(macAddress[2]);
      jt += ",\"mac3\":" + String(macAddress[3]);
      jt += ",\"mac4\":" + String(macAddress[4]);
      jt += ",\"mac5\":" + String(macAddress[5]);
      jt += ",\"SSID\":\"" + this->SSID + "\"";
      jt += ",\"connectIfAvailable\":" + String(this->connectIfAvailable);
      jt += ",\"passwordKnown\":" + String(this->passwordKnown);
      jt += ",\"password\":\"" + this->password + "\"";
      jt += ",\"passwordFailed\":" + String(this->passwordFailed);
      jt += ",\"preferredAP\":" + String(this->preferredAP);
      jt += ",\"apTimeIP0\":" + String(this->apTimeIP[0]);
      jt += ",\"apTimeIP1\":" + String(this->apTimeIP[1]);
      jt += ",\"apTimeIP2\":" + String(this->apTimeIP[2]);
      jt += ",\"apTimeIP3\":" + String(this->apTimeIP[3]);
      jt += ",\"apTimeHost\":\"" + this->apTimeHost + "\"";
      jt += ",\"apTimeURL\":\"" + this->apTimeURL + "\"";
      if (!abbreviatedForSaving) {
        jt += ",\"encryptionType\":" + String(this->encryptionType);
        jt += ",\"RSSI\":" + String(this->RSSI);
        jt += ",\"channel\":" + String(this->channel);
        jt += ",\"isHidden\":" + String(this->isHidden);
        jt += ",\"currentScanIndex\":" + String(this->currentScanIndex);
        jt += ",\"connectedNow\":" + String(this->connectedNow);
      }
      jt += "}";
      return jt;
    }

    /*-------------------------------------------------------------
      AccessPoint::update(currentScanIndex, SSID, encryptionType, RSSI, channel, isHidden)
    --------------------------------------------------------------*/
    void AccessPoint::update(long currentScanIndex, String SSID, uint8_t encryptionType, int32_t RSSI, int32_t channel, bool isHidden) {
      boolean anyChange=false;
      if (this->currentScanIndex!=currentScanIndex) anyChange=true;
      if (this->SSID!=SSID) anyChange=true;
      if (this->encryptionType!=encryptionType) anyChange=true;
      if (this->RSSI!=RSSI) anyChange=true;
      if (this->channel!=channel) anyChange=true;
      if (this->isHidden!=isHidden) anyChange=true;
      if (anyChange) {
        this->currentScanIndex=currentScanIndex;
        this->SSID=SSID;
        this->encryptionType=encryptionType;
        this->RSSI=RSSI;
        this->channel=channel;
        this->isHidden=isHidden;
        this->wssBroadcast();
      }
    }

    /*-------------------------------------------------------------
      AccessPoint::wssBroadcast()
    --------------------------------------------------------------*/
    void AccessPoint::wssBroadcast() {
      webSocketServer.broadcastTXT("{\"AccessPoint\":"+this->asJsonText()+"}");
    }

    /*-------------------------------------------------------------
      AccessPoint::attemptToConnect()
    --------------------------------------------------------------*/
    boolean AccessPoint::attemptToConnect() {
      //<<<< need to write...
      
      return false;
    }


/********************************************************************** 
 *  Class AccessPoints
 **********************************************************************/

    /*-------------------------------------------------------------
      AccessPoints::AccessPoints()
    --------------------------------------------------------------*/
    AccessPoints::AccessPoints(boolean x) {
      this->nextID=1;
    }

    /*-------------------------------------------------------------
      AccessPoints::evaluateScan()
    --------------------------------------------------------------*/
    void AccessPoints::evaluateScan() {
      if (WiFi.scanComplete()<=0) return;
      for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
        it->second->currentScanIndex=-1;
        it->second->connectedNow=false;
      }
      String SSID;
      uint8_t ENCRYPTION_TYPE;
      int32_t RSSI;
      uint8_t* BSSID;
      int32_t CHANNEL;
      bool IS_HIDDEN;
      uint8_t* AP_BSSID = WiFi.BSSID();
      for (int i=0; i<WiFi.scanComplete(); i++) {
        WiFi.getNetworkInfo(i, SSID, ENCRYPTION_TYPE, RSSI, BSSID, CHANNEL, IS_HIDDEN);
        boolean exists=false;
        for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
          if (it->second->macAddress[0]==BSSID[0] && it->second->macAddress[1]==BSSID[1] && it->second->macAddress[2]==BSSID[2] && it->second->macAddress[3]==BSSID[3] && it->second->macAddress[4]==BSSID[4] && it->second->macAddress[5]==BSSID[5]) {
            exists=true;
            it->second->update(i, SSID, ENCRYPTION_TYPE, RSSI, CHANNEL, IS_HIDDEN);
            it->second->connectedNow=false;
            if (WiFi.status()==WL_CONNECTED)
              if (it->second->macAddress[0]==AP_BSSID[0] && it->second->macAddress[1]==AP_BSSID[1] && it->second->macAddress[2]==AP_BSSID[2] && it->second->macAddress[3]==AP_BSSID[3] && it->second->macAddress[4]==AP_BSSID[4] && it->second->macAddress[5]==AP_BSSID[5]) {
                it->second->connectedNow=true;
              }
          }
        }
        if (!exists) {
          AccessPoint* ap = new AccessPoint(SSID, ENCRYPTION_TYPE, RSSI, BSSID, CHANNEL, IS_HIDDEN, i);
        }
      }
      WiFi.scanDelete();
      accessPoints.wssBroadcast();
    }

    /*-------------------------------------------------------------
      AccessPoints::wssBroadcast()
    --------------------------------------------------------------*/
    void AccessPoints::wssBroadcast() {
      for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
        it->second->wssBroadcast();
      };
    }

    /*-------------------------------------------------------------
      AccessPoints::preferredAP()
    --------------------------------------------------------------*/
    AccessPoint* AccessPoints::preferredAP() {
      for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
        if (it->second->preferredAP) {
          return it->second;
        }
      };
      return NULL;
    }

    /*-------------------------------------------------------------
      AccessPoints::setPreferredAP(newID)
    --------------------------------------------------------------*/
    void AccessPoints::setPreferredAP(long newID) {
      for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
        if (it->second->id==newID) {
          it->second->preferredAP=true;
          it->second->connectIfAvailable=true;
          it->second->wssBroadcast();
        } else {
          if (it->second->preferredAP) {
            it->second->preferredAP=false;
            it->second->wssBroadcast();
          }
        }
      };
      this->save(); /* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< test, but "real". */
    }

    /*-------------------------------------------------------------
      AccessPoints::setPassword(id, newPassword)
    --------------------------------------------------------------*/
    void AccessPoints::setPassword(long id, String newPassword) {
      for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
        if (it->second->id==id) {
          it->second->password=newPassword;
          it->second->passwordKnown = (newPassword!="");
          it->second->passwordFailed = false;
          it->second->wssBroadcast();
        }
      };
    }

    /*-------------------------------------------------------------
      AccessPoints::set...  (from PasswordForm)
    --------------------------------------------------------------*/
    void AccessPoints::setConnectIfAvailable(long id, boolean newValue) {
      for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
        if (it->second->id==id) {
          it->second->connectIfAvailable=newValue;
        }
      };
    }
    void AccessPoints::setAPtimeIP0(long id, uint8_t newValue) {
      for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
        if (it->second->id==id) {
          it->second->apTimeIP[0]=newValue;
        }
      };
    }
    void AccessPoints::setAPtimeIP1(long id, uint8_t newValue) {
      for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
        if (it->second->id==id) {
          it->second->apTimeIP[1]=newValue;
        }
      };
    }
    void AccessPoints::setAPtimeIP2(long id, uint8_t newValue) {
      for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
        if (it->second->id==id) {
          it->second->apTimeIP[2]=newValue;
        }
      };
    }
    void AccessPoints::setAPtimeIP3(long id, uint8_t newValue) {
      for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
        if (it->second->id==id) {
          it->second->apTimeIP[3]=newValue;
        }
      };
    }
    void AccessPoints::setAPtimeHost(long id, String newString) {
      for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
        if (it->second->id==id) {
          it->second->apTimeHost=newString;
        }
      };
    }
    void AccessPoints::setAPtimeURL(long id, String newString) {
      for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
        if (it->second->id==id) {
          it->second->apTimeURL=newString;
          it->second->wssBroadcast();
          accessPoints.save();
        }
      };
    }


    /*-------------------------------------------------------------
      AccessPoints::save();
    --------------------------------------------------------------*/
    void AccessPoints::save() {
      File f = SPIFFS.open("/cfg/AccessPoints.txt", "w");
      if (f) {
        boolean isFirst=true;
        f.print("[");
        for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it) {
          if (!isFirst) f.println(",");
          f.print(it->second->asJsonText(true));
          isFirst=false;
        };
        f.println("]");
        f.close();
      }
    }

    /*-------------------------------------------------------------
      AccessPoints::load();
    --------------------------------------------------------------*/
    void AccessPoints::load() {
      File f = SPIFFS.open("/cfg/AccessPoints.txt", "r");
      if (f) {
        long fileSize=f.size();
        DynamicJsonBuffer jb(fileSize+10);
        JsonArray &ja = jb.parseArray(f);
        if (ja.success()) {
          for (auto& ap : ja) {
            uint8_t   macAddress[6];
            String    SSID;
            boolean   connectIfAvailable;
            boolean   passwordKnown;
            String    password;
            boolean   passwordFailed;
            boolean   preferredAP;
            uint8_t   apTimeIP[4];
            String    apTimeHost;
            String    apTimeURL;

            macAddress[0] = ap["mac0"];
            macAddress[1] = ap["mac1"];
            macAddress[2] = ap["mac2"];
            macAddress[3] = ap["mac3"];
            macAddress[4] = ap["mac4"];
            macAddress[5] = ap["mac5"];
            SSID = ap["SSID"].as<String>();
            apTimeIP[0] = ap["apTimeIP0"];
            apTimeIP[1] = ap["apTimeIP1"];
            apTimeIP[2] = ap["apTimeIP2"];
            apTimeIP[3] = ap["apTimeIP3"];
            apTimeHost = ap["apTimeHost"].as<String>();
            apTimeURL = ap["apTimeURL"].as<String>();
            
            connectIfAvailable = (ap["connectIfAvailable"]==1);
            passwordKnown = (ap["passwordKnown"]==1);
            password = ap["password"].as<String>();
            passwordFailed = (ap["passwordFailed"]==1);
            preferredAP = (ap["preferredAP"]==1);
            AccessPoint* newAP = new AccessPoint(macAddress[0], macAddress[1], macAddress[2], macAddress[3], macAddress[4], macAddress[5], 
              SSID, connectIfAvailable, passwordKnown, password, passwordFailed, preferredAP,
              apTimeIP[0], apTimeIP[1], apTimeIP[2], apTimeIP[3], apTimeHost, apTimeURL);
          }
        }
      }
      f.close();
    }


    /*-------------------------------------------------------------
      AccessPoints::resetAllToNotAttempted();
    --------------------------------------------------------------*/
    void AccessPoints::resetAllToNotAttempted() {
        for (std::map<long,AccessPoint*>::iterator it=this->map.begin(); it!=this->map.end(); ++it)
          it->second->attempted=false;
     }

    /*-------------------------------------------------------------
      AccessPoints::change(id, SSID, ip0, ip1, ip2, ip3, channel, password);
    --------------------------------------------------------------*/
    void AccessPoints::change(long id, String SSID, uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3, int channel, String password) {
      for (std::map<long,AccessPoint*>::iterator it=accessPoints.map.begin(); it!=accessPoints.map.end(); ++it) {
        if (it->second->id==id) {
          it->second->SSID=SSID;
          it->second->ipAddress[0]=ip0;
          it->second->ipAddress[1]=ip1;
          it->second->ipAddress[2]=ip2;
          it->second->ipAddress[3]=ip3;
          it->second->channel=channel;
          it->second->password=password;
          //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< also need to re-start AP if there were changes...
        }
      }
    }

    /*-------------------------------------------------------------
      AccessPoints::change(id, password, connectIfAvailable, preferredAP, apTImeIP0, apTimeIP1, apTimeIP2, apTimeIP3, apTimeHost, apTimeURL);
    --------------------------------------------------------------*/
    void AccessPoints::change(long id, String password, boolean connectIfAvailable, boolean preferredAP, uint8_t apTimeIP0, uint8_t apTimeIP1, uint8_t apTimeIP2, uint8_t apTimeIP3, String apTimeHost, String apTimeURL) {
      for (std::map<long,AccessPoint*>::iterator it=accessPoints.map.begin(); it!=accessPoints.map.end(); ++it) {
        if (it->second->id==id) {
          it->second->password=password;
          it->second->connectIfAvailable=connectIfAvailable;
          it->second->preferredAP=preferredAP;
          it->second->apTimeIP[0]=apTimeIP0;
          it->second->apTimeIP[1]=apTimeIP1;
          it->second->apTimeIP[2]=apTimeIP2;
          it->second->apTimeIP[3]=apTimeIP3;
          it->second->apTimeHost=apTimeHost;
          it->second->apTimeURL=apTimeURL;
          //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< should send user a page to tell them the effects of their change(s)...
          
          //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< also need to re-establish WiFi if there were changes...

          //...but do both of these things in WS, not here...
          
        } else {
          if (preferredAP) it->second->preferredAP=false;
        }
      }
    }

    /*-------------------------------------------------------------
      AccessPoints::AP_AccessPoint();
    --------------------------------------------------------------*/
    AccessPoint* AccessPoints::AP_AccessPoint() {
      for (std::map<long,AccessPoint*>::iterator it=accessPoints.map.begin(); it!=accessPoints.map.end(); ++it) {
        if (it->second->id==0) return it->second;
      }
      return NULL;
    }
