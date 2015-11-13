#include "TxtConf.h"
#include <tchar.h>
#include "NodeCmdLine.h"
#include <iostream>
#include <fstream>
//#include <iosfwd>

ServiceConf::ServiceConf()
{
}


ServiceConf::~ServiceConf()
{
}

bool ServiceConf::CreateAllProcess(std::vector<ProcessInfo>v, std::string& strErr)
{
	nocreateprocess
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
		for (size_t i = 0; i < vProcessinfo.size();i++)
		{
			phandle[i] = vProcessinfo[i].handle;
		}
	}
	return true;
}

bool ServiceConf::CreateProcess(ProcessInfo*pinfo)
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

void ServiceConf::Monitoring()
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

bool ServiceConf::ReadConf(std::string path,const char* nodetype)
{
	bool ret = false;
	char buf[512] = { 0 };
	std::map<std::string, std::string> nodemap;
	std::string errStr;
	if (nodetype == top)
	{
		DWORD error = ::GetPrivateProfileString(nodetype, TopCmdLine::backEnd.c_str(), "", buf, sizeof buf, path.c_str());
		if (buf != "" && error != 0)nodemap.insert(std::make_pair(TopCmdLine::backEnd, buf)); else errStr = TopCmdLine::backEnd;
		error = ::GetPrivateProfileString(nodetype, TopCmdLine::stateUp.c_str(), "", buf, sizeof buf, path.c_str());
		nodemap.insert(std::make_pair(TopCmdLine::stateUp, buf));
		if (buf != "")nodemap.insert(std::make_pair(TopCmdLine::stateUp, buf)); else errStr = TopCmdLine::stateUp;
		error = ::GetPrivateProfileString(nodetype, TopCmdLine::proxy.c_str(), "", buf, sizeof buf, path.c_str());
		if (buf != "")
			nodemap.insert(std::make_pair(TopCmdLine::proxy, buf)); else errStr = TopCmdLine::proxy;
		error = ::GetPrivateProfileString(nodetype, TopCmdLine::ventilator.c_str(), "", buf, sizeof buf, path.c_str());
		if (buf != "")
			nodemap.insert(std::make_pair(TopCmdLine::ventilator, buf)); else errStr = TopCmdLine::ventilator;
		error = ::GetPrivateProfileString(nodetype, TopCmdLine::stateDown.c_str(), "", buf, sizeof buf, path.c_str());
		if (buf != "")
			nodemap.insert(std::make_pair(TopCmdLine::stateDown, buf)); else errStr = TopCmdLine::stateDown;
		error = ::GetPrivateProfileString(nodetype, "apppath", "", buf, sizeof buf, path.c_str());
			
		if (errStr != "")
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
			/*std::vector<ProcessInfo> v;*/
			vProcessinfo.push_back(pinfo);
			cmdline = nodecmd.v.toStyledString();
			ret = CreateAllProcess(vProcessinfo,errStr);
		}		
	}
	else if (nodetype == ventilator)
	{
		std::ifstream ifs;
		::GetPrivateProfileString(ServiceConf::path.c_str(), 
			ventilator.c_str(), "", buf, sizeof buf, path.c_str());
		if (buf =="")
		{
			std::cerr << "no ventilator file" << std::endl;
			goto endline;
		}
		ifs.open(buf);
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(ifs, root) == true)
		{
			ret = CreateVentilatorCmd(path, root);
			if (ret == true)
				CreateAllProcess(vProcessinfo, errStr);
		}
		ifs.close();
	}
	else if(nodetype == worker)
	{
		::GetPrivateProfileString(ServiceConf::path.c_str(),
			worker.c_str(), "", buf, sizeof buf, path.c_str());
		if (buf == "")
		{
			std::cerr << "no ventilator file" << std::endl;
			goto endline;
		}
		WorkerCmdline nodecmd;
		std::ifstream ifs;
		ifs.open(buf);
		::GetPrivateProfileString(ServiceConf::path.c_str(),
			ventilator.c_str(), "", buf, sizeof buf, path.c_str());
		if (buf == "")
		{
			std::cerr << "no ventilator file" << std::endl;
			goto endline;
		}
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(ifs, root)==true)
		{
			auto nodes = root["nodes"];
			ret = CreateWorkerCmd(buf,root);
			if (ret == true)
				ret = CreateAllProcess(vProcessinfo, errStr);
		}
		else
		{
			std::cerr << reader.getFormattedErrorMessages() << std::endl;
		}
		ifs.close();
	}
endline:
	return ret;
}

std::map<std::string, std::vector<std::string>> ServiceConf::GetTopToVentilatorAddr(std::string path)
{
	char buf[512] = { 0 };
	std::map<std::string, std::vector<std::string>> ret;
	::GetPrivateProfileString(ServiceConf::top.c_str(), TopCmdLine::stateDown.c_str(), "", buf, sizeof buf, path.c_str());
	std::vector<std::string> v;
	std::string stateip;
	if (buf != "")
	{
		stateip = buf;
	}
	::GetPrivateProfileString(ServiceConf::top.c_str(), TopCmdLine::ventilator.c_str(), "", buf, sizeof buf, path.c_str());
	if (buf != "")
	{
		Json::Reader r;
		Json::Value va;
		r.parse(buf, va);
		for (size_t i = 0; i < va.size();i++)
		{
 			v.push_back(stateip);
			v.push_back(va[i][TopCmdLine::jobSender].asString());
			ret.insert(std::make_pair(va[i][top].asString(),v));
		}
	}
	return ret;
}

std::map<std::string, std::vector<std::string>> ServiceConf::GetVentilatorToWorkerAddr(std::string path)
{
	std::ifstream ifs;
	Json::Reader reader;
	Json::Value root;
	ifs.open(path);
	std::map<std::string, std::vector<std::string>> ret;
	if (reader.parse(ifs, root) == true)
	{
		for (size_t i = 0; i < root["nodes"].size();i++)
		{
			auto node = root["nodes"][i];
			for (size_t j = 0; j < node["group"].size(); j++)
			{
				std::vector<std::string> v;
				auto group = node["group"][j];
				auto hb = group[VentilatorCmdLine::heartBeaten].asString();
				auto task = group[VentilatorCmdLine::taskAssigner].asString();
				auto id = group[ventilator].asString() ;
				v.push_back(hb);
				v.push_back(task);
				ret.insert(std::make_pair(id, v));
			}
		}
	}
	return ret;
}

bool ServiceConf::CreateVentilatorCmd(std::string path, Json::Value&root)
{
	auto nodes = root["nodes"];
	char buf[512] = { 0 };
	auto ret = false;
	std::map<std::string, std::string> nodemap;
	auto maptopaddr = GetTopToVentilatorAddr(path);
	for (size_t i = 0; i < nodes.size(); i++)
	{
		std::cerr << nodes[i].toStyledString() << std::endl;
		std::string app = nodes[i]["app"].asString();
		std::string function = nodes[i][VentilatorCmdLine::function].asString();
		for (size_t j = 0; j < nodes[i]["group"].size();j++)
		{
			VentilatorCmdLine nodecmd;
			auto vNode = nodes[i]["group"][j];
			auto itrip = maptopaddr.find(vNode[top].asString());
			if (itrip == maptopaddr.end())
			{
				goto endline;
			}
			std::string hb = vNode[VentilatorCmdLine::heartBeaten].asString();
			std::string task = vNode[VentilatorCmdLine::taskAssigner].asString();
			std::string job = itrip->second.front() ;
			std::string stateip = itrip->second.back() ;
			ProcessInfo pinfo;
			pinfo.appPath = app;
			pinfo.type = nodecmd.valueType;
			nodemap.insert(std::make_pair(VentilatorCmdLine::heartBeaten, hb));
			nodemap.insert(std::make_pair(VentilatorCmdLine::taskAssigner, task));
			nodemap.insert(std::make_pair(VentilatorCmdLine::jobReceiver, job));
			nodemap.insert(std::make_pair(VentilatorCmdLine::function, function));
			nodemap.insert(std::make_pair(VentilatorCmdLine::stateSender, stateip));
			nodecmd.CreateCmd(nodemap, std::map<int, std::string>(), std::map<int, int>(), std::map<std::string, std::vector<std::string> >());
			cmdline = pinfo.commandLine = nodecmd.v.toStyledString();
			vProcessinfo.push_back(pinfo);
		}
	}
	ret = true;
endline:
	return ret;
}

bool ServiceConf::CreateWorkerCmd(std::string path, Json::Value root)
{
	bool ret = false;
	auto nodes = root["nodes"];
	char buf[512] = { 0 };
	
	std::map<std::string, std::string> nodemap;
	auto venmap = std::map<std::string, std::vector<std::string>>();
	venmap = GetVentilatorToWorkerAddr(path);
	for (size_t i = 0; i < nodes.size(); i++)
	{
		WorkerCmdline nodecmd;
		std::cerr << nodes[i].toStyledString() << std::endl;
		auto itr = venmap.find(nodes[i][ventilator].asString());
		if (itr == venmap.end())
			goto endline;
		auto hb = itr->second[0]; //nodes[i][ventilator].asString();
		auto state = itr->second[0];
		auto task = itr->second[1];
		auto app = nodes[i]["app"].asString();
		auto apppath = root[app].asString();
		ProcessInfo pinfo;
		pinfo.appPath = apppath;
		pinfo.type = nodecmd.valueType;
		nodemap.insert(std::make_pair(WorkerCmdline::heartBeaten, hb));
		nodemap.insert(std::make_pair(WorkerCmdline::task, task));
		nodemap.insert(std::make_pair(WorkerCmdline::state, state));
		nodecmd.CreateCmd(nodemap, std::map<int, std::string>(), std::map<int, int>(), std::map<std::string, std::vector<std::string> >());
		cmdline = pinfo.commandLine = nodecmd.v.toStyledString();
		vProcessinfo.push_back(pinfo);
	}
	ret = true;
endline:
	return ret;
}

std::string ServiceConf::worker = "worker";

std::string ServiceConf::path = "path";

std::string ServiceConf::ventilator = "ventilator";

std::string ServiceConf::top = "top";
