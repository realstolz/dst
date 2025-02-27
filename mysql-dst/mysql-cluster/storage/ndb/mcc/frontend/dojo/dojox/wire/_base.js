//>>built
define(["dijit","dojo","dojox"],function(_1,_2,_3){
_2.provide("dojox.wire._base");
_3.wire._defaultWireClass="dojox.wire.Wire";
_3.wire._wireClasses={"attribute":"dojox.wire.DataWire","path":"dojox.wire.XmlWire","children":"dojox.wire.CompositeWire","columns":"dojox.wire.TableAdapter","nodes":"dojox.wire.TreeAdapter","segments":"dojox.wire.TextAdapter"};
_3.wire.register=function(_4,_5){
if(!_4||!_5){
return;
}
if(_3.wire._wireClasses[_5]){
return;
}
_3.wire._wireClasses[_5]=_4;
};
_3.wire._getClass=function(_6){
_2["require"](_6);
return _2.getObject(_6);
};
_3.wire.create=function(_7){
if(!_7){
_7={};
}
var _8=_7.wireClass;
if(_8){
if(_2.isString(_8)){
_8=_3.wire._getClass(_8);
}
}else{
for(var _9 in _7){
if(!_7[_9]){
continue;
}
_8=_3.wire._wireClasses[_9];
if(_8){
if(_2.isString(_8)){
_8=_3.wire._getClass(_8);
_3.wire._wireClasses[_9]=_8;
}
break;
}
}
}
if(!_8){
if(_2.isString(_3.wire._defaultWireClass)){
_3.wire._defaultWireClass=_3.wire._getClass(_3.wire._defaultWireClass);
}
_8=_3.wire._defaultWireClass;
}
return new _8(_7);
};
_3.wire.isWire=function(_a){
return (_a&&_a._wireClass);
};
_3.wire.transfer=function(_b,_c,_d,_e){
if(!_b||!_c){
return;
}
if(!_3.wire.isWire(_b)){
_b=_3.wire.create(_b);
}
if(!_3.wire.isWire(_c)){
_c=_3.wire.create(_c);
}
var _f=_b.getValue(_d);
_c.setValue(_f,(_e||_d));
};
_3.wire.connect=function(_10,_11,_12){
if(!_10||!_11||!_12){
return;
}
var _13={topic:_10.topic};
if(_10.topic){
_13.handle=_2.subscribe(_10.topic,function(){
_3.wire.transfer(_11,_12,arguments);
});
}else{
if(_10.event){
_13.handle=_2.connect(_10.scope,_10.event,function(){
_3.wire.transfer(_11,_12,arguments);
});
}
}
return _13;
};
_3.wire.disconnect=function(_14){
if(!_14||!_14.handle){
return;
}
if(_14.topic){
_2.unsubscribe(_14.handle);
}else{
_2.disconnect(_14.handle);
}
};
});
 