//>>built
require({cache:{"url:dijit/templates/TitlePane.html":"<div>\n\t<div data-dojo-attach-event=\"onclick:_onTitleClick, onkeypress:_onTitleKey\"\n\t\t\tclass=\"dijitTitlePaneTitle\" data-dojo-attach-point=\"titleBarNode\">\n\t\t<div class=\"dijitTitlePaneTitleFocus\" data-dojo-attach-point=\"focusNode\">\n\t\t\t<img src=\"${_blankGif}\" alt=\"\" data-dojo-attach-point=\"arrowNode\" class=\"dijitArrowNode\" role=\"presentation\"\n\t\t\t/><span data-dojo-attach-point=\"arrowNodeInner\" class=\"dijitArrowNodeInner\"></span\n\t\t\t><span data-dojo-attach-point=\"titleNode\" class=\"dijitTitlePaneTextNode\"></span>\n\t\t</div>\n\t</div>\n\t<div class=\"dijitTitlePaneContentOuter\" data-dojo-attach-point=\"hideNode\" role=\"presentation\">\n\t\t<div class=\"dijitReset\" data-dojo-attach-point=\"wipeNode\" role=\"presentation\">\n\t\t\t<div class=\"dijitTitlePaneContentInner\" data-dojo-attach-point=\"containerNode\" role=\"region\" id=\"${id}_pane\">\n\t\t\t\t<!-- nested divs because wipeIn()/wipeOut() doesn't work right on node w/padding etc.  Put padding on inner div. -->\n\t\t\t</div>\n\t\t</div>\n\t</div>\n</div>\n"}});
define("dijit/TitlePane",["dojo/_base/array","dojo/_base/declare","dojo/dom","dojo/dom-attr","dojo/dom-class","dojo/dom-geometry","dojo/_base/event","dojo/fx","dojo/_base/kernel","dojo/keys","./_CssStateMixin","./_TemplatedMixin","./layout/ContentPane","dojo/text!./templates/TitlePane.html","./_base/manager"],function(_1,_2,_3,_4,_5,_6,_7,_8,_9,_a,_b,_c,_d,_e,_f){
return _2("dijit.TitlePane",[_d,_c,_b],{title:"",_setTitleAttr:{node:"titleNode",type:"innerHTML"},open:true,toggleable:true,tabIndex:"0",duration:_f.defaultDuration,baseClass:"dijitTitlePane",templateString:_e,doLayout:false,_setTooltipAttr:{node:"focusNode",type:"attribute",attribute:"title"},buildRendering:function(){
this.inherited(arguments);
_3.setSelectable(this.titleNode,false);
},postCreate:function(){
this.inherited(arguments);
if(this.toggleable){
this._trackMouseState(this.titleBarNode,"dijitTitlePaneTitle");
}
var _10=this.hideNode,_11=this.wipeNode;
this._wipeIn=_8.wipeIn({node:_11,duration:this.duration,beforeBegin:function(){
_10.style.display="";
}});
this._wipeOut=_8.wipeOut({node:_11,duration:this.duration,onEnd:function(){
_10.style.display="none";
}});
},_setOpenAttr:function(_12,_13){
_1.forEach([this._wipeIn,this._wipeOut],function(_14){
if(_14&&_14.status()=="playing"){
_14.stop();
}
});
if(_13){
var _15=this[_12?"_wipeIn":"_wipeOut"];
_15.play();
}else{
this.hideNode.style.display=this.wipeNode.style.display=_12?"":"none";
}
if(this._started){
if(_12){
this._onShow();
}else{
this.onHide();
}
}
this.arrowNodeInner.innerHTML=_12?"-":"+";
this.containerNode.setAttribute("aria-hidden",_12?"false":"true");
this.focusNode.setAttribute("aria-pressed",_12?"true":"false");
this._set("open",_12);
this._setCss();
},_setToggleableAttr:function(_16){
this.focusNode.setAttribute("role",_16?"button":"heading");
if(_16){
this.focusNode.setAttribute("aria-controls",this.id+"_pane");
_4.set(this.focusNode,"tabIndex",this.tabIndex);
}else{
_4.remove(this.focusNode,"tabIndex");
}
this._set("toggleable",_16);
this._setCss();
},_setContentAttr:function(_17){
if(!this.open||!this._wipeOut||this._wipeOut.status()=="playing"){
this.inherited(arguments);
}else{
if(this._wipeIn&&this._wipeIn.status()=="playing"){
this._wipeIn.stop();
}
_6.setMarginBox(this.wipeNode,{h:_6.getMarginBox(this.wipeNode).h});
this.inherited(arguments);
if(this._wipeIn){
this._wipeIn.play();
}else{
this.hideNode.style.display="";
}
}
},toggle:function(){
this._setOpenAttr(!this.open,true);
},_setCss:function(){
var _18=this.titleBarNode||this.focusNode;
var _19=this._titleBarClass;
this._titleBarClass="dijit"+(this.toggleable?"":"Fixed")+(this.open?"Open":"Closed");
_5.replace(_18,this._titleBarClass,_19||"");
this.arrowNodeInner.innerHTML=this.open?"-":"+";
},_onTitleKey:function(e){
if(e.charOrCode==_a.ENTER||e.charOrCode==" "){
if(this.toggleable){
this.toggle();
}
_7.stop(e);
}else{
if(e.charOrCode==_a.DOWN_ARROW&&this.open){
this.containerNode.focus();
e.preventDefault();
}
}
},_onTitleClick:function(){
if(this.toggleable){
this.toggle();
}
},setTitle:function(_1a){
_9.deprecated("dijit.TitlePane.setTitle() is deprecated.  Use set('title', ...) instead.","","2.0");
this.set("title",_1a);
}});
});
 