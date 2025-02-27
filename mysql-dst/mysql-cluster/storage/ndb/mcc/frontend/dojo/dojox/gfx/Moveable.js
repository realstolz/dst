//>>built
define("dojox/gfx/Moveable",["dojo/_base/lang","dojo/_base/declare","dojo/_base/array","dojo/_base/event","dojo/_base/connect","dojo/dom-class","dojo/_base/window","./Mover"],function(_1,_2,_3,_4,_5,_6,_7,_8){
return _2("dojox.gfx.Moveable",null,{constructor:function(_9,_a){
this.shape=_9;
this.delay=(_a&&_a.delay>0)?_a.delay:0;
this.mover=(_a&&_a.mover)?_a.mover:_8;
this.events=[this.shape.connect("onmousedown",this,"onMouseDown")];
},destroy:function(){
_3.forEach(this.events,this.shape.disconnect,this.shape);
this.events=this.shape=null;
},onMouseDown:function(e){
if(this.delay){
this.events.push(this.shape.connect("onmousemove",this,"onMouseMove"),this.shape.connect("onmouseup",this,"onMouseUp"));
this._lastX=e.clientX;
this._lastY=e.clientY;
}else{
new this.mover(this.shape,e,this);
}
_4.stop(e);
},onMouseMove:function(e){
if(Math.abs(e.clientX-this._lastX)>this.delay||Math.abs(e.clientY-this._lastY)>this.delay){
this.onMouseUp(e);
new this.mover(this.shape,e,this);
}
_4.stop(e);
},onMouseUp:function(e){
this.shape.disconnect(this.events.pop());
this.shape.disconnect(this.events.pop());
},onMoveStart:function(_b){
_5.publish("/gfx/move/start",[_b]);
_6.add(_7.body(),"dojoMove");
},onMoveStop:function(_c){
_5.publish("/gfx/move/stop",[_c]);
_6.remove(_7.body(),"dojoMove");
},onFirstMove:function(_d){
},onMove:function(_e,_f){
this.onMoving(_e,_f);
this.shape.applyLeftTransform(_f);
this.onMoved(_e,_f);
},onMoving:function(_10,_11){
},onMoved:function(_12,_13){
}});
});
 