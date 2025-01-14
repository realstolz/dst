//>>built
define(["dijit","dojo","dojox"],function(_1,_2,_3){
_2.provide("dojox.drawing.tools.Path");
_3.drawing.tools.Path=_3.drawing.util.oo.declare(_3.drawing.stencil.Path,function(){
this.pathMode="";
this.currentPathMode="";
this._started=false;
this.oddEvenClicks=0;
},{draws:true,onDown:function(_4){
if(!this._started){
this.onStartPath(_4);
}
},makeSubPath:function(_5){
if(_5){
if(this.currentPathMode=="Q"){
this.points.push({x:this.points[0].x,y:this.points[0].y});
}
this.points.push({t:"Z"});
this.render();
}
this.currentPathMode="";
this.pathMode="M";
},onStartPath:function(_6){
this._started=true;
this.revertRenderHit=this.renderHit;
this.renderHit=false;
this.closePath=false;
this.mouse.setEventMode("PathEdit");
this.closePoint={x:_6.x,y:_6.y};
this._kc1=this.connect(this.keys,"onEsc",this,function(){
this.onCompletePath(false);
});
this._kc2=this.connect(this.keys,"onKeyUp",this,function(_7){
switch(_7.letter){
case "c":
this.onCompletePath(true);
break;
case "l":
this.pathMode="L";
break;
case "m":
this.makeSubPath(false);
break;
case "q":
this.pathMode="Q";
break;
case "s":
this.pathMode="S";
break;
case "z":
this.makeSubPath(true);
break;
}
});
},onCompletePath:function(_8){
this.remove(this.closeGuide,this.guide);
var _9=this.getBounds();
if(_9.w<this.minimumSize&&_9.h<this.minimumSize){
this.remove(this.hit,this.shape,this.closeGuide);
this._started=false;
this.mouse.setEventMode("");
this.setPoints([]);
return;
}
if(_8){
if(this.currentPathMode=="Q"){
this.points.push({x:this.points[0].x,y:this.points[0].y});
}
this.closePath=true;
}
this.renderHit=this.revertRenderHit;
this.renderedOnce=true;
this.onRender(this);
this.disconnect([this._kc1,this._kc2]);
this.mouse.setEventMode("");
this.render();
},onUp:function(_a){
if(!this._started||!_a.withinCanvas){
return;
}
if(this.points.length>2&&this.closeRadius>this.util.distance(_a.x,_a.y,this.closePoint.x,this.closePoint.y)){
this.onCompletePath(true);
}else{
var p={x:_a.x,y:_a.y};
this.oddEvenClicks++;
if(this.currentPathMode!=this.pathMode){
if(this.pathMode=="Q"){
p.t="Q";
this.oddEvenClicks=0;
}else{
if(this.pathMode=="L"){
p.t="L";
}else{
if(this.pathMode=="M"){
p.t="M";
this.closePoint={x:_a.x,y:_a.y};
}
}
}
this.currentPathMode=this.pathMode;
}
this.points.push(p);
if(this.points.length>1){
this.remove(this.guide);
this.render();
}
}
},createGuide:function(_b){
if(!this.points.length){
return;
}
var _c=[].concat(this.points);
var pt={x:_b.x,y:_b.y};
if(this.currentPathMode=="Q"&&this.oddEvenClicks%2){
pt.t="L";
}
this.points.push(pt);
this.render();
this.points=_c;
var _d=this.util.distance(_b.x,_b.y,this.closePoint.x,this.closePoint.y);
if(this.points.length>1){
if(_d<this.closeRadius&&!this.closeGuide){
var c={cx:this.closePoint.x,cy:this.closePoint.y,rx:this.closeRadius,ry:this.closeRadius};
this.closeGuide=this.container.createEllipse(c).setFill(this.closeColor);
}else{
if(_d>this.closeRadius&&this.closeGuide){
this.remove(this.closeGuide);
this.closeGuide=null;
}
}
}
},onMove:function(_e){
if(!this._started){
return;
}
this.createGuide(_e);
},onDrag:function(_f){
if(!this._started){
return;
}
this.createGuide(_f);
}});
_3.drawing.tools.Path.setup={name:"dojox.drawing.tools.Path",tooltip:"Path Tool",iconClass:"iconLine"};
_3.drawing.register(_3.drawing.tools.Path.setup,"tool");
});
 