/********************************************************************
 /js/data/data.js
 ********************************************************************/

var thisStationID;
var AccessPoints=[];
var Network={};


var dataHostname;
var baseIP;

var webSocket;

function go() {
  document.oncontextmenu=function(event) {rightClick(event); return false;};
  dataHostname=window.location.hostname;
  if (dataHostname=="localhost") dataHostname="192.168.168.53"; // get data from .53 if website is on computer
  for (var x=dataHostname.length; dataHostname.charAt(x)!="."; x--);
  baseIP=dataHostname.substring(0, x+1)

  drawAPSetupTable();

  requestData();
}

function requestData() {
  if (typeof(webSocket)!="undefined") webSocket.close();
  switch (dataHostname) {
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
    //<< TODO:  Change UI and attempt to re-connect somehow
  }
  webSocket.onerror = function() {
    console.error("webSocket " + this.url + " error!");
    //<< TODO:  Provide feedback or something
  }
  webSocket.onmessage = function(event) {
    var Data = JSON.parse(event.data);
	/* thisStationID */
	if (typeof(Data["thisStationID"]) !="undefined") {
		thisStationID=Data.thisStationID;
	}
	/* AccessPoint */
	if (typeof(Data["AccessPoint"]) !="undefined") {
		var exists=(AccessPoints[Data["AccessPoint"].id]);
		AccessPoints[Data["AccessPoint"].id]=Data["AccessPoint"];
		if (!exists) {
			drawAPSetupTable();
		} else {
			updateAPSetupListingRow(Data["AccessPoint"].id);
		}
	}
	/* Network */
	if (typeof(Data["Network"]) !="undefined") {
		Network=Data["Network"];
		updateWifiSetupHeader(-1);
	}
    /* ObjectTypes */
    if (typeof(Data["ObjectTypes"]) != "undefined") {
    	//<< TODO
	}
    /* Obj */
    if (typeof(Data["Obj"])!="undefined") {
    	//<< TODO
    }
    /* EndOfData */
    if (typeof(Data["EndOfData"])!="undefined") {
		console.group("Data Received:");
		var nAccessPoints=0;
		AccessPoints.forEach(function(o) {nAccessPoints++});
			
		console.group("StationID: "+thisStationID);
		console.groupEnd();

		console.groupCollapsed("AccessPoints ["+nAccessPoints+"]");
		console.log(AccessPoints);
		console.groupEnd();

		console.groupCollapsed("Network");
		console.log(Network);
		console.groupEnd();
    }
  };
}
