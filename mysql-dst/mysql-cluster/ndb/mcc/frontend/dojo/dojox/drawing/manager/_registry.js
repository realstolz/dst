//>>built
define(["dijit","dojo","dojox"],function(_1,_2,_3){
_2.provide("dojox.drawing.manager._registry");
(function(){
var _4={tool:{},stencil:{},drawing:{},plugin:{},button:{}};
_3.drawing.register=function(_5,_6){
if(_6=="drawing"){
_4.drawing[_5.id]=_5;
}else{
if(_6=="tool"){
_4.tool[_5.name]=_5;
}else{
if(_6=="stencil"){
_4.stencil[_5.name]=_5;
}else{
if(_6=="plugin"){
_4.plugin[_5.name]=_5;
}else{
if(_6=="button"){
_4.button[_5.toolType]=_5;
}
}
}
}
}
};
_3.drawing.getRegistered=function(_7,id){
return id?_4[_7][id]:_4[_7];
};
})();
});
 