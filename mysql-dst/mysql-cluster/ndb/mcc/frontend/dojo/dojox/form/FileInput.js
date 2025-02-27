//>>built
require({cache:{"url:dojox/form/resources/FileInput.html":"<div class=\"dijitFileInput\">\n\t<input id=\"${id}\" class=\"dijitFileInputReal\" type=\"file\" dojoAttachPoint=\"fileInput\" name=\"${name}\" />\n\t<div class=\"dijitFakeInput\">\n\t\t<input class=\"dijitFileInputVisible\" type=\"text\" dojoAttachPoint=\"focusNode, inputNode\" />\n\t\t<div class=\"dijitInline dijitFileInputText\" dojoAttachPoint=\"titleNode\">${label}</div>\n\t\t<div class=\"dijitInline dijitFileInputButton\" dojoAttachPoint=\"cancelNode\" \n\t\t\tdojoAttachEvent=\"onclick:reset\">${cancelText}</div>\n\t</div>\n</div>\n"}});
define("dojox/form/FileInput",["dojo/_base/declare","dojo/_base/kernel","dojo/_base/fx","dojo/dom-attr","dojo/dom-class","dojo/text!./resources/FileInput.html","dijit/form/_FormWidget","dijit/_Templated"],function(_1,_2,fx,_3,_4,_5,_6,_7){
_2.experimental("dojox.form.FileInput");
_1("dojox.form.FileInput",_6,{label:"Browse ...",cancelText:"Cancel",name:"uploadFile",templateString:_5,startup:function(){
this._listener=this.connect(this.fileInput,"onchange","_matchValue");
this._keyListener=this.connect(this.fileInput,"onkeyup","_matchValue");
},postCreate:function(){
},_matchValue:function(){
this.inputNode.value=this.fileInput.value;
if(this.inputNode.value){
this.cancelNode.style.visibility="visible";
fx.fadeIn({node:this.cancelNode,duration:275}).play();
}
},setLabel:function(_8,_9){
this.titleNode.innerHTML=_8;
},reset:function(e){
this.disconnect(this._listener);
this.disconnect(this._keyListener);
if(this.fileInput){
this.domNode.removeChild(this.fileInput);
}
fx.fadeOut({node:this.cancelNode,duration:275}).play();
this.fileInput=document.createElement("input");
this.fileInput.setAttribute("type","file");
this.fileInput.setAttribute("id",this.id);
this.fileInput.setAttribute("name",this.name);
_4.add(this.fileInput,"dijitFileInputReal");
this.domNode.appendChild(this.fileInput);
this._keyListener=this.connect(this.fileInput,"onkeyup","_matchValue");
this._listener=this.connect(this.fileInput,"onchange","_matchValue");
this.inputNode.value="";
}});
return dojox.form.FileInput;
});
 