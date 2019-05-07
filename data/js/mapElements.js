var Maps=[{index:0, imageURL: "/img/MANGULON_MAP.png", title:"Mangulon Garden Resort", defaultMapArea: 0, imageWidth: 2550, imageHeight: 2760}];
var MapAreas=[{index:0, map: 0, title:"Mangulon Overview", left: 0, width: 100, top: 0, height: 100, overview:1},
              {index:1, map: 0, title:"Gate/Guard House",left: 52, width: 19, top: 77, height: 17.5, overview:0,
                mapArrows:[
                  {index: 0, left: 44, top: 14, rotate: -10, targetMapArea: 4}
                ]
              },
              {index:2, map: 0, title:"Hall Main Path",left: 40, width: 18, top: 57, height: 20, overview:0,
                mapArrows:[
                  {index: 0, left: 85, top: 38.5, rotate: 52, targetMapArea: 3}
                ]
              },
              {index:3, map: 0, title:"Small Parking/Office",left: 50, width: 20, top: 50, height: 20, overview:0,
                mapArrows:[
                  {index: 0, left: 39, top: 92, rotate: 180, targetMapArea: 4},
                  {index: 1, left: 3, top: 80.9, rotate: 240, targetMapArea: 2}
                ]
              },
              {index:4, map: 0, title:"Small Parking Road",left: 52, width: 18, top: 66, height: 19, overview:0,
                mapArrows:[
                  {index: 0, left: 48, top: 92, rotate: 170, targetMapArea: 1},
                  {index: 1, left: 36.5, top: 14, rotate: 15, targetMapArea: 3}
                ]
              }
              ];
              
var windowWidth;
var windowHeight;
var currentMapIndex=0;
var currentMapArea=0;
var mapScale=1;
var imageZoom=100;
var imageLeftPercentage=0;
var imageTopPercentage=0;
var imageWidthPercentage=100;
var imageHeightPercentage=100;
var backgroundPositionX=0;
var backgroundPositionY=0;
var clippedImageWidth=99;
var clippedImageHeight=99;
var zoomedImageWidth=99;
var zoomedImageHeight=99;

function mapResize() {
  //local variables for map/mapArea
  var map=Maps[currentMapIndex];
  var mapArea=MapAreas[currentMapArea];
  
  //local variables for mapContainerElement/mapImageElement
  var mapContainerElement=document.getElementById("mapContainer");
  var mapImageElement=document.getElementById("mapImage");
  var mapTitleElement=document.getElementById("mapTitle");
  var mapSubtitleElement=document.getElementById("mapSubtitle");
  var zoomOutElement=document.getElementById("zoomOut");
  
  //local variables for input variables
  var imageWidth=map.imageWidth;
  var imageHeight=map.imageHeight;
  imageLeftPercentage=mapArea.left;
  imageTopPercentage=mapArea.top;
  imageWidthPercentage=mapArea.width;
  imageHeightPercentage=mapArea.height;
  mapContainerElement.style.removeProperty("--imageWidth");
  mapContainerElement.style.removeProperty("--imageHeight");
  mapContainerElement.style.removeProperty("--imageLeftPercentage");
  mapContainerElement.style.removeProperty("--imageTopPercentage");
  mapContainerElement.style.removeProperty("--imageWidthPercentage");
  mapContainerElement.style.removeProperty("--imageHeightPercentage");
  mapContainerElement.style.setProperty("--imageWidth",imageWidth+"px");
  mapContainerElement.style.setProperty("--imageHeight",imageHeight+"px");
  mapContainerElement.style.setProperty("--imageLeftPercentage",imageLeftPercentage+"%");
  mapContainerElement.style.setProperty("--imageTopPercentage",imageTopPercentage+"%");
  mapContainerElement.style.setProperty("--imageWidthPercentage",imageWidthPercentage+"%");
  mapContainerElement.style.setProperty("--imageHeightPercentage",imageHeightPercentage+"%");
  
  //set title & subtitle
  mapContainerElement.style.removeProperty("--mapTitle");
  mapContainerElement.style.removeProperty("--mapSubtitle");
  mapContainerElement.style.setProperty("--mapTitle","'"+map.title+"'");
  mapContainerElement.style.setProperty("--mapSubtitle","'"+mapArea.title+"'");
  
  //set background-image of mapImageElement
  mapImageElement.style.removeProperty("background-image");
  mapImageElement.style.setProperty("background-image","URL('"+map.imageURL+"')");

  //determine available windowWidth/windowHeight
  windowWidth=window.innerWidth-0;
  windowHeight=window.innerHeight-0;
  mapContainerElement.style.removeProperty("--windowWidth");
  mapContainerElement.style.removeProperty("--windowHeight");
  mapContainerElement.style.setProperty("--windowWidth",windowWidth+"px");      
  mapContainerElement.style.setProperty("--windowHeight",windowHeight+"px");      

  //determine unzoomedImageWidth, unzoomedImageHeight
  var unzoomedImageWidth=imageWidth*imageWidthPercentage/100;
  var unzoomedImageHeight=imageHeight*imageWidthPercentage/100;
  mapContainerElement.style.removeProperty("--unzoomedImageWidth");
  mapContainerElement.style.removeProperty("--unzoomedImageHeight");
  mapContainerElement.style.setProperty("--unzoomedImageWidth",unzoomedImageWidth+"px");
  mapContainerElement.style.setProperty("--unzoomedImageHeight",unzoomedImageHeight+"px");

  //determine mapScale, zoomedImageWidth/zoomedImageHeight
  //--first, using width to compute mapScale
  mapScale=(windowWidth/unzoomedImageWidth);
  zoomedImageWidth=unzoomedImageWidth*mapScale;
  zoomedImageHeight=unzoomedImageHeight*mapScale;
  //--...and if it doesn't fit, then using height to compute mapScale
  if (zoomedImageHeight>windowHeight) {
    mapScale=(windowHeight/unzoomedImageHeight);
    zoomedImageWidth=unzoomedImageWidth*mapScale;
    zoomedImageHeight=unzoomedImageHeight*mapScale;
  }
  mapContainerElement.style.removeProperty("--mapScale");
  mapContainerElement.style.setProperty("--mapScale",mapScale);
  mapContainerElement.style.removeProperty("--zoomedImageWidth");
  mapContainerElement.style.removeProperty("--zoomedImageHeight");
  mapContainerElement.style.setProperty("--zoomedImageWidth",zoomedImageWidth+"px");
  mapContainerElement.style.setProperty("--zoomedImageHeight",zoomedImageHeight+"px");

  //set backgroundPositionX, backgroundPositionY (% of original image that is pushed up and left to create new 'origin', converted to px)
  backgroundPositionX=-(zoomedImageWidth*mapArea.left)/100;
  backgroundPositionY=-(zoomedImageHeight*mapArea.top)/100;
  mapContainerElement.style.removeProperty("--backgroundPositionX");
  mapContainerElement.style.removeProperty("--backgroundPositionY");
  mapContainerElement.style.setProperty("--backgroundPositionX",backgroundPositionX+"px");
  mapContainerElement.style.setProperty("--backgroundPositionY",backgroundPositionY+"px");

  //determine clippedImageWidth, clippedImageHeight
  clippedImageWidth=zoomedImageWidth*imageWidthPercentage/100;
  clippedImageHeight=zoomedImageHeight*imageHeightPercentage/100;
  mapContainerElement.style.removeProperty("--clippedImageWidth");
  mapContainerElement.style.removeProperty("--clippedImageHeight");
  mapContainerElement.style.setProperty("--clippedImageWidth",clippedImageWidth+"px");
  mapContainerElement.style.setProperty("--clippedImageHeight",clippedImageHeight+"px");
  
  //determine imageZoom
  imageZoom=100*(1/(imageWidthPercentage/100));
  mapContainerElement.style.removeProperty("--imageZoom");
  mapContainerElement.style.setProperty("--imageZoom",imageZoom+"%");

  //set overview attribute
  mapContainerElement.setAttribute("overview",mapArea.overview);

  //compute zoomAdjust
  mapContainerElement.style.removeProperty("zoomAdjust");
  mapContainerElement.style.setProperty("--zoomAdjust", (100/imageZoom));

  drawMapAreas();
  drawMapArrows();
  drawMapIcons();

}

function drawMapArrows() {
  mapArrowsElement=document.getElementById("mapArrows");
  if (MapAreas[currentMapArea].overview==1) {
    mapArrowsElement.innerHTML="";
    return;
  }
  var e="";
  if (MapAreas[currentMapArea].mapArrows) {
    for (var i=0; i<MapAreas[currentMapArea].mapArrows.length; i++) {
      e+=mapArrowHTML(MapAreas[currentMapArea].mapArrows[i]);
    }
  }
  mapArrowsElement.innerHTML=e;
}
function mapArrowHTML(ma) {
  var e="<div class='mapArrow' style=\"";
  e+="--left:"+ma.left+"%;";
  e+="--top:"+ma.top+"%;";
  e+="--rotate:"+ma.rotate+"deg;";
  e+="\";";
  e+=" onclick='mapArrowClick("+ma.targetMapArea+");'";
  e+="></div>";
  return e;
}
function mapArrowClick(n) {
  currentMapArea=n;
  mapResize();
}

function drawMapAreas() {
  if (MapAreas[currentMapArea].overview!=1) return;
  mapAreasElement=document.getElementById("mapAreas");
  var e="";
  for (var i=0; i<MapAreas.length; i++) {
    if (MapAreas[i].map==currentMapIndex) {
      if (MapAreas[i].overview!=1) {
        e=e+mapAreaHTML(MapAreas[i]);
      }
    }
  }
  mapAreasElement.innerHTML=e;
}
function mapAreaHTML(mA) {
  var e="<div class='mapArea' style=\"";
  e+="--title:'"+mA.title+"';";
  e+="--left:"+mA.left+"%;";
  e+="--width:"+mA.width+"%;";
  e+="--top:"+mA.top+"%;";
  e+="--height:"+mA.height+"%;";
  e+="\"";
  e+=" onclick='mapAreaClick("+mA.index+");'";
  e+="></div>";
  return e;
}
function mapAreaClick(n) {
  currentMapArea=n;
  mapResize();
}
function zoomOut() {
  for (var i=0; i<MapAreas.length; i++) {
    if (MapAreas[i].map==currentMapIndex) {
      if (MapAreas[i].overview==1) {
        currentMapArea=i;
        mapResize();
        return;
      }
    }
  }
}
