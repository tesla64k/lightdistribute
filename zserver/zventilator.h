#pragma once
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <list>
#include "zframeworkbase.h"
#include "calcunit.h"
#include "NodeCmdLine.h"


class zventilator:public VentilatorCmdLine
{
public:
	zventilator();
	~zventilator();
	void*	ctx;							//zeromq������
protected:
	void*	proxyCmdSocket;				//sub ,��������ڵ㣬����ͨ��
	void*	assignTaskSocket;				//router,�����·�ͨ��
	void*	stateToProxySocket;				//pub,��������״̬���澯ͨ��
	void*	heartBeatenFromTaskSocket;		//sub,��������
	void*	connectDemonSocket;				//router,������𣬽��������õ�
public:
	std::string proxyAddr;				//�ϲ�����������ַ��connect;
	std::string taskAssignerAddr;			//�����������ַ��bind
	std::string stateSenderAddr;			//�����������ַ��connect
	std::string	heartBeatenAddr;		//����������ַ��bind
	std::string	demonAddr;				//�����ַ��bind
private:
	std::vector<zmq_pollitem_t>			vPolls;
	std::map<int,zparallel::job_t*>		mapJob;
	std::list<zparallel::job_t*>			listJob;
	std::set<zparallel::task_t*>			setTasking;
	std::set<std::string>				vWorkerTag;
public:
	bool				StartVentilator();
	void				VentilatorLoop();
// 	Json::Value			ParseCmdLine(std::string);
// 	bool				ValidateCmd(Json::Value&v);
private:
	zparallel::task_t*	SendTask(std::set<std::string>::iterator itr, zparallel::job_t* pjob);
	NodeCmdLine*		nodecmd;
};

