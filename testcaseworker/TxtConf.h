#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <map>
#include <set>
#include "json.h"
//�������̣���Ӧ�˿ں�ռ��
//������Դ������
//�ָ��������ҵ���work����
//��top��ventilator��work�Ľ����ػ�


#define nocreateprocess return true;

class ServiceConf
{
	struct ProcessInfo
	{
		HANDLE	handle;					//���̾��	
		std::string	type;				//��������
		std::string commandLine;		//������
		std::string	appPath;			//ģ��·��
	};
public:
	ServiceConf();
	~ServiceConf();
	void Monitoring();		//��ѯ��������ѯ�����Ƿ����
	bool ReadConf(std::string path,const char* );
	std::string cmdline;
public:
	static std::string top;
	static std::string ventilator;
	static std::string worker;
private:
	bool CreateProcess(ProcessInfo*pinfo);		//�������̣���Ӧ�˿ں�,process-port-commandline
	bool CreateAllProcess(std::vector<ProcessInfo>v, std::string&strErr);
	bool CreateVentilatorCmd(std::string path, Json::Value&v);
	bool CreateWorkerCmd(std::string path, Json::Value);
	std::map<std::string, std::vector<std::string>> GetTopToVentilatorAddr(std::string path);
	std::map<std::string, std::vector<std::string>> GetVentilatorToWorkerAddr(std::string path);

private:
	int		processCount;		//
	HANDLE*	phandle;			//
	void*	piSocket;			//��ventilatorͨ�ţ�ѡ��push
	void*	romSocket;			//
	std::map<HANDLE, ProcessInfo*>		mapProcessInfo;
	std::vector<ProcessInfo>			vProcessinfo;
	static std::string path;
};
