//>>built
define("dojox/geo/charting/Feature",["dojo/_base/lang","dojo/_base/declare","dojo/_base/array","dojo/_base/html","dojo/dom","dojo/_base/event","dojox/gfx/fx","dojox/color"],function(_1,_2,_3,_4,_5,_6,fx,_7){
return _2("dojox.geo.charting.Feature",null,{_isZoomIn:false,isSelected:false,markerText:null,constructor:function(_8,_9,_a){
this.id=_9;
this.shape=_8.mapObj.createGroup();
this.parent=_8;
this.mapObj=_8.mapObj;
this._bbox=_a.bbox;
this._center=_a.center;
this._defaultFill=_8.defaultColor;
this._highlightFill=_8.highlightColor;
this._defaultStroke={width:this._normalizeStrokeWeight(0.5),color:"white"};
var _b=(_1.isArray(_a.shape[0]))?_a.shape:[_a.shape];
_3.forEach(_b,function(_c){
this.shape.createPolyline(_c).setStroke(this._defaultStroke);
},this);
this.unsetValue();
},unsetValue:function(){
this.value=null;
this.unsetColor();
},unsetColor:function(){
this._defaultFill=this.parent.defaultColor;
var _d=new _7.Color(this.parent.defaultColor).toHsl();
_d.l=1.2*_d.l;
this._highlightFill=_7.fromHsl(_d);
this._setFillWith(this._defaultFill);
},setValue:function(_e){
this.value=_e;
if(_e==null){
this.unsetValue();
}else{
if(this.parent.series.length!=0){
for(var i=0;i<this.parent.series.length;i++){
var _f=this.parent.series[i];
if((_e>=_f.min)&&(_e<_f.max)){
this._setFillWith(_f.color);
this._defaultFill=_f.color;
var col=new _7.Color(_f.color).toHsv();
col.v=(col.v+20);
this._highlightFill=_7.fromHsv(col);
return;
}
}
this.unsetColor();
}
}
},_setFillWith:function(_10){
var _11=(_1.isArray(this.shape.children))?this.shape.children:[this.shape.children];
_3.forEach(_11,_1.hitch(this,function(_12){
if(this.parent.colorAnimationDuration>0){
var _13=fx.animateFill({shape:_12,color:{start:_12.getFill(),end:_10},duration:this.parent.colorAnimationDuration});
_13.play();
}else{
_12.setFill(_10);
}
}));
},_setStrokeWith:function(_14){
var _15=(_1.isArray(this.shape.children))?this.shape.children:[this.shape.children];
_3.forEach(_15,function(_16){
_16.setStroke({color:_14.color,width:_14.width,join:"round"});
});
},_normalizeStrokeWeight:function(_17){
var _18=this.shape._getRealMatrix();
return (dojox.gfx.renderer!="vml")?_17/(this.shape._getRealMatrix()||{xx:1}).xx:_17;
},_onmouseoverHandler:function(evt){
this.parent.onFeatureOver(this);
this._setFillWith(this._highlightFill);
this.mapObj.marker.show(this.id,evt);
},_onmouseoutHandler:function(){
this._setFillWith(this._defaultFill);
this.mapObj.marker.hide();
_4.style("mapZoomCursor","display","none");
},_onmousemoveHandler:function(evt){
if(this.mapObj.marker._needTooltipRefresh){
this.mapObj.marker.show(this.id,evt);
}
if(this.isSelected){
if(this.parent.enableFeatureZoom){
evt=_6.fix(evt||window.event);
_4.style("mapZoomCursor","left",evt.pageX+12+"px");
_4.style("mapZoomCursor","top",evt.pageY+"px");
_4.byId("mapZoomCursor").className=this._isZoomIn?"mapZoomOut":"mapZoomIn";
_4.style("mapZoomCursor","display","block");
}else{
_4.style("mapZoomCursor","display","none");
}
}
},_onclickHandler:function(evt){
this.parent.onFeatureClick(this);
if(!this.isSelected){
this.parent.deselectAll();
this.select(true);
this._onmousemoveHandler(evt);
}else{
if(this.parent.enableFeatureZoom){
if(this._isZoomIn){
this._zoomOut();
}else{
this._zoomIn();
}
}
}
},select:function(_19){
if(_19){
this.shape.moveToFront();
this._setStrokeWith({color:"black",width:this._normalizeStrokeWeight(2)});
this._setFillWith(this._highlightFill);
this.isSelected=true;
this.parent.selectedFeature=this;
}else{
this._setStrokeWith(this._defaultStroke);
this._setFillWith(this._defaultFill);
this.isSelected=false;
this._isZoomIn=false;
}
},_zoomIn:function(){
var _1a=this.mapObj.marker;
_1a.hide();
this.parent.fitToMapArea(this._bbox,15,true,_1.hitch(this,function(){
this._setStrokeWith({color:"black",width:this._normalizeStrokeWeight(2)});
_1a._needTooltipRefresh=true;
this.parent.onZoomEnd(this);
}));
this._isZoomIn=true;
_5.byId("mapZoomCursor").className="";
},_zoomOut:function(){
var _1b=this.mapObj.marker;
_1b.hide();
this.parent.fitToMapContents(3,true,_1.hitch(this,function(){
this._setStrokeWith({color:"black",width:this._normalizeStrokeWeight(2)});
_1b._needTooltipRefresh=true;
this.parent.onZoomEnd(this);
}));
this._isZoomIn=false;
_5.byId("mapZoomCursor").className="";
},init:function(){
this.shape.id=this.id;
this.tooltip=null;
}});
});
 