/********************************************************************
 /js/setup/setup.js
 ********************************************************************/

function drawAPSetupTable(pwID=-1) {
	document.getElementById("APSetup").innerHTML="<div id='APSetupHeading'></div><div id='APSetupListing'></div><div id='APSetupForm'></div>";
	document.getElementById("APSetupHeading").innerHTML = wifiSetupHeaderHTML();
	if (pwID==-1) {
		document.getElementById("APSetupListing").innerHTML=APSetupListingRowsHTML();
	} else {
		document.getElementById("APSetupListing").innerHTML="the form";
	}
	return;
}

function wifiSetupHeaderHTML() {
	var e="<table id='wifiSetupHeader' class='heading'";
	if (Network) {
		e += " scanning='" + Network.scanning + "'";
		e += " style=\"";
		e += " --status:" + Network.status + ";";
		e += " --localIP:'" + Network.localIP + "';";
		e += " --macAddress:'" + Network.macAddress + "';"
		e += "\"";
	}
	e += ">";
	e += "<tr><th rowspan=2 onclick='exitWifiSetup();'></th><th colspan=2></th><th rowspan=2 onclick='scanNow();'></th></tr>";
	e += "<tr><th colspan=2></th>";
	e += "<tr><th colspan=2></th><th colspan=2></th></tr>";
	e += "</table>";
	return e;
}

function updateWifiSetupHeader() {
	var e=document.getElementById("wifiSetupHeader");
	e.style.removeProperty("--status");
	e.style.removeProperty("--localIP");
	e.style.removeProperty("--macAddress");
	var status="NOT CONNECTED";
	switch (Network.status) {
		case 1:
			status="Access Point Only";
		case 2:
			status="On Preferred Network";
		case 3:
			status="On Non-Preferred Network";
		case 4:
			status="On No-Connect Network";
	}
	e.style.setProperty("--status","'"+status+"'");
	e.style.setProperty("--localIP","'"+Network.localIP+"'");
	e.style.setProperty("--macAddress","'"+Network.macAddress+"'");
	e.setAttribute("scanning",Network.scanning);
}

function APSetupListingRowsHTML() {
	var e="<table>";
	AccessPoints.forEach(function(ap) {
		e += APSetupListingRowHTML(ap.id);
	});
	e += "</table>";
	return e;
}

function APSetupListingRowHTML(id) {
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
	e+=" --SSID:'" + ap.SSID + "';";
	e+=" --macAddress:'" + ap.macAddress + "';";
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
	signalStrength += " ["+ap.RSSI+"]";
	e+=" --signalStrength:'" + signalStrength + "';";
	e+="\">";
	e+="<td";
	e += " onclick='setPreferredAP(this);'";
	e += "></td>";
	e+="<td><div></div>";
	e+="<div></div></td>";
	e+="<td><div></div><div></div></td><td onclick='APSetupForm(this);'></td></tr>";
	return e;
}

function updateAPSetupListingRow(id) {
	var allAPSetupRows = document.getElementById("APSetupListing").getElementsByTagName("TR");
	var nAPSetupRows = allAPSetupRows.length;
	for(var i=0; i<nAPSetupRows; i++) {
		if (allAPSetupRows[i].getAttribute("id")==id) {
			var el=allAPSetupRows[i];
			var ap=AccessPoints[id];
			el.setAttribute("currentScanIndex", ap.currentScanIndex);
			el.setAttribute("preferredAP", ap.preferredAP);
			el.setAttribute("connectedNow", ap.connectedNow);
			el.setAttribute("isOpen", ap.isOpen);
			el.setAttribute("passwordKnown", ap.passwordKnown);
			el.setAttribute("passwordFailed", ap.passwordFailed);
			el.setAttribute("connectIfAvailable", ap.connectIfAvailable);
			el.setAttribute("encryptionType", ap.encryptionType);
			var encryptionTypeString="";
			var isOpen="0";
			switch(ap.encryptionType) {
				case 5:		encryptionTypeString+="WEP";				break;
				case 2:		encryptionTypeString+="WPA/PSK";			break;
				case 4:		encryptionTypeString+="WPA2/PSK";			break;
				case 7:		encryptionTypeString+="OPEN";				isOpen="1";		break;
				case 8:		encryptionTypeString+="AUTO";				break;
			}
			el.style.removeProperty("--macAddress");
			el.style.setProperty("--macAddress","'"+ap.macAddress+"'");
			el.style.removeProperty("--channel");
			el.style.setProperty("--channel",ap.channel);
			el.style.removeProperty("--RSSI");
			el.style.setProperty("--RSSI",ap.RSSI);
			el.style.removeProperty("--encryptionType");
			el.style.setProperty("--encryptionType","'"+encryptionTypeString+"'");
			el.style.removeProperty("--isHidden");
			if (ap.isHidden) {
				el.style.setProperty("--isHidden","Y");
			} else {
				el.style.setProperty("--isHidden","N");
			}
			el.style.removeProperty("--preferredAP");
			el.style.setProperty("--preferredAP",ap.preferredAP);
			var signalStrength="Weak";
			if (ap.RSSI>=-70) signalStrength="Fair";
			if (ap.RSSI>=-60) signalStrength="Good";
			if (ap.RSSI>=-50) signalStrength="Excellent";
			if (ap.RSSI>=-40) signalStrength="Amazing";
			signalStrength += " ["+ap.RSSI+"]";
			el.style.removeProperty("--signalStrength");
			el.style.setProperty("--signalStrength","'"+signalStrength+"'");
		}
	}
}

function updateAPSetupForm(id) {
	var ap=AccessPoints[id];
	var e="<table>";
		e += "<tr><td>"+ap.SSID+"</td></tr>";
		e += "<tr><td>";
		e += "<input id='PWid' type='hidden' value='" + id + "'>";
		e += "<b>Password:</b>&nbsp<input type='text' id='PWpw' value='" + ap.password + "'>";
		e += "</td></tr>"
		e += "<tr><td id='connectIfAvailableTD'><input type='checkbox' id='connectIfAvailable'";
		if (ap.connectIfAvailable) e += " checked";
		e += " onchange='connectIfAvailableChanged();'> Connect If Available</td></tr>";

		e += "<tr><td id='TimeParameters' colspan=4></td></tr>";

		e += "<tr><td>";
		e += "<b>IP Address:</b>&nbsp;";
		e += "<input id='timeIP0' type='number' min=0 max=255 value=" + ap.apTimeIP0;
			if (!ap.connectIfAvailable) e+= " disabled";
			e += "><b>.</b>";
		e += "<input id='timeIP1' type='number' min=0 max=255 value=" + ap.apTimeIP1;
			if (!ap.connectIfAvailable) e+= " disabled";
			e += "><b>.</b>";
		e += "<input id='timeIP2' type='number' min=0 max=255 value=" + ap.apTimeIP2;
			if (!ap.connectIfAvailable) e+= " disabled";
			e += "><b>.</b>";
		e += "<input id='timeIP3' type='number' min=0 max=255 value=" + ap.apTimeIP3;
			if (!ap.connectIfAvailable) e+= " disabled";
			e += ">";
		e += "</td></tr>";

		e += "<tr><td>";
		e += "<b>Host:&nbsp;";
		e += "<input id='timeHost' type='text' value='" + ap.apTimeHost + "'";
			if (!ap.connectIfAvailable) e+= " disabled";
			e += ">";
		e += "</td></tr>";

		e += "<tr><td>";
		e += "<b>URL:&nbsp;";
		e += "<input id='timeURL' type='text' value='" + ap.apTimeURL + "'";
			if (!ap.connectIfAvailable) e+= " disabled";
			e += ">";
		e += "</td></tr>";

		e += "<tr><td>";
		e += "<input type='button' value='CHANGE' onclick='submitAPPW();'>&nbsp;&nbsp;";
		e += "<input type='button' value='Clear Password' onclick='document.getElementById(\"PWpw\").setAttribute(\"value\",\"\");'>&nbsp;&nbsp;";
		e += "<input type='button' value='Cancel' onclick='document.getElementById(\"APSetup\").setAttribute(\"mode\",\"Listing\");'>"
		e += "</td></tr>";
	e += "</table>";
	document.getElementById("APSetupForm").innerHTML=e;
}

function connectIfAvailableChanged() {
	var allInputElements=document.getElementById("APSetupForm").getElementsByTagName("INPUT");
	var nInputElements=allInputElements.length;
	for (var i=3; i<=8; i++) {
		allInputElements[i].disabled=!allInputElements[2].checked;
	}
}

function setPreferredAP(el) {
	var id=el.parentElement.getAttribute("id");
	if (!AccessPoints[id].connectIfAvailable) return;
	var cmd="{\"command\":\"setPreferredAP\",\"newValue\":"+id+"}";
	webSocket.send(cmd);	
	
}

function APSetupForm(el) {
	var id=el.parentElement.getAttribute("id");
	updateAPSetupForm(id);
	document.getElementById("APSetup").setAttribute("mode","Form");
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

	document.getElementById("APSetup").setAttribute("mode","Listing");
}

function exitWifiSetup() {
	console.error("exitWifiSetup() -> Need to Write")
}

function scanNow() {
	if (Network.scanning) return;
	console.error("scanNow() -> Need to Write");
}