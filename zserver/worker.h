#pragma once
#include <vector>
#include "zframeworkbase.h"
#include "NodeCmdLine.h"
#include <windows.h>
#include <tuple>
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
	CRITICAL_SECTION cs;
	//参数设置
public:
	inline std::tuple<int, int, std::string> GetTaskExeInfo(){
	 	EnterCriticalSection(&cs);
		auto info = std::tuple<int, int, std::string>(process, executStateLevel, taskinfo);
		LeaveCriticalSection(&cs);
		return info;
	}
	void							StartWork();
	zparallel::ztaskstatusframe		ApplyWork();
	inline void SetTaskExeInfo(int a, int b, std::string c){ 
		EnterCriticalSection(&cs);
		process = a, executStateLevel = b, taskinfo = c; 
		LeaveCriticalSection(&cs);
	}

private:
	void StartHeartBeaten(zparallel::ztaskstatusframe& ztask);
	void errout(std::string, int);
public:
	void Finallize();
	static DWORD WINAPI HeartBearten(LPVOID);
private:
	int process;
	int executStateLevel;
	std::string taskinfo;
};

