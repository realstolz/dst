//>>built
// wrapped by build app
define("dojox/drawing/library/icons", ["dijit","dojo","dojox"], function(dijit,dojo,dojox){
dojo.provide("dojox.drawing.library.icons");

dojox.drawing.library.icons = {
	// summary:
	//		A collection of icon Stencils for use with the buttons in the ui/Toolbar.
	// description:
	//		Each icon name coresponds to a Stencil or a Plugin. One can be inserted
	//		into a button by adding the property: 'icon', which points to one of
	//		these Stencil objects.
	line:{
		type:"line",
		borderWidth:1,
		x1:20,
		y1:20,
		x2:80,
		y2:80
	},
	ellipse:{
		type:"ellipse",
		borderWidth:1,
		cx:50,
		cy:50,
		rx:35,
		ry:20
	},
	rect:{
		type:"rect",
		borderWidth:1,
		x:10,
		y:30,
		width:80,
		height:40
	},
	triangle:{
		type:"path",
		borderWidth:1,
		closePath:true,
		points:[{x:50, y:10}, {x:10, y:90}, {x:90, y:90}]
	},
	path:{
		type:"path",
		borderWidth:0,
		closePath:true,
		points:[ { x:20, y:80 }, { x:26, y:20 }, { x:80, y:32 }, { x:32, y:50 }, { x:56, y:62 } ]
	},
	arrow:{
		type:"path",
		borderWidth:1,
		closePath:false,
		points:[ { x:20, y:20 }, { x:80, y:80 }, { t:'M', x:74, y:56 }, { x:80, y:80 }, { x:56, y:74 } ]
	},
	textBlock:{
		type:"path",
		borderWidth:0,
		closePath:true,
		points:[ { x:20, y:20 }, { x:80, y:20 }, { x:80, y:38 }, { x:80, y:38 }, { x:74, y:32 }, { x:68, y:26 }, { x:56, y:20 }, { x:56, y:68 }, { x:56, y:74 }, { x:62, y:80 }, { x:68, y:80 }, { x:68, y:80 }, { x:32, y:80 }, { x:32, y:80 }, { x:38, y:80 }, { x:44, y:74 }, { x:44, y:68 }, { x:44, y:20 }, { x:32, y:26 }, { x:26, y:32 }, { x:20, y:38 }, { x:20, y:38 } ]
	},
	
	equation:{
		type:"path",
		borderWidth:2,
		closePath:false,
		points:[ {x:20, y:60 }, { x:80, y:60 }, {t:'M', x:20, y:40 }, { x:80, y:40 } ]
	},
	
	axes:{
		type:"path",
		borderWidth:1,
		closePath:false,
		points:[ { x:20, y:32 }, { x:26, y:20 }, { x:32, y:32 }, { t:'M', x:26, y:20 }, { x:26, y:74 }, { x:80, y:74 }, { t:'M', x:68, y:68 }, { x:80, y:74 }, { x:68, y:80 } ]
	},
	vector:{
		type:"path",
		borderWidth:1,
		closePath:false,
		points:[ { x:20, y:80 }, { x:80, y:20 }, { t:'M', x:62, y:26 }, { x:80, y:20 }, { x:73, y:40 }, { t:'M', x:56, y:56 }, { x:62, y:68 }, { x:62, y:74 } ]
	},
	pan:{
		type:"path",
		borderWidth:1,
		closePath:true,
		points:[ { x:38, y:80 }, { x:26, y:68 }, { x:20, y:50 }, { x:26, y:44 }, { x:26, y:44 }, { x:38, y:56 }, { x:32, y:32 }, { x:32, y:26 }, { x:38, y:26 }, { x:44, y:44 }, { x:44, y:20 }, { x:50, y:20 }, { x:56, y:20 }, { x:56, y:44 }, { x:56, y:26 }, { x:62, y:26 }, { x:68, y:26 }, { x:68, y:50 }, { x:68, y:32 }, { x:74, y:32 }, { x:80, y:38 }, { x:80, y:50 }, { x:74, y:68 }, { x:68, y:80 } ]
	},
	
	plus:{
		type:"path",
		borderWidth:3,
		closePath:false,
		points:[ { x:50, y:20 }, { x:50, y:80 }, { t:"M", x:20, y:50 }, { x:80, y:50 } ]
	},
	zoomIn:{
		type:"path",
		borderWidth:3,
		closePath:false,
		points:[ { x:50, y:20 }, { x:50, y:80 }, { t:"M", x:20, y:50 }, { x:80, y:50 } ]
	},
	zoomOut:{
		type:"path",
		borderWidth:3,
		closePath:false,
		points:[ {x:20, y:50 }, { x:80, y:50 } ]
	},
	zoom100:{
		type:"text",
		text:"100%"
	},
	iconize:{
		type:"path",
		borderWidth:0,
		closePath:true,
		points:
		
[
{	x:20,		y:38	},
{	x:32,		y:26	},
{	x:44,		y:26	},
{	x:56,		y:38	},
{	x:56,		y:50	},
{	x:44,		y:62	},
{	x:32,		y:62	},
{	x:20,		y:56	},
{	t:'Z'	},
{	t:'M', x:38,		y:44	},
{	x:68,		y:44	},
{	x:68,		y:80	},
{	x:38,		y:80	},
{	t:'Z'	},
{	t:'M', x:56,		y:62	},
{	x:68,		y:20	},
{	x:80,		y:62	}
]
		
	},
	pencil:{
		type:"path",
		borderWidth:0,
		closePath:true,
		points:
		
[
{	x:20,		y:80	},
{	x:26,		y:68	},
{	x:68,		y:20	},
{	x:80,		y:20	},
{	x:80,		y:32	},
{	x:38,		y:74	},
{	t:'Z'	},
{	t:'M', x:62,		y:32	},
{	x:68,		y:26	},
{	x:74,		y:26	},
{	x:74,		y:32	},
{	x:68,		y:38	},
{	x:68,		y:32	},
{	t:'Z'	},
{	t:'M', x:56,		y:38	},
{	x:62,		y:38	},
{	x:32,		y:68	},
{	x:32,		y:68	}
]
	}
};
});
 