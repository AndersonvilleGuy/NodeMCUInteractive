var thisStationID;
var AccessPoints=[];
var Network={};

var hostname;
var baseIP;
var ObjectTypes=[];
var Objects=[];
var Pins=[];
var Stations=[];
var ControllerMonitors=[];
var Relays=[];
var Buttons=[];
var Icons=[];
var webSocket;
var pinLabels=["GPIO16","GPIO5","GPIO4~","GPIO0","GPIO2","GPIO14~","GPIO12~","GPIO13","GPIO15~","GPIO3","GPIO1","ADC0"];
function setup() {
  
}
function go() {
  hostname=window.location.hostname;
	hostname="192.168.168.53";
  for (var x=hostname.length; hostname.charAt(x)!="."; x--);
  baseIP=hostname.substring(0, x+1)
  setup();
  requestData();
  document.oncontextmenu=function(event) {rightClick(event); return false;};
}
function requestData() {
  if (typeof(webSocket)!="undefined") webSocket.close();
  switch (hostname) {
    case "192.168.168.51":  webSocket = new WebSocket('ws://192.168.168.51:81/web', ['arduino']);  thisStationID=51; break;
    case "192.168.168.52":  webSocket = new WebSocket('ws://192.168.168.52:81/web', ['arduino']);  thisStationID=52; break;
    case "192.168.168.53":  webSocket = new WebSocket('ws://192.168.168.53:81/web', ['arduino']);  thisStationID=53; break;
    case "192.168.168.54":  webSocket = new WebSocket('ws://192.168.168.54:81/web', ['arduino']);  thisStationID=54; break;
    case "192.168.168.55":  webSocket = new WebSocket('ws://192.168.168.55:81/web', ['arduino']);  thisStationID=55; break;
    case "192.168.168.56":  webSocket = new WebSocket('ws://192.168.168.56:81/web', ['arduino']);  thisStationID=56; break;
    case "192.168.168.57":  webSocket = new WebSocket('ws://192.168.168.57:81/web', ['arduino']);  thisStationID=57; break;
    case "192.168.168.58":  webSocket = new WebSocket('ws://192.168.168.58:81/web', ['arduino']);  thisStationID=58; break;
    case "192.168.168.59":  webSocket = new WebSocket('ws://192.168.168.59:81/web', ['arduino']);  thisStationID=59; break;
    case "192.168.168.60":  webSocket = new WebSocket('ws://192.168.168.60:81/web', ['arduino']);  thisStationID=60; break;
    case "192.168.168.61":  webSocket = new WebSocket('ws://192.168.168.61:81/web', ['arduino']);  thisStationID=61; break;
    case "192.168.168.62":  webSocket = new WebSocket('ws://192.168.168.62:81/web', ['arduino']);  thisStationID=62; break;
    case "192.168.168.63":  webSocket = new WebSocket('ws://192.168.168.63:81/web', ['arduino']);  thisStationID=63; break;
    case "192.168.168.64":  webSocket = new WebSocket('ws://192.168.168.64:81/web', ['arduino']);  thisStationID=64; break;
    case "192.168.168.65":  webSocket = new WebSocket('ws://192.168.168.65:81/web', ['arduino']);  thisStationID=65; break;
    case "192.168.168.66":  webSocket = new WebSocket('ws://192.168.168.66:81/web', ['arduino']);  thisStationID=66; break;
  }
  webSocket.onopen = function() {
    console.groupCollapsed("webSocket open.")
    console.log(webSocket);
    console.groupEnd();
  };
  webSocket.onclose = function() {
    console.error("webSocket " + this.url + " closed!");
    document.getElementById("map").innerHTML="waiting to reconnect...";
    document.getElementById("allObjects").innerHTML="waiting to reconnect...";
    document.getElementById("stationPinObjectMap").innerHTML="waiting to reconnect...";
    requestData();
  }
  webSocket.onerror = function() {
    console.error("webSocket " + this.url + " error!");
  }
  webSocket.onmessage = function(event) {
    var Data = JSON.parse(event.data);
	/* thisStationID */
	if (typeof(Data["thisStationID"]) !="undefined") {
		thisStationID=Data.thisStationID;
	}
	/* AccessPoint */
	if (typeof(Data["AccessPoint"]) !="undefined") {
		AccessPoints[Data["AccessPoint"].id]=Data["AccessPoint"];
		reDrawSetupTable();
	}
	/* Network */
	if (typeof(Data["Network"]) !="undefined") {
		Network=Data["Network"];
		reDrawSetupTable();
	}


		
		
    /* ObjectTypes */
    if (typeof(Data["ObjectTypes"]) != "undefined") {
      console.group("ObjectTypes received:");
      var nObjectTypes=0;
      for (var ot=0; ot<Data["ObjectTypes"].length; ot++) {
        ObjectTypes[Data["ObjectTypes"][ot]["objTypeID"]] = Data["ObjectTypes"][ot];
        nObjectTypes++;
      }
      
      // temporary to make Stations and Relays into Icons instead <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      ObjectTypes[2]["webCategory"]="Icon";
      ObjectTypes[8]["webCategory"]="Icon";

      console.groupCollapsed("ObjectTypes ["+nObjectTypes+"]");
      console.log(ObjectTypes);
      console.groupEnd();
      console.groupEnd();
    }
    /* Obj */
    if (typeof(Data["Obj"])!="undefined") {
      var id=Data["Obj"]["id"];
      Objects[id]=Data["Obj"];
      Objects[id]["objTypeName"]=ObjectTypes[Objects[id]["objType"]]["objType"];
      if (typeof(Objects[id]["value"])!="undefined") {
        Objects[id]["maxValue"]=Objects[id]["value"];
        Objects[id]["percentOfMaxValue"]="100%";
      }
      
      switch (ObjectTypes[Objects[id]["objType"]]["webCategory"]) {
        case "Pin":                 Pins[id]=Objects[id];                   break;
        case "Station":             Stations[id]=Objects[id];               break;
        case "ControllerMonitor":   ControllerMonitors[id]=Objects[id];     break;
        case "Relay":               Relays[id]=Objects[id];                 break;
        case "Button":              Buttons[id]=Objects[id];                break;
        //case "Icon":                Icons[id]=Objects[id];                  break;
      }
    }
    /* EndOfData */
    if (typeof(Data["EndOfData"])!="undefined") {
      
			Objects.forEach(function(o) {
        if (o["objType"]!="2") {
          if (o.hasOwnProperty("stationID"))
            o["station"]=Objects[o["stationID"]];
        }
      })
      Objects.forEach(function(o) {
        if (o.hasOwnProperty("pinID"))
          o["pin"]=Objects[o["pinID"]];
          if (typeof(o["pin"])!="undefined") {
            o["deciVolts"]=o["pin"]["deciVolts"];
            o["volts"]=o["pin"]["volts"];
          }
      })
      console.group("Data Received:");
			var nAccessPoints=0;
			
      var nObjects=0;
      var nPins=0;
      var nStations=0;
      var nControllerMonitors=0;
      var nRelays=0;
      var nButtons=0;
			
			AccessPoints.forEach(function(o) {nAccessPoints++});
      Objects.forEach(function(o) {nObjects++});
      Pins.forEach(function(o) {nPins++});
      Stations.forEach(function(o) {nStations++});
      ControllerMonitors.forEach(function(o) {nControllerMonitors++});
      Relays.forEach(function(o) {nRelays++});
      Buttons.forEach(function(o) {nButtons++});
			
			console.group("StationID: "+thisStationID);
			console.groupEnd();

			console.groupCollapsed("AccessPoints ["+nAccessPoints+"]");
			console.log(AccessPoints);
			console.groupEnd();

			console.groupCollapsed("Network");
			console.log(Network);
			console.groupEnd();
			
      console.group("Objects ["+nObjects+"]");
      console.groupCollapsed("Pins ["+nPins+"]");
      console.log(Pins);
      console.groupEnd();
      console.groupCollapsed("Stations ["+nStations+"]");
      console.log(Stations);
      console.groupEnd();
      console.groupCollapsed("ContollerMonitors ["+nControllerMonitors+"]");
      console.log(ControllerMonitors);
      console.groupEnd();
      console.groupCollapsed("Relays ["+nRelays+"]");
      console.log(Relays);
      console.groupEnd();
      console.groupCollapsed("Buttons ["+nButtons+"]");
      console.log(Buttons);
      console.groupEnd();
      console.groupEnd();
      console.groupEnd();
      console.groupEnd();
      console.groupEnd();
      //drawMapIcons();
    }
    /* Icon */
    if (typeof(Data["Icon"])!="undefined") {
      var iconID=Data["Icon"]["iconID"];
      Icons[iconID]=Data["Icon"];
    }
    /* IconEnd */
    if (typeof(Data["IconEnd"])!="undefined") {
      console.group("Icons Received:");
      var nIcons=0;
      Icons.forEach(function(o) {nIcons++});
      console.groupCollapsed("Icons ["+nIcons+"]");
      console.log(Icons);
      console.groupEnd();
      console.groupEnd();
      drawMapIcons();
    }
    /* ObjectValueUpdate */
    if (typeof(Data["ObjectValueUpdate"]) != "undefined") {
      var vu=Data["ObjectValueUpdate"][0];
      var id=vu["id"];
      //console.log("Updating #"+id);
      //console.log(vu);
      Object.keys(vu).forEach(function(key,index) {
        if (key!="id")
          Objects[id][key]=vu[key];
          //console.log(key+"::"+vu[key]);
      });
      if (typeof(Objects[id]["value"])!="undefined") {
        if (Objects[id]["value"]>Objects[id]["maxValue"]) Objects[id]["maxValue"]=Objects[id]["value"];
        Objects[id]["percentOfMaxValue"]=(100*Objects[id]["value"]/Objects[id]["maxValue"])+"%";
      }
      updateIcons(id);
    }
  };
}

function reDrawSetupTable(pwID=-1) {
	var setupElement=document.getElementById("APSetup");
	if (pwID==-1 && setupElement.getAttribute("mode")=="PasswordForm") return;
	var e="<table><tr><th colspan=4></th></tr>";
	switch (pwID) {
		case -1:
			setupElement.setAttribute("mode","DisplayAPs");
			e += "<tr networkStatus= " + Network.status + "><th colspan=4></th></tr>"

			AccessPoints.forEach(function(ap) {
				e += APSetupRow(ap.id);
			});
			e+="</table>";
			break;
		default:
			setupElement.setAttribute("mode","PasswordForm");
			e += APSetupRow(pwID);
			e += "<tr><td colspan=4>";
			e += "<input id='PWid' type='hidden' value='" + pwID + "'>";
			e += "<b>Password:</b>&nbsp<input type='text' id='PWpw' value='" + AccessPoints[pwID].password + "'>";
			e += "</td></tr>"
			e += "<tr><td colspan=4 id='connectIfAvailableTD'><input type='checkbox' id='connectIfAvailable'";
			if (AccessPoints[pwID].connectIfAvailable) e += " checked";
			e += "> Connect If Available</td></tr>";

			e += "<tr><td id='TimeParameters' colspan=4></td></tr>";

			e += "<tr><td colspan=4>";
			e += "<b>IP Address:</b>&nbsp;";
			e += "<input id='timeIP0' type='number' min=0 max=255 value=" + AccessPoints[pwID].apTimeIP0 + "><b>.</b>";
			e += "<input id='timeIP1' type='number' min=0 max=255 value=" + AccessPoints[pwID].apTimeIP1 + "><b>.</b>";
			e += "<input id='timeIP2' type='number' min=0 max=255 value=" + AccessPoints[pwID].apTimeIP2 + "><b>.</b>";
			e += "<input id='timeIP3' type='number' min=0 max=255 value=" + AccessPoints[pwID].apTimeIP3 + ">";
			e += "</td></tr>";

			e += "<tr><td colspan=4>";
			e += "<b>Host:&nbsp;";
			e += "<input id='timeHost' type='text' value='" + AccessPoints[pwID].apTimeHost + "'>";
			e += "</td></tr>";

			e += "<tr><td colspan=4>";
			e += "<b>URL:&nbsp;";
			e += "<input id='timeURL' type='text' value='" + AccessPoints[pwID].apTimeURL + "'>";
			e += "</td></tr>";

			e += "<tr><td colspan=4>";
			e += "<input type='button' value='Change' onclick='submitAPPW();'>&nbsp;";
			e += "<input type='button' value='Clear Password' onclick='document.getElementById(\"PWpw\").setAttribute(\"value\",\"\");'>&nbsp;";
			e += "<input type='button' value='Cancel' onclick='document.getElementById(\"APSetup\").setAttribute(\"mode\",\"\"); reDrawSetupTable();'>"
			e += "</td></tr>";
			e += "<table>";
			break;
	}
	setupElement.innerHTML=e;
}

function APSetupRow(id) {
	var e="";
	var ap=AccessPoints[id];
	var encryptionTypeString="";
	var isOpen="0";
	switch(ap.encryptionType) {
		case 5:		encryptionTypeString+="WEP";				break;
		case 2:		encryptionTypeString+="WPA/PSK";			break;
		case 4:		encryptionTypeString+="WPA2/PSK";			break;
		case 7:		encryptionTypeString+="OPEN";				isOpen="1";		break;
		case 8:		encryptionTypeString+="AUTO";				break;
	}
	e+="<tr";
	e+=" id='" + ap.id + "'";
	e+=" currentScanIndex='" + ap.currentScanIndex + "'";
	e+=" preferredAP='" + ap.preferredAP + "'";
	e+=" connectedNow='" + ap.connectedNow + "'";
	e+=" isOpen='" + isOpen + "'";
	e+=" passwordKnown='" + ap.passwordKnown + "'";
	e+=" passwordFailed='" + ap.passwordFailed + "'";
	e+=" connectIfAvailable='" + ap.connectIfAvailable + "'";
	e+=" encryptionType='" + ap.encryptionType + "'";
	e+=" style=\"";
	e+=" --channel:'" + ap.channel + "';";
	e+=" --RSSI:'" + ap.RSSI + "';";
	e+=" --encryptionType:'" + encryptionTypeString + "';";
	e+=" --isHidden:'";
	if (ap.isHidden) e+="Y"; else e+="N";
	e+="';";
	e+=" --preferredAP:'" + ap.preferredAP + "';";
	var signalStrength="Weak";
	if (ap.RSSI>=-70) signalStrength="Fair";
	if (ap.RSSI>=-60) signalStrength="Good";
	if (ap.RSSI>=-50) signalStrength="Excellent";
	if (ap.RSSI>=-40) signalStrength="Amazing";
	signalStrength += "&nbsp;["+ap.RSSI+"]";
	e+=" --signalStrength:'" + signalStrength + "';";
	e+="\">";
	e+="<td";
	if (ap.connectIfAvailable) e += " onclick='setPreferredAP(this);'";
	e += "></td>";
	e+="<td><div>"+ap.SSID+"</div>";
	e+="<div>"+ap.macAddress+"</div></td>";
	e+="<td><div></div><div></div></td><td onclick='setAPPassword(this);'></td></tr>";
	return e;
}

function setPreferredAP(el) {
	var APid=el.parentElement.getAttribute("id");
	var cmd="{\"command\":\"setPreferredAP\",\"newValue\":"+APid+"}";
	webSocket.send(cmd);	
	
}

function setAPPassword(el) {
	var APid=el.parentElement.getAttribute("id");
	reDrawSetupTable(APid);
}

function submitAPPW() {
	var APid=document.getElementById("PWid").value;
	var PWpw=document.getElementById("PWpw").value;
	var connectIfAvailable=document.getElementById("connectIfAvailable").checked;
	if (connectIfAvailable) connectIfAvailable="1"; else connectIfAvailable="0";
	var timeIP0=document.getElementById("timeIP0").value;
	var timeIP1=document.getElementById("timeIP1").value;
	var timeIP2=document.getElementById("timeIP2").value;
	var timeIP3=document.getElementById("timeIP3").value;
	var timeHost=document.getElementById("timeHost").value;
	var timeURL=document.getElementById("timeURL").value;

	var cmd="{\"command\":\"setAPPassword\",\"id\":"+APid+",\"newString\":\""+PWpw+"\"}";
	webSocket.send(cmd);
	var cmd="{\"command\":\"setAPConnectIfAvailable\",\"id\":"+APid+",\"newValue\":\""+connectIfAvailable+"\"}";
	webSocket.send(cmd);
	var cmd="{\"command\":\"setAPtimeIP0\",\"id\":"+APid+",\"newValue\":"+timeIP0+"}";
	webSocket.send(cmd);
	var cmd="{\"command\":\"setAPtimeIP1\",\"id\":"+APid+",\"newValue\":"+timeIP1+"}";
	webSocket.send(cmd);
	var cmd="{\"command\":\"setAPtimeIP2\",\"id\":"+APid+",\"newValue\":"+timeIP2+"}";
	webSocket.send(cmd);
	var cmd="{\"command\":\"setAPtimeIP3\",\"id\":"+APid+",\"newValue\":"+timeIP3+"}";
	webSocket.send(cmd);
	var cmd="{\"command\":\"setAPtimeHost\",\"id\":"+APid+",\"newString\":\""+timeHost+"\"}";
	webSocket.send(cmd);
	var cmd="{\"command\":\"setAPtimeURL\",\"id\":"+APid+",\"newString\":\""+timeURL+"\"}";
	webSocket.send(cmd);

	document.getElementById("APSetup").setAttribute("mode","");
	reDrawSetupTable();
}
