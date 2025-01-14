//>>built
define("dojox/mobile/ToolBarButton",["dojo/_base/declare","dojo/_base/window","dojo/dom-class","dojo/dom-construct","dojo/dom-style","./common","./_ItemBase"],function(_1,_2,_3,_4,_5,_6,_7){
return _1("dojox.mobile.ToolBarButton",_7,{selected:false,btnClass:"",_defaultColor:"mblColorDefault",_selColor:"mblColorDefaultSel",buildRendering:function(){
this.domNode=this.containerNode=this.srcNodeRef||_2.doc.createElement("div");
this.inheritParams();
_3.add(this.domNode,"mblToolBarButton mblArrowButtonText");
var _8;
if(this.selected){
_8=this._selColor;
}else{
if(this.domNode.className.indexOf("mblColor")==-1){
_8=this._defaultColor;
}
}
_3.add(this.domNode,_8);
if(!this.label){
this.label=this.domNode.innerHTML;
}
if(this.icon&&this.icon!="none"){
this.iconNode=_4.create("div",{className:"mblToolBarButtonIcon"},this.domNode);
_6.createIcon(this.icon,this.iconPos,null,this.alt,this.iconNode);
if(this.iconPos){
_3.add(this.iconNode.firstChild,"mblToolBarButtonSpriteIcon");
}
}else{
if(_6.createDomButton(this.domNode)){
_3.add(this.domNode,"mblToolBarButtonDomButton");
}else{
_3.add(this.domNode,"mblToolBarButtonText");
}
}
this.connect(this.domNode,"onclick","onClick");
},select:function(){
_3.toggle(this.domNode,this._selColor,!arguments[0]);
this.selected=!arguments[0];
},deselect:function(){
this.select(true);
},onClick:function(e){
this.setTransitionPos(e);
this.defaultClickAction();
},_setBtnClassAttr:function(_9){
var _a=this.domNode;
if(_a.className.match(/(mblDomButton\w+)/)){
_3.remove(_a,RegExp.$1);
}
_3.add(_a,_9);
if(_6.createDomButton(this.domNode)){
_3.add(this.domNode,"mblToolBarButtonDomButton");
}
},_setLabelAttr:function(_b){
this.label=_b;
this.domNode.innerHTML=this._cv?this._cv(_b):_b;
}});
});
 