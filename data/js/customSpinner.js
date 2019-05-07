function customSpinnerHTML(name, value, min, max, step, bigStep, decimals) {
  var e=""
  e+="<div class='customSpinner'"
  e+=" name='"+name+"'";
  e+=" value='"+Number(value).toFixed(decimals)+"'";
  e+=" min='"+min+"'";
  e+=" max='"+max+"'";
  e+=" step='"+step+"'";
  e+=" bigStep='"+bigStep+"'";
  e+=" decimals='"+decimals+"'";
  e+=">";
    e+="<div onclick='spinClick(this,-1,1);'>&nbsp;</div>";
    e+="<div onclick='spinClick(this,-1,0);'>&nbsp;</div>";
    e+="<div>"+Number(value).toFixed(decimals)+"&nbsp;</div>";
    e+="<div onclick='spinClick(this,+1,0);'>&nbsp;</div>";
    e+="<div onclick='spinClick(this,+1,1);'>&nbsp;</div>";
  e+="</div>";
  return e;
}
function spinClick(el, direction, isBig) {
  el=el.parentElement;
  direction=Number(direction);
  isBig=Number(isBig);
  var value=Number(el.getAttribute("value"));
  var min=Number(el.getAttribute("min"));
  var max=Number(el.getAttribute("max"));
  var step=Number(el.getAttribute("step"));
  var bigStep=Number(el.getAttribute("bigStep"));
  var decimals=Number(el.getAttribute("decimals"));
  var newValue;
  switch(isBig) {
    case 1:
      newValue=value+(direction*bigStep);
      break;
    default:
      newValue=value+(direction*step);
  }
  if (newValue<min) newValue=min;
  if (newValue>max) newValue=max;
  el.children[2].innerHTML=newValue.toFixed(decimals);
  el.setAttribute("value",newValue.toFixed(decimals));
  switch (editWindowType) {
    case "icon":
      var id=Number(editWindowID);
      var icon=Icons[id];
      var attribute=el.getAttribute("name");
      if (typeof(icon)!="undefined") {
        Icons[id][attribute]=newValue.toFixed(decimals);
        mapResize();
        var AllIconElements=document.getElementsByTagName("ICON");
        var nIconElements=AllIconElements.length;
        for (var i=0; i<nIconElements; i++) {
          if (AllIconElements[i].getAttribute("iconID")==id) {
            AllIconElements[i].setAttribute("editing","1");
          } else {
            AllIconElements[i].setAttribute("editing","0");
          }
        }
      }
      break;
  }
}
