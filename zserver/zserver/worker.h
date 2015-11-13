#pragma once
#include <vector>
#include "zframeworkbase.h"
#include "NodeCmdLine.h"
class worker :public WorkerCmdline
{
	enum WORKERHANDLEENUM
	{
		STARTWORK = 0x31,
	};
public:
	worker();
	~worker();
protected:
	void*	ctx;
	void*	stateSocket;			//pub

	void*	sinkSocket;				//push,任务汇总通道，out single-direction
	void*	applyTaskSocket;		//pull ,任务下发通道,in,single-direction
	void*	heartBeatSocket;		//req,心跳维护，连接sink,in-out,double-direction
	void*	interProcSlaveSocket;	//
	void*	interProcMasterSocket;	//
/*	void*	threadTalkSocket;		//pair,内部维护通道	*/
	//IP端口

public:
	std::string	taskAddr;			//ventilator地址
	std::string	heartBeatAddr;
	std::string	stateAddr;
	std::string interProcAddr;
	//参数设置
/*	zparallel::zparam*			pZparam;*/
public:
	void							StartWork();
	zparallel::ztaskstatusframe		ApplyWork();
	//virtual NodeCmdLine* Reflect(NodeCmdLine* );
private:
	void StartHeartBeaten(zparallel::ztaskstatusframe& ztask);
public:
	void Finallize();
	static DWORD WINAPI HeartBearten(LPVOID);
// private:
// 	NodeCmdLine* nodecmd;
};

