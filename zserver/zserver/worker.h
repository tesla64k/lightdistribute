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

	void*	sinkSocket;				//push,�������ͨ����out single-direction
	void*	applyTaskSocket;		//pull ,�����·�ͨ��,in,single-direction
	void*	heartBeatSocket;		//req,����ά��������sink,in-out,double-direction
	void*	interProcSlaveSocket;	//
	void*	interProcMasterSocket;	//
/*	void*	threadTalkSocket;		//pair,�ڲ�ά��ͨ��	*/
	//IP�˿�

public:
	std::string	taskAddr;			//ventilator��ַ
	std::string	heartBeatAddr;
	std::string	stateAddr;
	std::string interProcAddr;
	//��������
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

