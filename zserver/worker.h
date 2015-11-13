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
	void*	ctx;
protected:
	void*	stateSocket;			//pub

	void*	sinkSocket;				//push,任务汇总通道，out single-direction
	void*	applyTaskSocket;		//pull ,任务下发通道,in,single-direction
	void*	heartBeatSocket;		//req,心跳维护，连接sink,in-out,double-direction
	void*	interProcSlaveSocket;	//
	void*	interProcMasterSocket;	//
	//IP端口

public:
	std::string	taskAddr;			//ventilator地址
	std::string	heartBeatAddr;
	std::string	stateAddr;
	std::string interProcAddr;
	//参数设置
public:
	void							StartWork();
	zparallel::ztaskstatusframe		ApplyWork();
private:
	void StartHeartBeaten(zparallel::ztaskstatusframe& ztask);
	void errout(std::string, int);
public:
	void Finallize();
	static DWORD WINAPI HeartBearten(LPVOID);
};

