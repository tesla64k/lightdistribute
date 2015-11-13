#include "worker.h"
#include <memory>
using namespace std;

worker::worker()
{
	//nodecmd = new WorkerCmdline;
	interProcAddr = "inproc://#1";
}


worker::~worker()
{
	//delete nodecmd;
}


void worker::StartWork()
{
	applyTaskSocket = zmq_socket(ctx, ZMQ_REQ);
	int ret = zmq_connect(applyTaskSocket, taskAddr.c_str());
	errout( task +":"+taskAddr , ret);
	stateSocket = zmq_socket(ctx, ZMQ_PUSH);
	ret = zmq_connect(stateSocket, stateAddr.c_str());
	errout(state + ":" + stateAddr, ret);
	heartBeatSocket = zmq_socket(ctx, ZMQ_PUB);
	ret = zmq_connect(heartBeatSocket, heartBeatAddr.c_str());
	errout(heartBeaten + ":" + heartBeatAddr, ret);
	interProcMasterSocket = zmq_socket(ctx, ZMQ_PAIR);
	ret = zmq_bind(interProcMasterSocket, interProcAddr.c_str());
	errout("inproc:" + interProcAddr, ret);
	interProcSlaveSocket = zmq_socket(ctx, ZMQ_PAIR);
	ret = zmq_connect(interProcMasterSocket, interProcAddr.c_str());
	errout("inproc:" + interProcAddr, ret);
	int time = 2000;
	zmq_setsockopt(interProcSlaveSocket, ZMQ_RCVTIMEO, &time, sizeof (int));
}

zparallel::ztaskstatusframe worker::ApplyWork()
{
	std::string s;
	zmq_msg_t	msg;
	zmq_msg_send(&msg, applyTaskSocket,0);		//crash  需要看看是不是会崩溃
	zmq_msg_init(&msg);
	zparallel::ztaskstatusframe ztask;
	int rc = zmq_msg_recv(&msg, applyTaskSocket, 0);
	bool ret;
	if (rc>0)
	{
		s = (char*)zmq_msg_data(&msg);
		Json::Reader r;
		Json::Value v;
		r.parse(s, v);
		ret = ztask.ReadTask(v);
	}
	if (ret == false)
		return zparallel::ztaskstatusframe();
	StartHeartBeaten(ztask);
	return ztask;
}


void worker::StartHeartBeaten(zparallel::ztaskstatusframe& ztask)
{

	void* pvoid = nullptr;
	int jobid = ztask.valueJobId.asInt();
	int taskid = ztask.valueTaskId.asInt();
	std::tuple<int, int, worker*>*tup = new std::tuple<int, int, worker*>;
	QueueUserWorkItem(HeartBearten, tup, WT_EXECUTEDEFAULT);
}

void worker::errout(std::string res, int code)
{
	if (code == 0)
	{
		std::cerr << res << " start ok!!" << std::endl;
	}
	else
	{
		std::cerr << res << " start failed>>>>>>>>>>" << std::endl;
	}

}

void worker::Finallize()
{
	char buf[10] = { 0 };
	int rc = zmq_send(interProcMasterSocket, buf, 0, 0);
}

DWORD WINAPI worker::HeartBearten(LPVOID p)
{
	std::tuple<int, int, worker*>*tup = (std::tuple<int, int, worker*>*)p;
	worker* pworker = std::get<2>(*tup);
	char buf[10];
	auto msg = zparallel::CreateStateMsg(std::get<0>(*tup), std::get<1>(*tup), zparallel::HEARTBEAT, zparallel::CASE_TASKSTART);
	int rc = zmq_msg_send(&msg, pworker->heartBeatSocket, ZMQ_DONTWAIT);
	while (true)
	{
		int rc = zmq_recv(pworker->interProcSlaveSocket, buf, 0, 0);
		if (rc >= 0)
		{
			break;
		}
		msg = zparallel::CreateStateMsg(std::get<0>(*tup), std::get<1>(*tup), zparallel::HEARTBEAT, zparallel::CASE_TASKING);
		zmq_msg_send(&msg, pworker->heartBeatSocket, ZMQ_DONTWAIT);
	}
	delete tup;
	return 0;
}
