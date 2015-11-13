#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <map>
#include <set>

//创建进程，对应端口号占用
//计算资源管理器
//恢复，重启挂掉的work进程
//对top，ventilator，work的进程守护




class ServiceConf
{
	struct ProcessInfo
	{
		HANDLE	handle;					//进程句柄	
		std::string	type;				//进程性质
		std::string commandLine;		//命令行
		std::string	appPath;			//模块路径
	};
public:
	ServiceConf();
	~ServiceConf();
	void Monitoring();		//轮询遍历，查询进程是否存在
	bool ReadConf(std::string path);
private:
	bool CreateProcess(ProcessInfo*pinfo);		//创建进程，对应端口号,process-port-commandline
	bool CreateAllProcess(std::vector<ProcessInfo>v, std::string&strErr);
private:
	int		processCount;		//
	HANDLE*	phandle;			//
	void*	piSocket;			//与ventilator通信，选择push
	void*	romSocket;			//
	std::map<HANDLE, ProcessInfo*>		mapProcessInfo;
	std::vector<ProcessInfo>			vProcessinfo;
	static std::string top;
	static std::string ventilator;
	static std::string worker;
};
