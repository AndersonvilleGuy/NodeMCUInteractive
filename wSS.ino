/*******************************************************************
 webSocketServerFunctionality
*******************************************************************/

  /*---------------------------------------------------------------
    Global Variables
  ---------------------------------------------------------------*/
  int old_WebSocketServerConnectedClients = 0;
  const int max_WebSocketServerConnections = 5;

  /*---------------------------------------------------------------
    webSocket_setup()
  ---------------------------------------------------------------*/
  void webSocket_setup() {
    webSocketServer.begin();
    webSocketServer.onEvent(webSocketServerEvent);
  }

  /*---------------------------------------------------------------
    webSocketServerEvent()
  ---------------------------------------------------------------*/
  void webSocketServerEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    String data="";
    jsonBuffer.clear();
    JsonObject& dataIn = jsonBuffer.parseObject(payload);
    switch(type) {
      case WStype_DISCONNECTED:
        break;
      case WStype_CONNECTED:
        {
          /* StationID */
          webSocketServer.sendTXT(num, "{\"thisStationID\":" + String(thisStationID)+"}");

          /* AccessPoints */
          accessPoints.wssBroadcast();

          /* Network */
          network.wssBroadcast();

          webSocketServer.sendTXT(num, "{\"EndOfData\":1}");
          break;
        }
      case WStype_TEXT:
          if (dataIn.success()) {
            const char* command = dataIn["command"];
            if (strcmp(command,"reset")==0) {
              ESP.reset();
              return;
            }
            const long id = dataIn["id"];
            const byte variable = dataIn["variable"];
            const long newValue = dataIn["newValue"];
            const String newString = dataIn["newString"];
            //if (strcmp(command,"set")==0) {
            //  Objects[id]->set(variable, newValue);
            //  return;
            //}
            if (strcmp(command,"setPreferredAP")==0) {
              accessPoints.setPreferredAP(dataIn["newValue"]);
            }
            if (strcmp(command,"setAPPassword")==0) {
              accessPoints.setPassword(dataIn["id"],dataIn["newString"]);
            }
            if (strcmp(command,"setAPConnectIfAvailable")==0) {
              accessPoints.setConnectIfAvailable(dataIn["id"],dataIn["newValue"]);
            }

            if (strcmp(command,"setAPtimeIP0")==0) {
              accessPoints.setAPtimeIP0(dataIn["id"],dataIn["newValue"]);
            }
            if (strcmp(command,"setAPtimeIP1")==0) {
              accessPoints.setAPtimeIP1(dataIn["id"],dataIn["newValue"]);
            }
            if (strcmp(command,"setAPtimeIP2")==0) {
              accessPoints.setAPtimeIP2(dataIn["id"],dataIn["newValue"]);
            }
            if (strcmp(command,"setAPtimeIP3")==0) {
              accessPoints.setAPtimeIP3(dataIn["id"],dataIn["newValue"]);
            }
            if (strcmp(command,"setAPtimeHost")==0) {
              accessPoints.setAPtimeHost(dataIn["id"],dataIn["newString"]);
            }
            if (strcmp(command,"setAPtimeURL")==0) {
              accessPoints.setAPtimeURL(dataIn["id"],dataIn["newString"]);
            }

            if (strcmp(command,"softAPOnly")==0) {
              network.setToAPOnly();
            }
            
          }
          break;
    }
  }
