/*****************************************************
             PROJECT  : MALT
             VERSION  : 1.1.0-dev
             DATE     : 02/2018
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include <dlfcn.h>
#include <cstdlib>
#include <execinfo.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include "SymbolSolver.hpp"
#include <json/JsonState.h>
#include <common/Debug.hpp>
#include <common/Array.hpp>
#include <common/Options.hpp>

/*******************  NAMESPACE  ********************/
namespace MALT
{

/*******************  FUNCTION  *********************/
SymbolSolver::SymbolSolver(void )
{
	strings.push_back("??");
}

/*******************  FUNCTION  *********************/
SymbolSolver::~SymbolSolver(void )
{
	for (FuncNameDicMap::const_iterator it = nameMap.begin() ; it != nameMap.end() ; ++it)
		free((void*)it->second);
}

/*******************  FUNCTION  *********************/
const char* SymbolSolver::getName(void* callSite)
{
	//errors
	assert(callSite != NULL);

	//search in map
	FuncNameDicMap::const_iterator it = this->nameMap.find(callSite);

	//if not found, build new entry
	if (it == nameMap.end())
	{
		return setupNewEntry(callSite);
	} else {
		return it->second;
	}
}

/*******************  FUNCTION  *********************/
void SymbolSolver::registerAddress(void* callSite)
{
	//check if present, if true, nothing to do
	CallSiteMap::const_iterator it = callSiteMap.find(callSite);
	if (it != callSiteMap.end())
		return;

	//search procmap entry
	LinuxProcMapEntry * procMapEntry = getMapEntry(callSite);

	//insert
	callSiteMap[callSite].mapEntry = procMapEntry;
}

/*******************  FUNCTION  *********************/
const char* SymbolSolver::setupNewEntry(void* callSite)
{
	//errors
	assert(callSite != NULL);

	//use backtrace_symbol to extract name and line
	char ** tmp = backtrace_symbols(&callSite,1);
	//TODO move to assume
	assert(tmp != NULL);
	assert(tmp[0] != NULL);

	//copy the intersting part
	char * res = strdup(tmp[0]);
	nameMap[callSite] = res;
	free(tmp);

	//ok return
	return res;
}

/*******************  FUNCTION  *********************/
std::ostream& operator<<(std::ostream& out, const SymbolSolver& dic)
{
	for (FuncNameDicMap::const_iterator it = dic.nameMap.begin() ; it != dic.nameMap.end() ; ++it)
		out << it->first << " " << it->second << std::endl;
	return out;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const SymbolSolver& value)
{
	json.openStruct();

// 	json.printField("entries",value.nameMap);
	
	json.printField("map",value.procMap);
	json.printField("strings",value.strings);
	json.printField("instr",value.callSiteMap);

	json.closeStruct();
}

/*******************  FUNCTION  *********************/
const char* SymbolSolver::setupNewEntry(void* callSite, const std::string& name)
{
	//search in map
	FuncNameDicMap::const_iterator it = this->nameMap.find(callSite);

	//if not found, build new entry
	if (it == nameMap.end())
	{
		char * tmp = strdup(name.c_str());
		nameMap[callSite] = tmp;
		return tmp;
	} else {
		return it->second;
	}
}

/*******************  FUNCTION  *********************/
bool SymbolSolver::procMapIsLoaded(void) const
{
	return (procMap.empty() == false);
}

/*******************  FUNCTION  *********************/
void SymbolSolver::loadProcMap(void)
{
	//errors
	assert(procMap.empty());
	
	//open proc map
	FILE * fp = fopen("/proc/self/maps","r");
	assumeArg(fp != NULL,"Failed to read segment mapping in %1 : %2.").arg("/proc/self/map").argStrErrno().end();
	
	//loop on entries
	char buffer[4096];
	char ignored[4096];
	char fileName[4096];
	size_t ignored2;
	LinuxProcMapEntry entry;

	//loop on lines
	while (!feof(fp))
	{
		//load buffer
		char * res = fgets(buffer,sizeof(buffer),fp);
		
		//if ok, parse line
		if (res == buffer)
		{
			//parse
			int cnt = sscanf(buffer,"%p-%p %s %p %s %lu %s\n",&(entry.lower),&(entry.upper),ignored,&(entry.offset),ignored,&ignored2,fileName);
			//printf("%s => %p - %p\n",buffer,entry.lower,entry.upper);
			
			//check args
			if (cnt == 7)
				entry.file = fileName;
			else if (cnt == 6)
				entry.file.clear();
			else
				MALT_FATAL_ARG("Invalid readline of proc/map entry : %1.").arg(buffer).end();
			
			//ok push
			procMap.push_back(entry);
		}
	}
	
	//close
	fclose(fp);
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const LinuxProcMapEntry& value)
{
	json.openStruct();
	json.printField("lower",value.lower);
	json.printField("upper",value.upper);
	json.printField("offset",value.offset);
	json.printField("file",value.file);
	json.closeStruct();
}

/*******************  FUNCTION  *********************/
CallSite::CallSite(LinuxProcMapEntry* mapEntry)
{
	this->mapEntry = mapEntry;
	this->line = 0;
	this->function = -1;
	this->file = -1;
}

/*******************  FUNCTION  *********************/
LinuxProcMapEntry* SymbolSolver::getMapEntry(void* callSite)
{
	//search in map by checking intervals
	for (LinuxProcMap::iterator it = procMap.begin() ; it != procMap.end() ; ++it)
		if (callSite >= it->lower && callSite < it->upper)
			return &(*it);

	//check errors
	if (callSite != (void*)0x1)
		MALT_WARNING_ARG("Caution, call site is not found in procMap : %1.").arg(callSite).end();
	return NULL;
}

/*******************  FUNCTION  *********************/
void SymbolSolver::solveNames(void)
{
	//first try with maqao infos
	if (!maqaoSites.empty())
	{
		if (gblOptions != NULL && !gblOptions->outputSilent)
			fprintf(stderr,"MALT : Resolving symbols with maqao infos...\n");
		this->solveMaqaoNames();
	}
	
	if (gblOptions != NULL && !gblOptions->outputSilent)
		fprintf(stderr,"MALT : Resolving symbols with addr2line...\n");
	
	//avoid to LD_PRELOAD otherwise we will create fork bomb
	setenv("LD_PRELOAD","",1);
	
	//loop on assemblies to extract names
	for (LinuxProcMap::iterator it = procMap.begin() ; it != procMap.end() ; ++it)
	{
		if (!(it->file.empty() || it->file[0] == '['))
			solveNames(&(*it));
	}
	
	//final check for not found
	solveMissings();
}

/*******************  FUNCTION  *********************/
void SymbolSolver::solveMaqaoNames(void)
{
	for (CallSiteMap::iterator it = callSiteMap.begin() ; it != callSiteMap.end() ; ++it)
	{
		MaqaoSiteMap::iterator site = maqaoSites.find(it->first);
		if (site != maqaoSites.end())
		{
			it->second.file = getString(site->second.file);
			it->second.function = getString(site->second.function);
			it->second.line = site->second.line;
			assert(it->second.mapEntry == NULL);
		}
	}
}

/*******************  FUNCTION  *********************/
/*
 * Some links :
 * man proc & man addr2line
 * http://stackoverflow.com/a/7557756/257568
 * ​http://libglim.googlecode.com/svn/trunk/exception.hpp
 * ​http://stackoverflow.com/questions/10452847/backtrace-function-inside-shared-libraries 
*/
void SymbolSolver::solveNames(LinuxProcMapEntry * procMapEntry)
{
	//prepare command
	bool hasEntries = false;
	std::stringstream addr2lineCmd;
	addr2lineCmd << "addr2line -C -f -e " << procMapEntry->file;
	std::vector<CallSite*> lst;
	
	//Gentoo now enable -fPIE by default on executable so we need to detect the case
	//and if enable consider the address relative to the mapping as for .so files.
	//We know on x86_64 that binary are by default map at 0x00400000, if not then
	//we consider -fPIE enabled and apply shift.
	bool isSharedLib = false;
	
	//check if shared lib or exe
	if (procMapEntry->file.substr(procMapEntry->file.size()-3) == ".so" || procMapEntry->file.find(".so.") != std::string::npos)
		isSharedLib = true;
	#ifdef __x86_64__
		else if (procMapEntry->lower != (void*)0x00400000)//check if -fPIE on x86_64
			isSharedLib = true;
	#endif
	
	//preate addr2line args
	for (CallSiteMap::iterator it = callSiteMap.begin() ; it != callSiteMap.end() ; ++it)
	{
		if (it->second.mapEntry == procMapEntry)
		{
			hasEntries = true;
			if (isSharedLib)
				addr2lineCmd << ' '  << (void*)((size_t)it->first - (size_t)procMapEntry->lower);
			else
				addr2lineCmd << ' '  << (void*)((size_t)it->first);
			lst.push_back(&it->second);
		}
	}
	
	//debug
	//printf("%s\n",addr2lineCmd.str().c_str());
	
	//hide error if silent
	if (gblOptions != NULL && gblOptions->outputSilent)
		 addr2lineCmd << ' ' << "2>/dev/null";
	
	//if no extry, exit
	if (!hasEntries)
		return;
	
	//run command
	//std::cerr << addr2lineCmd.str() << std::endl;
	FILE * fp = popen(addr2lineCmd.str().c_str(),"r");
	
	//check error, skip resolution
	if (fp == NULL)
	{
		MALT_ERROR_ARG("Fail to use addr2line on %1 to load symbols : %2.").arg(procMapEntry->file).argStrErrno().end();
		return;
	}
	
	//read all entries (need big for some big template based C++ application, 
	//seen at cern)
	static char bufferFunc[200*4096];
	static char bufferFile[20*4096];
	size_t i = 0;
	while (!feof(fp))
	{
		//read the two lines
		char * funcRes = fgets(bufferFunc,sizeof(bufferFunc),fp);
		char * fileRes = fgets(bufferFile,sizeof(bufferFile),fp);
		
		if (funcRes != bufferFunc || fileRes != bufferFile)
			break;

		//std::cerr << bufferFunc;
		//std::cerr << bufferFile;

		//check end of line and remove it
		int endLine = strlen(bufferFunc);
		assumeArg(bufferFunc[endLine-1] == '\n',"Missing \\n at end of line for the function or symbol name read from addr2line : %1.").arg(bufferFunc).end();
		bufferFunc[endLine-1] = '\0';

		//check errors
		assume(i < lst.size(),"Overpass lst size.");

		//search ':' separator at end of "file:line" string
		char * sep = strrchr(bufferFile,':');
		if (sep == NULL)
		{
			MALT_WARNING_ARG("Fail to split source location on ':' : %1").arg(bufferFile).end();
		} else {
			*sep='\0';
			
			//extract line
			lst[i]->line = atoi(sep+1);
			
			//get filename and function name address
			lst[i]->file = getString(bufferFile);

			//if (strcmp(bufferFunc,"??") == 0)
			//	lst[i]->function = -1;
			//else
			lst[i]->function = getString(bufferFunc);
		}

		//move next
		i++;
		//std::cerr<< std::endl;
	}
	
	//close
	int res = pclose(fp);
	if (res != 0)
	{
		MALT_ERROR_ARG("Get error while using addr2line on %1 to load symbols : %2.").arg(procMapEntry->file).argStrErrno().end();
		return;
	}

	//error
	assumeArg(i == lst.size(),"Some entries are missing from addr2line, get %1, but expect %2. (%3)").arg(i).arg(lst.size()).arg(procMapEntry->file).end();
}

/*******************  FUNCTION  *********************/
int SymbolSolver::getString(const char * value)
{
	int id = 0;
	for (std::vector<std::string>::iterator it = strings.begin() ; it != strings.end() ; ++it)
	{
		if (*it == value)
			return id;
		id++;
	}
	if (*value == '\0')
		fprintf(stderr,"insert empty\n");
	strings.push_back(value);
	return strings.size()-1;
}

/*******************  FUNCTION  *********************/
void SymbolSolver::registerMaqaoFunctionSymbol(int funcId, const char* funcName, const char* file, int line)
{
	MaqaoSite & site = maqaoSites[(void*)(size_t)funcId];
	site.function = funcName;
	puts(file);
	site.line = line;
	site.file = file;
}

/*******************  FUNCTION  *********************/
void convertToJson(htopml::JsonState& json, const CallSite& value)
{
	if (value.mapEntry != NULL || value.line != 0 || value.function != -1 || value.file != -1)
	{
		json.openStruct();
		if (value.file != -1)// && *value.file != "??")
			json.printField("file",value.file);
		if (value.function != -1)// && *value.function != "??")
			json.printField("function",value.function);
		if (value.line != 0)
			json.printField("line",value.line);
		if (value.mapEntry != NULL)
			json.printField("binary",value.mapEntry->file);
		json.closeStruct();
	}
}

/*******************  FUNCTION  *********************/
const CallSite* SymbolSolver::getCallSiteInfo(void* site) const
{
	CallSiteMap::const_iterator it = callSiteMap.find(site);
	if (it == callSiteMap.end())
		return NULL;
	else
		return &it->second;
}

/*******************  FUNCTION  *********************/
const std::string& SymbolSolver::getString(int id) const
{
	assert((size_t)id < strings.size());
	return strings[id];
}

/*******************  FUNCTION  *********************/
/**
 * Function to use after loading symbols with resolveNames(). It search all '??' values
 * and try a resolution of symbol names with backtrace_symbols() from glibc.
**/
void SymbolSolver::solveMissings(void)
{
	//store symbols not resolved
	Array<void*> toResolve(16,1024,false);

	//search
	for (CallSiteMap::const_iterator it = callSiteMap.begin() ; it != callSiteMap.end() ; ++it)
		if (it->second.function == -1 || getString(it->second.function) == "??")
			toResolve.push_back(it->first);
		
	//nothing to do
	if (toResolve.size() == 0)
		return;
	
	char ** res = backtrace_symbols(toResolve.getBuffer(),toResolve.size());
	if (res != NULL)
	{
		int i = 0;
		for (CallSiteMap::iterator it = callSiteMap.begin() ; it != callSiteMap.end() ; ++it)
			if (it->second.function == -1 || getString(it->second.function) == "??")
				it->second.function = getString(extractSymbolName(res[i++]));
		free(res);
	}
}

/*******************  FUNCTION  *********************/
char * SymbolSolver::extractSymbolName(char* value)
{
	//Vars
	char * ret = NULL;
	std::string old = value;
	
	//errors
	assert(value != NULL);
	
	//search last
	char * pos1 = strrchr(value,'(');
	char * pos2 = strrchr(value,')');
	
	//scanf
	if (pos1 == NULL)
	{
		ret = value;
	} else if (pos2 == NULL) {
		ret = pos1;
	} else {
		*pos2 = '\0';
		ret = pos1+1;
	}
	
	//remove +0xXX
	int i = 0;
	while (ret[i] != '\0')
	{
		if (ret[i] == '+')
			ret[i] = '\0';
		else
			i++;
	}
	
	if (*ret == 0)
	{
		ret = value;
		memcpy(value,old.c_str(),old.size());
	}
	
	return ret;
}

/*******************  FUNCTION  *********************/
bool SymbolSolver::isSameFuntion(const CallSite* s1, void* s2) const
{
	if (s1 == NULL || s2 == NULL)
		return false;
	
	const CallSite * ss2 = getCallSiteInfo(s2);
	if (ss2 == NULL)
		return false;
	
	if (ss2 == s1)
		return true;
	
	if (s1->function <= 0 || ss2->function <= 0)
		return false;
	
	return (s1->mapEntry == ss2->mapEntry && s1->function == ss2->function);
}

}
