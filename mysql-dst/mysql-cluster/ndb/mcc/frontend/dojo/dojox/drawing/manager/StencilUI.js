//>>built
define(["dijit","dojo","dojox"],function(_1,_2,_3){
_2.provide("dojox.drawing.manager.StencilUI");
(function(){
var _4,_5;
_3.drawing.manager.StencilUI=_3.drawing.util.oo.declare(function(_6){
_4=_6.surface;
this.canvas=_6.canvas;
this.defaults=_3.drawing.defaults.copy();
this.mouse=_6.mouse;
this.keys=_6.keys;
this._mouseHandle=this.mouse.register(this);
this.stencils={};
},{register:function(_7){
this.stencils[_7.id]=_7;
return _7;
},onUiDown:function(_8){
if(!this._isStencil(_8)){
return;
}
this.stencils[_8.id].onDown(_8);
},onUiUp:function(_9){
if(!this._isStencil(_9)){
return;
}
this.stencils[_9.id].onUp(_9);
},onOver:function(_a){
if(!this._isStencil(_a)){
return;
}
this.stencils[_a.id].onOver(_a);
},onOut:function(_b){
if(!this._isStencil(_b)){
return;
}
this.stencils[_b.id].onOut(_b);
},_isStencil:function(_c){
return !!_c.id&&!!this.stencils[_c.id]&&this.stencils[_c.id].type=="drawing.library.UI.Button";
}});
})();
});
 