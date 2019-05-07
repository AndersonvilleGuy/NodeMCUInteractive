/*
<icon iconid="1" objectid="105104" imageid="3" mapnumber="1" state="0" yPercentage="0.693" xPercentage="0.455" onmouseover="mouseMovement(this,1);" onmouseout="mouseMovement(this,0);" style="--objectID:'105104';--top:69.3%;--left:45.5%;--size:10px;--diameter:32px;--rotation:45deg;--border-radius:24px;--background-position:16px; background-color: rgba(255,255,0,.5);" mouseover="0"><div></div></icon>
<icon iconid="9" objectid="105105" imageid="3" mapnumber="1" state="0" yPercentage="0.115" xPercentage="0.141" onmouseover="mouseMovement(this,1);" onmouseout="mouseMovement(this,0);" style="--objectID:'105104';--top:12%;--left:14%;--size:10px;--diameter:32px;--rotation:45deg;--border-radius:24px;--background-position:16px; background-color: rgba(255,255,0,.5);" mouseover="0"><div></div></icon>
*/

/*
var Icons = [{iconID: 1, name: "StreetLight - Hall Main Entrance", objectID: 105104, imageID: 3, mapNumber: 0, state: 1, xPercentage: 45.6, yPercentage: 69.2, size: .65, lightRadius: 3, rotate: 45},
             {iconID: 9, name: "Streetlight - Guard Hut", objectID: 105105, imageID: 3, mapNumber: 0, state: 1, xPercentage: 60.4, yPercentage: 86.5, size: .65, lightRadius: 3, rotate: 45},
             {iconID: 10, name: "Streetlight - Electric Hut", objectID: 105105, imageID: 3, mapNumber: 0, state: 1, xPercentage: 57.4, yPercentage: 75.5, size: .65, lightRadius: 3, rotate: 45},
             {iconID: 11, name: "Streetlight - Small Parking", objectID: 105105, imageID: 3, mapNumber: 0, state: 1, xPercentage: 56.1, yPercentage: 63.5, size: .65, lightRadius: 3, rotate: 45},
             {iconID: 6, name: "Gate Light - Middle", objectID: 999999, imageID: 4, mapNumber: 0, state: 1, xPercentage: 66.7, yPercentage: 85.4, size: .75, lightRadius: 1.25, rotate: 90},  
             {iconID: 7, name: "Gate Light - Right", objectID: 999999, imageID: 4, mapNumber: 0, state: 1, xPercentage: 67.6, yPercentage: 84.6, size: .75, lightRadius: 1.25, rotate: 90},  
             {iconID: 8, name: "Gate Light - Left", objectID: 999999, imageID: 4, mapNumber: 0, state: 1, xPercentage: 67.6, yPercentage: 86.2, size: .75, lightRadius: 1.25, rotate: 90}  
            ];
*/

function drawMapIcons() {
  var mapIconsElement=document.getElementById("mapIcons");
  var e="";
  for (var i=0; i<Icons.length; i++) {
    if (Icons[i]) {
      if (Icons[i].mapNumber==currentMapIndex) {
        e+=mapIconHTML(Icons[i]);
      }
    }
  }
  mapIconsElement.innerHTML=e;
}

function mapIconHTML(ic) {
  var e="<icon"
  e+=" iconID='"+ic.iconID+"'";
  e+=" imageID='"+ic.imageID+"'";
  e+=" objectID='"+ic.objectID+"'";
  e+=" state='"+Objects[ic.objectID].state+"'";
  e+=" value='"+Objects[ic.objectID].value+"'";
  e+=" title='"+ic.name+"'";
  e+=" style=\"";
  e+="--iconid: "+ic.iconID+";";
  e+="--objectID: "+ic.objectID+";";
  e+="--imageID: "+ic.imageID+";";
  e+="--mapNumber: "+ic.mapNumber+";";
  e+="--state: "+Objects[ic.objectID].state+";";
  e+="--value: '"+Objects[ic.objectID].value+"';";
  e+="--size: "+(ic.size*12)+"px;";
  e+="--lightRadius: "+(ic.lightRadius*ic.size*12)+"px;";
  e+="--left: "+(zoomedImageWidth*((ic.xPercentage/100)-(imageLeftPercentage/100)))+"px;";
  e+="--top: "+(zoomedImageHeight*((ic.yPercentage/100)-(imageTopPercentage/100)))+"px;";
  e+="--rotate: "+ic.rotate+"deg;";
  /*
  e+="---xPercentage: "+(ic.xPercentage-imageLeftPercentage)+"%;";
  e+="---yPercentage: "+(ic.yPercentage-imageTopPercentage)+"%;";
  e+="---xPercentageBeforePush: "+(ic.xPercentage)+"%;";
  e+="---yPercentageBeforePush: "+(ic.yPercentage)+"%;";
  e+="---imageLeftPercentage: "+(imageLeftPercentage)+"%;";
  e+="---imageTopPercentage: "+(imageTopPercentage)+"%;";
  e+="---backgroundPositionX: "+(backgroundPositionX)+";";
  e+="---backgroundPositionY: "+(backgroundPositionY)+";";
  e+="---clippedImageWidth: "+(clippedImageWidth)+"px;";
  e+="---clippedImageHeight: "+(clippedImageHeight)+"px;";
  e+="---zoomedImageWidth: "+(zoomedImageWidth)+"px;";
  e+="---zoomedImageHeight: "+(zoomedImageHeight)+"px;";
  */
  e+="\"";
	e+= "onclick='iconClick(this);'";
	e+=">";
	e+="</icon>";
  return e;
}

function updateIcons(id) {
  console.info("updateIcons("+id+")");
  var AllIcons=document.getElementsByTagName("ICON");
  var nIcons=AllIcons.length;
  for (var i=0; i<nIcons; i++) {
    if (AllIcons[i].hasAttribute("objectID")) {
      if (AllIcons[i].getAttribute("objectID")==id) {
        if (typeof(Objects[id].state)!='undefined') {
          AllIcons[i].setAttribute("state",Objects[id].state);
          AllIcons[i].style.removeProperty("--state");
          AllIcons[i].style.setProperty("--state",Objects[id].state);
        }
        if (typeof(Objects[id].value)!='undefined') {
          AllIcons[i].setAttribute("value",Objects[id].value);
          AllIcons[i].style.removeProperty("--value");
          AllIcons[i].style.setProperty("--value","'"+Objects[id].value+"'");
        }
      }
    }
  }
}

function iconClick(el) {
	var iconID=el.getAttribute("iconID");
	var objectID=el.getAttribute("objectID")
	var icon=Icons[iconID];
	var object=Objects[objectID];
	console.info("iconClick("+iconID+")");
	var cmd="{\"command\":\"set\",\"variable\":5,\"newValue\":";
	if (object.state==0) {
		cmd += "1";
	}	else {
		cmd += "0";
	}
	cmd += ",\"id\":";
	cmd += objectID;
	cmd += "}";
	console.info(cmd);
	webSocket.send(cmd);
}