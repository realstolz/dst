//>>built
define("dijit/layout/LayoutContainer",["dojo/_base/kernel","dojo/_base/lang","dojo/_base/declare","../_WidgetBase","./_LayoutWidget","./utils"],function(_1,_2,_3,_4,_5,_6){
_2.extend(_4,{layoutAlign:"none"});
return _3("dijit.layout.LayoutContainer",_5,{baseClass:"dijitLayoutContainer",constructor:function(){
_1.deprecated("dijit.layout.LayoutContainer is deprecated","use BorderContainer instead",2);
},layout:function(){
_6.layoutChildren(this.domNode,this._contentBox,this.getChildren());
},addChild:function(_7,_8){
this.inherited(arguments);
if(this._started){
_6.layoutChildren(this.domNode,this._contentBox,this.getChildren());
}
},removeChild:function(_9){
this.inherited(arguments);
if(this._started){
_6.layoutChildren(this.domNode,this._contentBox,this.getChildren());
}
}});
});
 