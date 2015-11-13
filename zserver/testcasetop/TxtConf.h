#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <map>
#include <set>

//�������̣���Ӧ�˿ں�ռ��
//������Դ������
//�ָ��������ҵ���work����
//��top��ventilator��work�Ľ����ػ�




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
	bool ReadConf(std::string path);
private:
	bool CreateProcess(ProcessInfo*pinfo);		//�������̣���Ӧ�˿ں�,process-port-commandline
	bool CreateAllProcess(std::vector<ProcessInfo>v, std::string&strErr);
private:
	int		processCount;		//
	HANDLE*	phandle;			//
	void*	piSocket;			//��ventilatorͨ�ţ�ѡ��push
	void*	romSocket;			//
	std::map<HANDLE, ProcessInfo*>		mapProcessInfo;
	std::vector<ProcessInfo>			vProcessinfo;
	static std::string top;
	static std::string ventilator;
	static std::string worker;
};
