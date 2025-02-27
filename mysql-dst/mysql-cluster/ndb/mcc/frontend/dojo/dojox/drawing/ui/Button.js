//>>built
define(["dijit","dojo","dojox"],function(_1,_2,_3){
_2.provide("dojox.drawing.ui.Button");
_3.drawing.ui.Button=_3.drawing.util.oo.declare(function(_4){
_4.subShape=true;
_2.mixin(this,_4);
this.width=_4.data.width||_4.data.rx*2;
this.height=_4.data.height||_4.data.ry*2;
this.y=_4.data.y||_4.data.cy-_4.data.ry;
this.id=this.id||this.util.uid(this.type);
this.util.attr(this.container,"id",this.id);
if(this.callback){
this.hitched=_2.hitch(this.scope||window,this.callback,this);
}
_4.drawingType="ui";
if(_4.data.width&&_4.data.height){
this.shape=new _3.drawing.stencil.Rect(_4);
}else{
this.shape=new _3.drawing.stencil.Ellipse(_4);
}
var _5=function(s,p,v){
_2.forEach(["norm","over","down","selected"],function(nm){
s[nm].fill[p]=v;
});
};
_5(this.style.button,"y2",this.height+this.y);
_5(this.style.button,"y1",this.y);
if(_4.icon&&!_4.icon.text){
var _6=this.drawing.getConstructor(_4.icon.type);
var o=this.makeOptions(_4.icon);
o.data=_2.mixin(o.data,this.style.button.icon.norm);
if(o.data&&o.data.borderWidth===0){
o.data.fill=this.style.button.icon.norm.fill=o.data.color;
}else{
if(_4.icon.type=="line"||(_4.icon.type=="path"&&!_4.icon.closePath)){
this.style.button.icon.selected.color=this.style.button.icon.selected.fill;
}else{
}
}
this.icon=new _6(o);
}else{
if(_4.text||(_4.icon&&_4.icon.text)){
o=this.makeOptions(_4.text||_4.icon.text);
o.data.color=this.style.button.icon.norm.color;
this.style.button.icon.selected.color=this.style.button.icon.selected.fill;
this.icon=new _3.drawing.stencil.Text(o);
this.icon.attr({height:this.icon._lineHeight,y:((this.height-this.icon._lineHeight)/2)+this.y});
}
}
var c=this.drawing.getConstructor(this.toolType);
if(c){
this.drawing.addUI("tooltip",{data:{text:c.setup.tooltip},button:this});
}
this.onOut();
},{callback:null,scope:null,hitched:null,toolType:"",onClick:function(_7){
},makeOptions:function(d,s){
s=s||1;
d=_2.clone(d);
var o={util:this.util,mouse:this.mouse,container:this.container,subShape:true};
if(typeof (d)=="string"){
o.data={x:this.data.x-5,y:this.data.y+2,width:this.data.width,height:this.data.height,text:d,makeFit:true};
}else{
if(d.points){
_2.forEach(d.points,function(pt){
pt.x=pt.x*this.data.width*0.01*s+this.data.x;
pt.y=pt.y*this.data.height*0.01*s+this.data.y;
},this);
o.data={};
for(var n in d){
if(n!="points"){
o.data[n]=d[n];
}
}
o.points=d.points;
}else{
for(n in d){
if(/x|width/.test(n)){
d[n]=d[n]*this.data.width*0.01*s;
}else{
if(/y|height/.test(n)){
d[n]=d[n]*this.data.height*0.01*s;
}
}
if(/x/.test(n)&&!/r/.test(n)){
d[n]+=this.data.x;
}else{
if(/y/.test(n)&&!/r/.test(n)){
d[n]+=this.data.y;
}
}
}
delete d.type;
o.data=d;
}
}
o.drawingType="ui";
return o;
if(d.borderWidth!==undefined){
o.data.borderWidth=d.borderWidth;
}
return o;
},enabled:true,selected:false,type:"drawing.library.UI.Button",select:function(){
this.selected=true;
if(this.icon){
this.icon.attr(this.style.button.icon.selected);
}
this._change(this.style.button.selected);
this.shape.shadow&&this.shape.shadow.hide();
},deselect:function(){
this.selected=false;
if(this.icon){
this.icon.attr(this.style.button.icon.norm);
}
this.shape.shadow&&this.shape.shadow.show();
this._change(this.style.button.norm);
},disable:function(){
if(!this.enabled){
return;
}
this.enabled=false;
this._change(this.style.button.disabled);
this.icon.attr({color:this.style.button.norm.color});
},enable:function(){
if(this.enabled){
return;
}
this.enabled=true;
this._change(this.style.button.norm);
this.icon.attr({color:this.style.button.icon.norm.color});
},_change:function(_8){
this.shape.attr(_8);
this.shape.shadow&&this.shape.shadow.container.moveToBack();
if(this.icon){
this.icon.shape.moveToFront();
}
},onOver:function(){
if(this.selected||!this.enabled){
return;
}
this._change(this.style.button.over);
},onOut:function(){
if(this.selected){
return;
}
this._change(this.style.button.norm);
},onDown:function(){
if(this.selected||!this.enabled){
return;
}
this._change(this.style.button.selected);
},onUp:function(){
if(!this.enabled){
return;
}
this._change(this.style.button.over);
if(this.hitched){
this.hitched();
}
this.onClick(this);
},attr:function(_9){
if(this.icon){
this.icon.attr(_9);
}
}});
_3.drawing.register({name:"dojox.drawing.ui.Button"},"stencil");
});
 