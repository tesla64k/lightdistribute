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
	//��������
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

