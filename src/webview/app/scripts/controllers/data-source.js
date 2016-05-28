"use strict";

//SImple class to wrap all access to remote data, it might be easier to change or remove the server
//this way by just rewriting this class for exemple to directly load the data inside the browser
//itself
function MaltDataSourceNodeJS()
{
	
}

//wrapper to fetch source files from remote server [source-editor.js]
MaltDataSourceNodeJS.prototype.loadSourceFile = function(file,handler)
{
	$.get(mattServerAddress+"/app-sources"+file,handler);
};

//wrapper to getch annotation of source files [source-editor.js]
MaltDataSourceNodeJS.prototype.loadSourceFileAnnotations = function(file,handler)
{
	$.getJSON(mattServerAddress+"/file-infos.json?file="+file,handler);
};

//wrapper to fetch timed data [page-timeline.js]
MaltDataSourceNodeJS.prototype.loadTimedData = function($http,handler)
{
	$http.get(mattServerAddress+'/timed.json').success(handler);
};

//wrapper to fetch timed data [page-alloc-size-distr.js]
MaltDataSourceNodeJS.prototype.getScatter = function($http,handler)
{
	$http.get(mattServerAddress+'/scatter.json').success(handler);
};

//wrapper to fetch stack data [page-stack-peak.js]
MaltDataSourceNodeJS.prototype.loadStackData = function($http,handler)
{
	$http.get(mattServerAddress+'/stacks-mem.json').success(handler);
};

//wrapper to getch data for function list on left side of source browser [page-source.js & page-home]
MaltDataSourceNodeJS.prototype.loadFlatFunctionStats = function($http,handler)
{
	$http.get(mattServerAddress+'/flat.json').success(handler);
};

//wrapper to fetch data for realloc stats [page-realloc.js]
MaltDataSourceNodeJS.prototype.loadReallocStats = function($http,handler)
{
	$http.get(mattServerAddress+'/realloc-map.json').success(handler);
};

//wrapper to fetch global summary for home page [page-home.js]
MaltDataSourceNodeJS.prototype.loadGlobalSummary = function($http,handler)
{
	$http.get(mattServerAddress+'/data/summary.json').success(handler);
};

//wrapper to fetch the global variables infos [page-global-vars.js]
MaltDataSourceNodeJS.prototype.loadGlobalVars = function($http,handler)
{
	$http.get(mattServerAddress+'/global-variables.json').success(handler);
};

MaltDataSourceNodeJS.prototype.getSizeDistr = function($http,handler)
{
	$http.get(mattServerAddress+'/size-map.json').success(handler);
};

MaltDataSourceNodeJS.prototype.getCallStackDataFileLine = function(file,line,handler)
{
	$.getJSON(mattServerAddress+"/stacks.json?file="+encodeURIComponent(file)+"&line="+line,handler);
};

MaltDataSourceNodeJS.prototype.getCallStackDataFunc = function(func,handler)
{
	$.getJSON(mattServerAddress+"/stacks.json?func="+encodeURIComponent(func),handler);
};

MaltDataSourceNodeJS.prototype.loadProcMaps = function(func,handler)
{
	$.getJSON(mattServerAddress+"/procmaps.json",handler);
};

/////////////////////////////////////////////////////////////////////////

//SImple class to wrap all access to remote data, it might be easier to change or remove the server
//this way by just rewriting this class for exemple to directly load the data inside the browser
//itself
function MaltDataSourceClientSide()
{
	var cur = this;
	this.cache = {};
	this.data = {};
// 	var MaltProject = require("MaltProject");
	this.project = new MaltProject();
	
	$.getJSON(mattServerAddress+"/data.json",function(data) {
		cur.data = data;
		cur.project.loadData(data);
	});
}

//SImple class to wrap all access to remote data, it might be easier to change or remove the server
//this way by just rewriting this class for exemple to directly load the data inside the browser
//itself
function MaltDataSourceClientSideData(data)
{
	var cur = this;
	this.cache = {};
	this.data = {};
// 	var MaltProject = require("MaltProject");
	this.project = new MaltProject();
	
	cur.project.loadData(data);
}

//wrapper to fetch source files from remote server [source-editor.js]
MaltDataSourceClientSide.prototype.loadSourceFile = function(file,handler)
{
	$.get(mattServerAddress+"/app-sources"+file,handler);
};

//wrapper to getch annotation of source files [source-editor.js]
MaltDataSourceClientSide.prototype.loadSourceFileAnnotations = function(file,handler)
{
	$.getJSON(mattServerAddress+"/file-infos.json?file="+file,handler);
};

//wrapper to fetch timed data [page-timeline.js]
MaltDataSourceClientSide.prototype.loadTimedData = function($http,handler)
{
// 	$http.get('/timed.json').success(handler);
	handler(this.project.getTimedValues());
};

//wrapper to fetch stack data [page-stack-peak.js]
MaltDataSourceClientSide.prototype.loadStackData = function($http,handler)
{
// 	$http.get('/stacks-mem.json').success(handler);
	handler(this.project.getStacksMem());
};

//wrapper to getch data for function list on left side of source browser [page-source.js & page-home]
MaltDataSourceClientSide.prototype.loadFlatFunctionStats = function($http,handler)
{
	//$http.get('/flat.json').success(handler);
	var res = this.cache[mattServerAddress+'/flat.json'];
	if (res === undefined)
	{
		res = this.project.getFlatFunctionProfile();
		this.cache[mattServerAddress+'/flat.json'] = res;
	}
	handler(res);
};

//wrapper to fetch data for realloc stats [page-realloc.js]
MaltDataSourceClientSide.prototype.loadReallocStats = function($http,handler)
{
// 	$http.get('/realloc-map.json').success(handler);
	handler(this.project.getReallocMap());
};

//wrapper to fetch global summary for home page [page-home.js]
MaltDataSourceClientSide.prototype.loadGlobalSummary = function($http,handler)
{
// 	$http.get('/data/summary.json').success(handler);
	handler(this.project.getSummaryV2());
};

//wrapper to fetch the global variables infos [page-global-vars.js]
MaltDataSourceClientSide.prototype.loadGlobalVars = function($http,handler)
{
// 	$http.get('/global-variables.json').success(handler);
	handler(this.project.getGlobalVariables());
};

MaltDataSourceClientSide.prototype.getSizeDistr = function($http,handler)
{
// 	$http.get('/size-map.json').success($http,handler);
	handler(this.project.getSizeMap());
};

MaltDataSourceClientSide.prototype.getScatter = function($http,handler)
{
// 	$http.get('/scatter.json').success($http,handler);
	handler(this.project.getScatter());
};

MaltDataSourceClientSide.prototype.getCallStackDataFileLine = function(file,line,handler)
{
	//$.getJSON("/stacks.json?file="+encodeURIComponent(file)+"&line="+line,handler);
	handler(this.project.getFilterdStacksOnFileLine(file,line));
};

MaltDataSourceClientSide.prototype.getCallStackDataFunc = function(func,handler)
{
	//$.getJSON("/stacks.json?func="+encodeURIComponent(func),handler);
	handler(this.project.getFilterdStacksOnSymbol(func));
};

/////////////////////////////////////////////////////////////////////////
//can be set optionaly in production mode via script/server.js
//which is absent from the grunt running mode
var mattServerAddress = serverSelector().address;

if (serverSelector().sandalone) {
	var mattDataSource = new MaltDataSourceClientSide();
} else {
	var mattDataSource = new MaltDataSourceNodeJS();
}