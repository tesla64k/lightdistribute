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

	void*	sinkSocket;				//push,�������ͨ����out single-direction
	void*	applyTaskSocket;		//pull ,�����·�ͨ��,in,single-direction
	void*	heartBeatSocket;		//req,����ά��������sink,in-out,double-direction
	void*	interProcSlaveSocket;	//
	void*	interProcMasterSocket;	//
	//IP�˿�

public:
	std::string	taskAddr;			//ventilator��ַ
	std::string	heartBeatAddr;
	std::string	stateAddr;
	std::string interProcAddr;
	CRITICAL_SECTION cs;
	//��������
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

