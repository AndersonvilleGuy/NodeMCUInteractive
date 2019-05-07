function updateContextMenu(ev, E) {
  var cmType="";
  if (E.id=="map") {
    cmType="map";
  } else {
    cmType="icon";
  }
  var e="";
  switch (cmType) {
    case "icon":
      var icon=Icons[E.getAttribute("iconID")];
      var obj=Objects[icon.objectID];
      e+= "<ul class='menu-options'>";
      e+= "<li class='menu-title'>"+icon.name+"</li>";
      e+= "<li class='menu-option' onclick='editIcon("+icon.iconID+");'>Edit</li>";
      
      e+= "<li class='menu-title'>"+Objects[obj.id].objTypeName+" #"+obj.id+"</li>";
      //e+= "<li class='menu-title'>Object #"+obj["id"]+"</li>";
      e+= "<li class='menu-option'>Option 3</li>";
      e+= "<li class='menu-option'>Option 4</li>";
      e+= "</ul>";
      break;
    case "map":
      e+= "<ul class='menu-options'>";
      e+= "<li class='menu-title'>Map Options</li>";
      e+= "<li class='menu-option'>Option 1</li>";
      e+= "<li class='menu-option'>Option 2</li>";
      e+= "</ul>";
      break;
  }
  cm=document.getElementById("contextMenu");

  document.getElementById("contextMenu").innerHTML=e;
  document.getElementById("contextMenu").style.display="block";
  
  var left=ev.clientX-ev.offsetX;
  var top=ev.clientY-ev.offsetY;
  if (left+cm.offsetWidth>windowWidth) left=windowWidth-cm.offsetWidth-6;
  if (top+cm.offsetHeight>windowHeight) top=windowHeight-cm.offsetHeight-6;
  
  cm.style.removeProperty("left");
  cm.style.removeProperty("top");
  cm.style.setProperty("left",left+"px");
  cm.style.setProperty("top",top+"px");
  
}

function rightClick(ev) {
  var EL=ev.target;
  
  var targetType="";
  if (EL.tagName=="ICON") {
    targetType="icon";
  } else {
    if (EL.tagName=="DIV") {
      if (EL.className=="mapArrow") targetType="mapArrow";
      if (EL.className=="mapArea") targetType="mapArea";
      if (EL.id=="mapImage") targetType="mapImage";
    }
  }
  if (targetType==="") {
    //no real right-click target
    document.getElementById("contextMenu").style.display="none";
    return;
  }
  
  console.log ("function rightClick(ev) needs to be implemented for EL with targetType = "+targetType);
  updateContextMenu(ev, EL);
  
  return;

  if (!EL.hasAttribute("objectID")) 
    while (!(EL.hasAttribute("objectID") || EL.id=="map")) EL=EL.parentElement;
  updateContextMenu(EL);
  document.getElementById("contextMenu").style.top=ev.pageY+"px";
  document.getElementById("contextMenu").style.left=ev.pageX+"px";
  
  mapWidth=document.getElementById("map").offsetWidth;
  mapHeight=document.getElementById("map").offsetHeight;
  cmWidth=document.getElementById("contextMenu").offsetWidth;
  cmHeight=document.getElementById("contextMenu").offsetHeight;
  cmTop=document.getElementById("contextMenu").offsetTop;
  cmLeft=document.getElementById("contextMenu").offsetLeft;
  if (cmLeft+cmWidth>mapWidth) cmLeft=mapWidth-cmWidth-5;
  if (cmTop+cmHeight>mapHeight) cmTop=mapHeight-cmHeight-5;
  
  document.getElementById("contextMenu").style.top=cmTop+"px";
  document.getElementById("contextMenu").style.left=cmLeft+"px";

  document.getElementById("contextMenu").style.display="block";
}

function editIcon(id) {
  editWindowType="icon";
  editWindowID=id;
  var icon=Icons[id];
  if (!icon) return;
  var AllIconElements=document.getElementsByTagName("ICON");
  var nIconElements=AllIconElements.length;
  for (var i=0; i<nIconElements; i++) {
    if (AllIconElements[i].getAttribute("iconID")==id) {
      AllIconElements[i].setAttribute("editing","1");
    } else {
      AllIconElements[i].setAttribute("editing","0");
    }
  }
  var ew=document.getElementById("editWindow")
  ew.innerHTML=iconEditWindowHTML(icon);
  ew.style.setProperty("display","block");
  
}

function iconEditWindowHTML(icon) {
  var e="";
  e += "<table class='editWindowTable'><tr><td>";
  e += "<table>";
  e += "<tr><th colspan=2>Icon #"+icon.iconID+"</th></tr>";
  e += "<tr><th colspan=2>"+icon.name+"</th></tr>";
  e += "<tr><td>&nbsp;</td></tr>";
  e += "<tr><td class='label'>Name:</td><td>input here...</td>";
  e += "<tr><td class='label'>Image:</td><td>input here...</td>";
  e += "<tr><td class='label'>Map Number:</td><td>input here...</td>";
  e += "<tr><td class='label'>xPercentage:</td><td>"+customSpinnerHTML("xPercentage",icon.xPercentage,0.1,100,.1,5,1)+"</td>";
  e += "<tr><td class='label'>yPercentage:</td><td>"+customSpinnerHTML("yPercentage",icon.yPercentage,0.1,100,.1,5,1)+"</td>";
  e += "<tr><td class='label'>size:</td><td>"+customSpinnerHTML("size",icon.size,0.05,1,.01,.1,2)+"</td>";
  e += "<tr><td class='label'>lightRadius:</td><td>"+customSpinnerHTML("lightRadius",icon.lightRadius,1,10,.25,1,2)+"</td>";
  e += "<tr><td class='label'>rotate:</td><td>"+customSpinnerHTML("rotate",icon.rotate,0,359,1,5,0)+"</td>";
  e += "<tr><td>&nbsp;</td></tr>";
  e += "<tr><td></td><td><input type='submit' value='Apply'/>&nbsp;<input type='reset' value='Revert' onclick='revertValues("+icon.iconID+");'/>&nbsp;<input type='button' value='Cancel' onclick='closeEditWindow("+icon.iconID+");'/>";  
  e += "</table>";
  e += "</td></tr></table>";
  return e;
}

function closeEditWindow(iconID) {
  var AllIconElements=document.getElementsByTagName("ICON");
  var nIconElements=AllIconElements.length;
  for (var i=0; i<nIconElements; i++) {
    AllIconElements[i].setAttribute("editing","0");
  }
  //maybe do some reverting here first...
  var ew=document.getElementById("editWindow")
  ew.style.setProperty("display","none");

}

function revertValues(iconID) {
  //figure out how to best revert...since it is the intent to actually re-paint each time any value changes...
  
}