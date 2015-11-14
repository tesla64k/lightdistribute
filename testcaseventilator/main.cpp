#include "json.h"
#include "zframeworkbase.h"
#include "txtconf.h"
#include "dispatch.h"

struct exp:public zparallel::dispatch
{
	virtual bool GroupBy(zparallel::ztaskstatusframe* zfs, std::deque<zparallel::task_t*>*, std::string&strerr) ;
	virtual void Encode(zparallel::task_t*pTask) ;
};

bool exp::GroupBy(zparallel::ztaskstatusframe* zfs, std::deque<zparallel::task_t*>*pdeq, std::string&strerr)
{
	std::cout << zfs->valueTag.asString() << std::endl;
	bool ret = false;
	for (size_t i = 0; i < zfs->valueParam.size();i++)
	{
		zparallel::task_t*ptask = new zparallel::task_t;
		ptask->taskInfo = zfs->valueParam[i].asString();
		ptask->jobId = zfs->valueJobId.asInt();
		ptask->taskId = i;
		ptask->heartBeatInterval = 5000;			//
		ptask->reCallTimes = 0;
		pdeq->push_back(ptask);
	}
	ret = true;
	return ret;
}

void main()
{
	Json::Reader r;
	ServiceConf service;
	service.ReadConf("D:\\zserver\\server.ini", ServiceConf::ventilator.c_str());
	auto __ = service.cmdline;
	std::cerr << __ << std::endl;
	zparallel::_servicetype _type = zparallel::GetService(service.cmdline);
	if (_type == zparallel::_servicetype::TOPPROXY)
	{
		zparallel::TopLoop();
	}
	else if (_type == zparallel::_servicetype::VENTILATOR)
	{
		zparallel::VentilatorLoop();
	}
	else if (_type == zparallel::_servicetype::SLAVER)
	{
		zparallel::StartWork();
	}
	system("pause");
}