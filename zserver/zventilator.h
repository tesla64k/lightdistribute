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
	void*	ctx;							//zeromq上下文
protected:
	void*	proxyCmdSocket;				//sub ,链接中央节点，任务通道
	void*	assignTaskSocket;				//router,任务下发通道
	void*	stateToProxySocket;				//pub,返回任务状态，告警通道
	void*	heartBeatenFromTaskSocket;		//sub,任务心跳
	void*	connectDemonSocket;				//router,任务挂起，结束，重置等
public:
	std::string proxyAddr;				//上层代理服务器地址，connect;
	std::string taskAssignerAddr;			//任务服务器地址，bind
	std::string stateSenderAddr;			//代理服务器地址，connect
	std::string	heartBeatenAddr;		//任务心跳地址，bind
	std::string	demonAddr;				//精灵地址，bind
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

