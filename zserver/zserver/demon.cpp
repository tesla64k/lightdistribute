#include "demon.h"
#include <tchar.h>
#include "NodeCmdLine.h"
#include <iostream>
#include <fstream>
//#include <iosfwd>

demon::demon()
{
}


demon::~demon()
{
}

bool demon::CreateAllProcess(std::vector<ProcessInfo>v,std::string& strErr)
{
	bool ret = false;
	for (auto&i:v)
	{
		ret = CreateProcess(&i);
		if (!ret) break;
	}
	if (ret)
	{
		int len = vProcessinfo.size();
		phandle = new HANDLE[len];
		for (int i = 0; i < vProcessinfo.size();i++)
		{
			phandle[i] = vProcessinfo[i].handle;
		}
	}
	return true;
}

bool demon::CreateProcess(ProcessInfo*pinfo)
{
	HANDLE h = nullptr;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	LPTSTR szCmdline = _tcsdup(TEXT("MyChildProcess"));

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	// Start the child process.
	if (( !::CreateProcess(pinfo->appPath.c_str(),   // No module name (use command line)
		(LPTSTR)pinfo->commandLine.c_str(),      // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		))
	{
		printf("CreateProcess failed (%d)./n", GetLastError());
		return false;
	}
	pinfo->handle = pi.hProcess;
	mapProcessInfo[pinfo->handle] = pinfo;
	return true;
}

void demon::Monitoring()
{
	while (true)
	{
		DWORD ret = ::WaitForMultipleObjects(processCount, phandle, false, INFINITE);
		DWORD count = ret - WAIT_OBJECT_0;
		::TerminateProcess(phandle[count], 0);
		::CloseHandle(phandle[count]);
		auto itr = mapProcessInfo.find(phandle[count]);
		ret = CreateProcess(itr->second);
		auto info = itr->second;
		mapProcessInfo.erase(itr);
		mapProcessInfo.insert(std::make_pair(info->handle, info));
		phandle[count] = info->handle;
	}
}

bool demon::ReadConf(std::string path)
{
	bool ret = false;
	char buf[512] = { 0 };
	char nodetype[1024] = { 0 };
	std::vector<ProcessInfo> v;
	std::map<std::string, std::string> nodemap;
	::GetPrivateProfileString("nodetype", "nodetype", "", nodetype, sizeof nodetype, path.c_str());
	std::string errStr;
	if (nodetype == top)
	{
		::GetPrivateProfileString(nodetype, TopCmdLine::backEnd.c_str(), "", buf, sizeof buf, path.c_str());
		if (buf != "")nodemap.insert(std::make_pair(TopCmdLine::backEnd, buf)); else errStr = TopCmdLine::backEnd;
		::GetPrivateProfileString(nodetype, TopCmdLine::stateUp.c_str(), "", buf, sizeof buf, path.c_str());
		nodemap.insert(std::make_pair(TopCmdLine::stateUp, buf));
		if (buf != "")nodemap.insert(std::make_pair(TopCmdLine::stateUp, buf)); else errStr = TopCmdLine::stateUp;
		::GetPrivateProfileString(nodetype, TopCmdLine::job.c_str(), "", buf, sizeof buf, path.c_str());
		if (buf != "")
			nodemap.insert(std::make_pair(TopCmdLine::job, buf)); else errStr = TopCmdLine::job;
		::GetPrivateProfileString(nodetype, TopCmdLine::ventilator.c_str(), "", buf, sizeof buf, path.c_str());
		if (buf != "")
			nodemap.insert(std::make_pair(TopCmdLine::ventilator, buf)); else errStr = TopCmdLine::ventilator;
		::GetPrivateProfileString(nodetype, "apppath", "", buf, sizeof buf, path.c_str());
			
		if (nodemap.size() != 4||buf == "")
		{
			std::cerr << "read ini failed:" << errStr << std::endl;
		}
		else
		{
			/*std::for_each(nodemap.begin(),nodemap.end(),)*/
			TopCmdLine nodecmd;
			nodecmd.CreateCmd(nodemap, std::map<int, std::string>(), std::map<int, int>(), std::map<std::string, std::vector<std::string> >());
			ProcessInfo pinfo;
			pinfo.appPath = buf;
			pinfo.commandLine = nodecmd.v.toStyledString();
			pinfo.type = nodecmd.valueType;
			std::vector<ProcessInfo> v;
			v.push_back(pinfo);
			ret = CreateAllProcess(v,errStr);
		}		
	}
	else if (nodetype == ventilator)
	{
		VentilatorCmdLine nodecmd;
		::GetPrivateProfileString(nodetype, 
			"nodes", "", buf, sizeof buf, path.c_str());
		nodecmd.CreateCmd(nodemap, std::map<int, std::string>(), std::map<int, int>(), std::map<std::string, std::vector<std::string> >());
		auto nodevalue = nodecmd.v["nodes"];
		for (int i = 0; i < nodevalue.size();i++)
		{
			auto func = nodevalue[i][VentilatorCmdLine::function];			
			if (func.isNull() == false)
			{
				ProcessInfo pinfo;
				pinfo.type = nodecmd.valueType;
				auto s = func.asString();
				::GetPrivateProfileString(VentilatorCmdLine::function.c_str(),
					s.c_str(), "", buf, sizeof buf, path.c_str());
				if (buf == ""){ ret = false; break; }
				nodemap.insert(std::make_pair(nodecmd.valueType,nodevalue[i].asString()));
				pinfo.commandLine = nodecmd.v.toStyledString();
				vProcessinfo.push_back(pinfo);
				ret = true;
			}
			else
			{
				ret = false;
				break;
			}
			if (ret)
			{
				ret = CreateAllProcess(vProcessinfo, errStr);
			}
		}
	}
	else if(nodetype == "")/*if (nodetype == worker)*/
	{
		WorkerCmdline nodecmd;
		std::ifstream ifs;
		ifs.open(path);
		Json::Reader reader;
		Json::Value root;
		if (!reader.parse(ifs, root, false))
		{
			auto nodes = root["nodes"];
			for (int i = 0; i < nodes.size();i++)
			{
				std::cerr << nodes[i].toStyledString() << std::endl;
				auto hb = nodes[i][WorkerCmdline::heartBeaten].asString();
				auto task = nodes[i][WorkerCmdline::task].asString();
				auto state = nodes[i][WorkerCmdline::state].asString();
				auto app = nodes[i]["app"].asString();
				auto apppath = nodes[app].asString();
				ProcessInfo pinfo;
				pinfo.appPath = apppath;
				pinfo.type = nodecmd.valueType;
				nodemap.insert(std::make_pair(WorkerCmdline::heartBeaten, hb));
				nodemap.insert(std::make_pair(WorkerCmdline::task, task));
				nodemap.insert(std::make_pair(WorkerCmdline::state, state));
				nodecmd.CreateCmd(nodemap, std::map<int, std::string>(), std::map<int, int>(), std::map<std::string, std::vector<std::string> >());
				pinfo.commandLine = nodecmd.v.toStyledString();
				vProcessinfo.push_back(pinfo);
			}	
			ret = CreateAllProcess(vProcessinfo, errStr);
		}
		ifs.close();
	}
	

	return ret;
}

std::string demon::worker ="worker";

std::string demon::ventilator = "ventilator";

std::string demon::top = "top";
