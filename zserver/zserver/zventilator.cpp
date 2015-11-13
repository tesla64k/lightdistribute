#include "zventilator.h"
#include "zhelpers.h"
#include "json.h"

zventilator::zventilator()
{
	nodecmd = new VentilatorCmdLine;
}


zventilator::~zventilator()
{
	delete nodecmd;
}

bool zventilator::StartVentilator()
{
	assignTaskSocket = zmq_socket(ctx, ZMQ_ROUTER);
	int ret = zmq_bind(assignTaskSocket, taskAssignAddr.c_str());

	proxyCmdSocket = zmq_socket(ctx, ZMQ_PULL);
	ret = zmq_connect(proxyCmdSocket, proxyAddr.c_str());
	
	stateToProxySocket = zmq_socket(ctx, ZMQ_PUSH);
	ret = zmq_connect(stateToProxySocket, proxyStateAddr.c_str());
	
	heartBeatenFromTaskSocket = zmq_socket(ctx, ZMQ_SUB);
	ret = zmq_bind(heartBeatenFromTaskSocket, heartBeatenAddr.c_str());
	
	connectDemonSocket = zmq_socket(ctx, ZMQ_ROUTER);
	ret = zmq_connect(connectDemonSocket, demonAddr.c_str());

	vPolls.push_back({ proxyCmdSocket, 0, ZMQ_POLLIN, 0 });
	vPolls.push_back({ assignTaskSocket, 0, ZMQ_POLLIN, 0 });
	vPolls.push_back({ heartBeatenFromTaskSocket, 0, ZMQ_POLLIN, 0 });

	return true;
}

void zventilator::WorkLoop()
{
	auto $ProxyCmd = [&](void* socket) ->int {
		Json::Reader r;
		Json::Value v;
		zmq_msg_t msg;
		zmq_msg_init(&msg);
		int rc = zmq_msg_recv(&msg, socket, 0);
		if (rc <= 0){ zmq_msg_close(&msg); return -1; };
		void* data = zmq_msg_data(&msg);
		std::string tasks = (char*)data;
		zmq_msg_close(&msg);
		r.parse(tasks, v);
		zparallel::job_t* job = new zparallel::job_t();
		if (job-> GroupBy(tasks) == false)
		{
			std::cerr << "json err!" << std::endl;
			return -1;
		}
		mapJob.insert(std::make_pair(job->jobId,job));
		listJob.push_back(job);
		zparallel::task_t* ptask;
		auto tag = vWorkerTag.begin();
		while (job->setTasks.size())
		{
			if (tag == vWorkerTag.end())
				break;
			ptask = SendTask(tag, job);
			if (ptask != nullptr)
			{
				auto stateMsg = zparallel::CreateStateMsg(job->jobId,ptask->taskId,zparallel::TASKSTATE,zparallel::TASKSTATE);
				rc = zmq_msg_send(&stateMsg, stateToProxySocket, ZMQ_DONTWAIT);
			}
			tag = vWorkerTag.begin();
		}
		return rc;
	};
	auto $WorkerSingal = [&](void*socket) ->int
	{
		zmq_msg_t msg;
		zmq_msg_init(&msg);
		int rc = zmq_msg_recv(&msg, socket, 0);
		if (rc <= 0)
			return rc;
		char* data = (char*)zmq_msg_data(&msg);
		vWorkerTag.insert(data);
		rc = zmq_msg_recv(&msg,socket,0);
		rc = zmq_msg_recv(&msg, socket, 0);
		zmq_msg_close(&msg);
		return rc;
	};
	auto $TaskHeartBeaten = [&](void*socket)->int
	{
		zmq_msg_t msg;
		Json::Reader r;
		Json::Value v;
		zmq_msg_init(&msg);
		int rc = zmq_msg_recv(&msg, socket, 0);		
		if (rc<=0)
			return rc;
		std::string hb = (char*)zmq_msg_data(&msg);
		r.parse(hb, v);
		zparallel::ztaskstatusframe taskstatusframe;
		if (taskstatusframe.Read(v) == false)
		{
			return -1;
		}
		auto iter = mapJob.find(taskstatusframe.valueJobId.asInt());
		if (iter == mapJob.end())
			return -1;
		if (taskstatusframe.valueTaskId.asInt()<0 ||
			taskstatusframe.valueTaskId.asInt()>iter->second->deqTasks.size())
			return -1;
		auto task = iter->second->deqTasks[taskstatusframe.valueTaskId.asInt()];
		task->livenese = 3;
		task->heartBeatAt = clock() + task->heartBeatInterval;
		if (taskstatusframe.valueCase.asInt() == zparallel::CASE_TASKOVER)		//task over
		{
			iter->second->OkTask(task);
			auto stateMsg = zparallel::CreateStateMsg(iter->second->jobId,task->taskId,zparallel::TASKSTATE,
				zparallel::CASE_TASKOVER );
			rc = zmq_msg_send(&stateMsg, stateToProxySocket, ZMQ_DONTWAIT);
			setTasking.erase(task);
			mapJob.erase(iter);
		}
		if (taskstatusframe.valueTag.asInt() == 0)		//heartbeaten.....
		{
			;
		}
		else if (taskstatusframe.valueTag.asInt() == 1)	//task status.....
		{
			rc = zmq_msg_send(&msg, stateToProxySocket, ZMQ_DONTWAIT);		//msg throw to proxy
		}
		return rc;
	};

	while (true)
	{
		int rc = zmq_poll(&vPolls[0], vPolls.size(), 1000);
		rc = zmq_errno();
		for (auto i = 0; i < vPolls.size(); i++)
		{
			zmq_pollitem_t& t = vPolls[i];
			if ((t.revents &ZMQ_POLLIN) && i == 0)						//按照初始化顺序，case通道第一
			{
				if (t.socket == proxyCmdSocket)
				{
					rc = $ProxyCmd(t.socket);
				}
				else if (t.socket == assignTaskSocket)
				{
					rc = $WorkerSingal(t.socket);
					if (rc<=0)
					{
						std::cerr << "worker signal err..." << std::endl;
					}
				}
				else if (t.socket == heartBeatenFromTaskSocket)
				{
					rc = $TaskHeartBeaten(t.socket);
				}
				else
				{
					;
				}
			}
		}

		auto tag = vWorkerTag.begin();
		for (auto job = listJob.begin(); job != listJob.end();job++)		//
		{
			if (tag == vWorkerTag.end())
				break;
			if ((*job)->unFinshed == 0||(*job)->setTasks.size() == 0 )
			{
				listJob.erase(job);
			}
			else
			{
				auto pjob = *job;
				while ( pjob->setTasks.size())
				{
					auto ptask = SendTask(tag, pjob);
					if (ptask != nullptr)
					{
						auto stateMsg = zparallel::CreateStateMsg(pjob->jobId, ptask->taskId, zparallel::TASKSTATE,zparallel::CASE_TASKSTART);
						rc = zmq_msg_send(&stateMsg, stateToProxySocket, ZMQ_DONTWAIT);
					}
					tag = vWorkerTag.begin();
					if (tag == vWorkerTag.end())
						break;
				}

			}
		}
		

		for (auto i : setTasking)				//处理心跳
		{
			clock_t t = clock();
			if (i->heartBeatAt<t)
			{
				i->livenese--;
				if (i->livenese <= 0)
				{
					i->reCallTimes++;
					if (i->reCallTimes >1)
					{

					}
					else
					{
						i->livenese = 3;
						i->heartBeatAt = clock() + i->heartBeatInterval;
					}
				}
				else
				{
					i->heartBeatAt = clock() + i->heartBeatInterval;
				}
			}
		}
	}
}
//tasking queue
zparallel::task_t* zventilator::SendTask(std::set<std::string>::iterator itr, zparallel::job_t* pjob)
{
	auto $RouterSend = [](zmq_msg_t& t, const std::string& tag, void*socket)->int
	{
		int rc = zmq_send(socket, tag.c_str(), tag.size(), ZMQ_DONTWAIT);
		if (rc < 0) return rc;
		rc = zmq_send(socket, "", 0, ZMQ_DONTWAIT);
		if (rc < 0) return rc;
		rc = zmq_msg_send(&t, socket, ZMQ_DONTWAIT);
		return rc;
	};

	zmq_msg_t msg_send;
	auto ptask = pjob->GetTask();
	if (ptask != nullptr)
	{
		msg_send = pjob->Encode(ptask);
	}
	else
	{
		goto ptrNULL;
	}
	int rc = $RouterSend(msg_send, *itr, assignTaskSocket);
	if (rc > 0)	// send a task
	{
		vWorkerTag.erase(itr);
		pjob->Commit();
		setTasking.insert(ptask);
	}
	else
	{
		pjob->RollBack();
		goto ptrNULL;
	}
	return ptask;
ptrNULL:
	return nullptr;
}

// Json::Value zventilator::ParseCmdLine(std::string)
// {
// 	Json::Value v;
// }
// 
// bool zventilator::ValidateCmd(Json::Value&v)
// {
// 	return true;
// }
