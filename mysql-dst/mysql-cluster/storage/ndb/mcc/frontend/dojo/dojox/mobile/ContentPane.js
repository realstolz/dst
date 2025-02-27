//>>built
define("dojox/mobile/ContentPane",["dojo/_base/kernel","dojo/_base/array","dojo/_base/declare","dojo/_base/lang","dojo/_base/window","dijit/_Contained","dijit/_WidgetBase","dojo/_base/xhr","./ProgressIndicator"],function(_1,_2,_3,_4,_5,_6,_7,_8,_9){
return _3("dojox.mobile.ContentPane",[_7,_6],{href:"",content:"",parseOnLoad:true,prog:true,buildRendering:function(){
this.inherited(arguments);
this.domNode.className="mblContentPane";
if(!this.containerNode){
this.containerNode=this.domNode;
}
},startup:function(){
if(this._started){
return;
}
if(this.prog){
this._p=_9.getInstance();
}
var _a=this.getParent&&this.getParent();
if(!_a||!_a.resize){
this.resize();
}
this.inherited(arguments);
},resize:function(){
_2.forEach(this.getChildren(),function(_b){
if(_b.resize){
_b.resize();
}
});
},loadHandler:function(_c){
this.set("content",_c);
},errorHandler:function(_d){
if(this._p){
this._p.stop();
}
},onLoad:function(){
},_setHrefAttr:function(_e){
var p=this._p;
if(p){
_5.body().appendChild(p.domNode);
p.start();
}
this.href=_e;
_8.get({url:_e,handleAs:"text",load:_4.hitch(this,"loadHandler"),error:_4.hitch(this,"errorHandler")});
},_setContentAttr:function(_f){
this.destroyDescendants();
if(typeof _f==="object"){
this.domNode.appendChild(_f);
}else{
this.domNode.innerHTML=_f;
}
if(this.parseOnLoad){
_1.parser.parse(this.domNode);
}
if(this._p){
this._p.stop();
}
this.onLoad();
}});
});
 