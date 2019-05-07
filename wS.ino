/*******************************************************************
 webServerFunctionality
*******************************************************************/

  /*---------------------------------------------------------------
    server_setup()
  ---------------------------------------------------------------*/
  void server_setup() {
    // Custom Pages
    webServer.on("/", HTTP_GET, handleRoot);
    webServer.on("/setup", HTTP_GET, handleSetup);
    webServer.on("/setup", HTTP_POST, handleSetupPost);
    
    webServer.on("/set", HTTP_GET, handleSet);
    webServer.on("/queue", HTTP_GET, handleQueue);
    webServer.on("/objectInfo", HTTP_GET, handleObjectInfo);
    webServer.on("/allObjectInfo", HTTP_GET, handleAllObjectInfo);
    webServer.on("/localObjectInfo", HTTP_GET, handleLocalObjectInfo);
    webServer.on("/localObjectValues", HTTP_GET, handleLocalObjectValues);
    //list directory
    webServer.on("/list", HTTP_GET, handleFileList);
    //load editor
    webServer.on("/edit", HTTP_GET, []() {
      if (!handleFileRead("/edit.htm")) webServer.send(404, "text/plain", "FileNotFound");
    });
    //create file
    webServer.on("/edit", HTTP_PUT, handleFileCreate);
    //delete file
    webServer.on("/edit", HTTP_DELETE, handleFileDelete);
    //first callback is called after the request has ended with all parsed arguments
    //second callback handles file uploads at that location
    webServer.on("/edit", HTTP_POST, []() {
      webServer.send(200, "text/plain", "");
    }, handleFileUpload);
    //called when the url is not defined here
    //use it to load content from SPIFFS
    webServer.onNotFound([]() {
      if (!handleFileRead(webServer.uri()))
        webServer.send(404, "text/plain", "FileNotFound");
    });
    webServer.begin();
  }
  
  /*---------------------------------------------------------------
    getContentType
  ---------------------------------------------------------------*/
  String getContentType(String filename){
    if(webServer.hasArg("download")) return "application/octet-stream";
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

  /*---------------------------------------------------------------
    formatBytes
  ---------------------------------------------------------------*/
  String formatBytes(size_t bytes){
    if (bytes < 1024){
      return String(bytes)+"B";
    } else if(bytes < (1024 * 1024)){
      return String(bytes/1024.0)+"KB";
    } else if(bytes < (1024 * 1024 * 1024)){
      return String(bytes/1024.0/1024.0)+"MB";
    } else {
      return String(bytes/1024.0/1024.0/1024.0)+"GB";
    }
   }

  /*---------------------------------------------------------------
    handleFileRead
  ---------------------------------------------------------------*/
  bool handleFileRead(String path){
    if(path.endsWith("/")) path += "index.htm";
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
      if(SPIFFS.exists(pathWithGz))
        path += ".gz";
      File file = SPIFFS.open(path, "r");
      if (contentType=="text/css") {
        String max_age = "max-age=";
        max_age += String(CSS_CACHE_TIME);
        webServer.sendHeader("Cache-Control", max_age, false);
      }
      if (contentType=="image/png" || contentType=="image/jpeg" || contentType=="image/gif") {
        String max_age = "max-age=";
        max_age += String(IMG_CACHE_TIME);
        webServer.sendHeader("Cache-Control", max_age, false);
      }
      if (contentType=="application/javascript") {
        String max_age = "max-age=";
        max_age += String(JS_CACHE_TIME);
        webServer.sendHeader("Cache-Control", max_age, false);
      }
      webServer.streamFile(file, contentType);
      file.close();
      return true;
    }
    return false;
  }

  /*---------------------------------------------------------------
    handleFileUpload
  ---------------------------------------------------------------*/
  void handleFileUpload(){
    if(webServer.uri() != "/edit") return;
    HTTPUpload& upload = webServer.upload();
    if(upload.status == UPLOAD_FILE_START){
      String filename = upload.filename;
      if(!filename.startsWith("/")) filename = "/"+filename;
      fsUploadFile = SPIFFS.open(filename, "w");
      filename = String();
    } else if(upload.status == UPLOAD_FILE_WRITE){
      if(fsUploadFile)
        fsUploadFile.write(upload.buf, upload.currentSize);
    } else if(upload.status == UPLOAD_FILE_END){
       if(fsUploadFile) {
        fsUploadFile.close();
      } else {
      }
    }
  }

  /*---------------------------------------------------------------
    handleFileDelete
  ---------------------------------------------------------------*/
  void handleFileDelete(){
    if(webServer.args() == 0) return webServer.send(500, "text/plain", "BAD ARGS");
    String path = webServer.arg(0);
    if(path == "/")
      return webServer.send(500, "text/plain", "BAD PATH");
    if(!SPIFFS.exists(path))
      return webServer.send(404, "text/plain", "FileNotFound");
    SPIFFS.remove(path);
    webServer.send(200, "text/plain", "");
    path = String();
  }

  /*---------------------------------------------------------------
    handleFileCreate
  ---------------------------------------------------------------*/
  void handleFileCreate(){
    if(webServer.args() == 0)
      return webServer.send(500, "text/plain", "BAD ARGS");
    String path = webServer.arg(0);
    if(path == "/")
      return webServer.send(500, "text/plain", "BAD PATH");
    if(SPIFFS.exists(path))
      return webServer.send(500, "text/plain", "FILE EXISTS");
    File file = SPIFFS.open(path, "w");
    if (file)
      file.close();
    else
      return webServer.send(500, "text/plain", "CREATE FAILED");
    webServer.send(200, "text/plain", "");
    path = String();
  }

  /*---------------------------------------------------------------
    handleFileList
  ---------------------------------------------------------------*/
  void handleFileList() {
    char f[50];
    if(!webServer.hasArg("dir")) {webServer.send(500, "text/plain", "BAD ARGS"); return;}
    String path = webServer.arg("dir");
    Dir dir = SPIFFS.openDir(path);
    path = String();
    String output = "[";
    while(dir.next()){
      File entry = dir.openFile("r");
      if (output != "[") output += ',';
      bool isDir = false;
      output += "{\"type\":\"";
      output += (isDir)?"dir":"file";
      output += "\",\"name\":\"";
      output += String(entry.name()).substring(1);
      output += "\"}";
      entry.close();
    }
    output += "]";
    webServer.send(200, "text/json", output);
  }

  /*---------------------------------------------------------------
    handleSet
  ---------------------------------------------------------------*/
  void handleSet() {
    long id = webServer.arg("id").toInt();
    byte variable = webServer.arg("variable").toInt();
    long v = webServer.arg("v").toInt();
    long qID = webServer.arg("qID").toInt();
    /* need later
    if (Objects[id]) {
      Objects[id]->set(variable, v);
    }
    */
    webServer.send(200, "text/html", String(qID)+"\n");
  }
  
  /*---------------------------------------------------------------
    handleObjectInfo
  ---------------------------------------------------------------*/
  void handleObjectInfo() {
    long id = webServer.arg("id").toInt();
    String page;
    //NodeMCU_Objects::objectAsJsonObj(id).printTo(page);
    webServer.send(200, "text/html", page);
  }

  /*---------------------------------------------------------------
    handleAllObjectInfo
  ---------------------------------------------------------------*/
  void handleAllObjectInfo() {
    String data="";
    jsonBuffer.clear();
    //NodeMCU_Objects::allAsJsonObj(false).printTo(data);
    webServer.send(200, "text/html", data);
    jsonBuffer.clear();
  }

  /*---------------------------------------------------------------
    handleLocalObjectInfo
  ---------------------------------------------------------------*/
  void handleLocalObjectInfo() {
    String data="";
    jsonBuffer.clear();
    //NodeMCU_Objects::allAsJsonObj(true).printTo(data);
    webServer.send(200, "text/html", data);
    jsonBuffer.clear();
  }

  /*---------------------------------------------------------------
    handleLocalObjectValues
  ---------------------------------------------------------------*/
  void handleLocalObjectValues() {
    String data="";
    jsonBuffer.clear();
    //NodeMCU_Objects::allLocalJsonValues().printTo(data);
    webServer.send(200, "text/html", data);
    jsonBuffer.clear();
  }

  void handleQueue() {
    /* need later
    String data="";
    if (QueueItems.empty()) {
      data="QueueItems is Empty";
      data+="<br>lastReceivedFromQueueWiFiClient=";
      data+=lastReceivedFromQueueWiFiClient;
      data+="<br>";
      data+="LastQueRequest=";
      data+=LastQueRequest;
      webServer.send(200, "text/html", data);
    } else {
      data="<table><tr><th>queueID</th><th>destinationStation</th><th>id</th><th>variable</th><th>value</th></tr>";
      for (std::map<long,QueueItem*>::iterator it=QueueItems.begin(); it!=QueueItems.end(); ++it) {
        data+="<tr><td>";
        data+=it->second->queueID;
        data+="</td><td>";
        data+=it->second->destinationStation->id;
        data+="</td><td>";
        data+=it->second->id;
        data+="</td><td>";
        data+=it->second->variable;
        data+="</td><td>";
        data+=it->second->value;
        data+="</td></tr>";
      }
      data+="</table>";
      data+="<br>lastReceivedFromQueueWiFiClient=";
      data+=lastReceivedFromQueueWiFiClient;
      data+="<br>";
      data+="LastQueRequest=";
      data+=LastQueRequest;
      webServer.send(200, "text/html", data);
    }
    */
  }

  /*---------------------------------------------------------------
    handleRoot
  ---------------------------------------------------------------*/
  void handleRoot() {
    String data="<html><body style='font-size: 36px;'>There is nothing here at the root.</body></html>";
    webServer.send(200, "text/html", data);
  }

  /*---------------------------------------------------------------
    handleSetup
  ---------------------------------------------------------------*/
  void handleSetup() {
    WiFi.scanNetworks();
    accessPoints.evaluateScan();
    String data="<!DOCTYPE html>";
    data += "<html>";
    data += "<head><title>WIFI SETUP</title>";
    data += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
    data += "<link rel='stylesheet' href='/css/setup/wifiSetup.css'>";
    data += "<link rel='icon' type='image/png' href='/img/wifiSetup/starYB.png'>";
    data += "<script type='text/javascript' src='/js/setup/wifiSetup.js'></script>";
    data += "</head>";
    data += "<body>";
    data += "<div id='APSetup' mode='Listing'>";
    data += "<div id='APSetupHeading'>";
    data += "<table id='wifiSetupHeader' class='heading' status=" + String(network.status);
    data += " style=\"";
    data += " --status:" + String(network.status) + ";";
    data += " --localIP:'" + WiFi.localIP().toString() + "';";
    data += " --macAddress:'" + WiFi.macAddress() + "';";
    data += "\">";
    data += "<tr><th rowspan=2></th><th colspan=2></th><th rowspan=2 onclick='document.getElementById(\"wifiSetupHeader\").setAttribute(\"scanning\",\"1\"); location.reload();'></th></tr>";
    data += "<tr><th colspan=2></th></tr>";
    data += "<tr><th colspan=2></th><th colspan=2></th></tr>";
    data += "</table>";
    data += "</div>";
    data += "<div id='APSetupListing'><table>";
    for (std::map<long,AccessPoint*>::iterator it=accessPoints.map.begin(); it!=accessPoints.map.end(); ++it) {
      data += "<tr";
      data += " id='" + String(it->second->id) + "'";
      data += " onclick='editAP(this);'";
      data += " SSID='" + it->second->SSID + "'";
      char buf[24];
      sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X", it->second->macAddress[0], it->second->macAddress[1], it->second->macAddress[2], it->second->macAddress[3], it->second->macAddress[4], it->second->macAddress[5]);
      String macAddressString = buf;
      data += " macAddress='" + macAddressString + "'";
      data += " channel='" + String(it->second->channel) + "'";
      data += " password='" + it->second->password + "'";
      data += " isHidden='" + String(it->second->isHidden) + "'";
      data += " passwordKnown='" + String((it->second->password!="")) + "'";
      if (it->second->id!=0) {
        data += " encryptionType='" + String(it->second->encryptionType) + "'";
        data += " currentScanIndex='" + String(it->second->currentScanIndex) + "'";
        data += " preferredAP='" + String(it->second->preferredAP) + "'";
        data += " isOpen='" + String(it->second->encryptionType==7) + "'";
        data += " connectIfAvailable='" + String(it->second->connectIfAvailable) + "'";
        data += " apTimeIP0='" + String(it->second->apTimeIP[0]) + "'";
        data += " apTimeIP1='" + String(it->second->apTimeIP[1]) + "'";
        data += " apTimeIP2='" + String(it->second->apTimeIP[2]) + "'";
        data += " apTimeIP3='" + String(it->second->apTimeIP[3]) + "'";
        data += " apTimeHost='" + it->second->apTimeHost + "'";
        data += " apTimeURL='" + it->second->apTimeURL + "'";
      }
      data += " connectedNow='" + String(it->second->connectedNow) + "'";
      if (it->second->id==0) {
        data += " ip0='" + String(it->second->ipAddress[0]) + "'";
        data += " ip1='" + String(it->second->ipAddress[1]) + "'";
        data += " ip2='" + String(it->second->ipAddress[2]) + "'";
        data += " ip3='" + String(it->second->ipAddress[3]) + "'";
      }
      // style here...
      data += "style=\"";
      data += "--SSID:'" + it->second->SSID + "';";
      data += " --macAddress:'" + macAddressString + "';";
      if (it->second->id!=0) {
        data += " --RSSI:'" + String(it->second->RSSI) + "';";
        data += " --encryptionType:'" + String(it->second->encryptionType) + "';";
        data += " --preferredAP:'" + String(it->second->preferredAP) + "';";
        String signalStrength = "";
        if (it->second->RSSI>=-70) signalStrength="Fair";
        if (it->second->RSSI>=-60) signalStrength="Good";
        if (it->second->RSSI>=-50) signalStrength="Excellent";
        if (it->second->RSSI>=-40) signalStrength="Amazing";
        signalStrength += " [" + String(it->second->RSSI) +"]";
        data += " --signalStrength:'" + signalStrength + "';";
      }
      data += " --channel:'" + String(it->second->channel) + "';";
      data += " --isHidden:'" + String(it->second->isHidden) + "';";
      if (it->second->id==0) {
        data += " --ipAddress:'" + String(it->second->ipAddress[0]) + "." + String(it->second->ipAddress[1]) + "." + String(it->second->ipAddress[2]) + "." + String(it->second->ipAddress[3]) + "';";
      }
      data += "\"";
      
      data += ">";
      data += "<td></td>";                                   //<< Star, ADD onclick, set as preferred
      data += "<td><div></div><div></div></td>";             //<< SSID, macAddress
      data += "<td><div></div><div></div></td>";             //<< Signal Strength, EncryptionType
      data += "<td></td>";                                   //<< Lock, ADD onclick, goto setup form page
      data += "</tr>";
    }
    data += "</table></div>";
    data += "<div id='APSetupForm'></div>";
    data += "</div>";
    
    data += "</body></html>";
    webServer.send(200, "text/html", data);
  }

void handleSetupPost() {
  String data;
  for(int i=0; i<webServer.args(); i++) {
    data += webServer.argName(i) + ":  ^" + webServer.arg(i) + "^<br>";  
  }

  String formType             = webServer.arg("formType");
  long id                     = webServer.arg("id").toInt();
  String SSID                 = webServer.arg("SSID");
  int ip0                     = webServer.arg("ip0").toInt();
  int ip1                     = webServer.arg("ip1").toInt();
  int ip2                     = webServer.arg("ip2").toInt();
  int ip3                     = webServer.arg("ip3").toInt();
  int channel                 = webServer.arg("channel").toInt();
  String password             = webServer.arg("password");
  boolean connectIfAvailable  = (webServer.arg("connectIfAvailable")=="on");
  boolean preferredAP         = (webServer.arg("preferredAP")=="on");
  int apTimeIP0               = webServer.arg("apTimeIP0").toInt();
  int apTimeIP1               = webServer.arg("apTimeIP1").toInt();
  int apTimeIP2               = webServer.arg("apTimeIP2").toInt();
  int apTimeIP3               = webServer.arg("apTimeIP3").toInt();
  String apTimeHost           = webServer.arg("apTimeHost");
  String apTimeURL            = webServer.arg("apTimeURL");

  if (formType=="AP") {
    data += "AP data received...";  
    accessPoints.change(id, SSID, ip0, ip1, ip2, ip3, channel, password);
    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<notify client with informative page.
    network.connectAndConfigureAP();
  } else {
    data += "WiFi data received...";
    accessPoints.change(id, password, connectIfAvailable, preferredAP, apTimeIP0, apTimeIP1, apTimeIP2, apTimeIP3, apTimeHost, apTimeURL);
  }
  data += "<br><b>";
  //webServer.send(200, "text/html", data);
  handleSetup(); //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< need to decide what to actually send back to client...advice about where likely to re-connect, probably
}
  
