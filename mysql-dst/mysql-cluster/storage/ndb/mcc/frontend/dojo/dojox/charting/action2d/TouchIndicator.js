//>>built
define("dojox/charting/action2d/TouchIndicator",["dojo/_base/lang","dojo/_base/declare","dojo/_base/event","./ChartAction","./_IndicatorElement","dojox/lang/utils"],function(_1,_2,_3,_4,_5,du){
return _2("dojox.charting.action2d.TouchIndicator",_4,{defaultParams:{series:"",dualIndicator:false,vertical:true,autoScroll:true,fixed:true,precision:0},optionalParams:{lineStroke:{},outlineStroke:{},shadowStroke:{},stroke:{},outline:{},shadow:{},fill:{},fillFunc:null,labelFunc:null,font:"",fontColor:"",markerStroke:{},markerOutline:{},markerShadow:{},markerFill:{},markerSymbol:""},constructor:function(_6,_7,_8){
this._listeners=[{eventName:"ontouchstart",methodName:"onTouchStart"},{eventName:"ontouchmove",methodName:"onTouchMove"},{eventName:"ontouchend",methodName:"onTouchEnd"},{eventName:"ontouchcancel",methodName:"onTouchEnd"}];
this.opt=_1.clone(this.defaultParams);
du.updateWithObject(this.opt,_8);
du.updateWithPattern(this.opt,_8,this.optionalParams);
this._uName="touchIndicator"+this.opt.series;
this.connect();
},connect:function(){
this.inherited(arguments);
this.chart.addPlot(this._uName,{type:_5,inter:this});
},disconnect:function(){
var _9=this.chart.getPlot(this._uName);
if(_9.pageCoord){
this.onTouchEnd();
}
this.chart.removePlot(this._uName);
this.inherited(arguments);
},onTouchStart:function(_a){
if(_a.touches.length==1){
this._onTouchSingle(_a,true);
}else{
if(this.opt.dualIndicator&&_a.touches.length==2){
this._onTouchDual(_a);
}
}
},onTouchMove:function(_b){
if(_b.touches.length==1){
this._onTouchSingle(_b);
}else{
if(this.opt.dualIndicator&&_b.touches.length==2){
this._onTouchDual(_b);
}
}
},_onTouchSingle:function(_c,_d){
if(this.chart._delayedRenderHandle&&!_d){
clearTimeout(this.chart._delayedRenderHandle);
this.chart._delayedRenderHandle=null;
this.chart.render();
}
var _e=this.chart.getPlot(this._uName);
_e.pageCoord={x:_c.touches[0].pageX,y:_c.touches[0].pageY};
_e.dirty=true;
if(_d){
this.chart.delayedRender();
}else{
this.chart.render();
}
_3.stop(_c);
},_onTouchDual:function(_f){
var _10=this.chart.getPlot(this._uName);
_10.pageCoord={x:_f.touches[0].pageX,y:_f.touches[0].pageY};
_10.secondCoord={x:_f.touches[1].pageX,y:_f.touches[1].pageY};
_10.dirty=true;
this.chart.render();
_3.stop(_f);
},onTouchEnd:function(_11){
var _12=this.chart.getPlot(this._uName);
_12.stopTrack();
_12.pageCoord=null;
_12.secondCoord=null;
_12.dirty=true;
this.chart.delayedRender();
}});
});
 