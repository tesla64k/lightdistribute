#include "calcunit.h"


// zparallel::task_t::task_t(int jid,int tid)
// {
// 	jobId = jid;
// 	taskId = tid;
// }

bool zparallel::job_t::GroupBy(std::string info, calcresource_t*)
{
	pdispatch->GroupBy(info);
	return true;
}

// bool calcunit::job_t::UseCalcResource(calcresource_t*calc, std::vector<task_t*>&v, task_t*ptask)
// {
// 	if (calc->idle > 0)
// 	{
// 		v.push_back(ptask);
// 		calc->BusyPlus(ptask->cost);
// 		return true;
// 	}
// 	return false;
// }

zparallel::task_t* zparallel::job_t::GetTask()
{
	
	if (setTasks.size())
	{
		auto front = setTasks.begin();
		pending = *front;
		setTasks.erase(front);
		return pending;
	}
	return nullptr;
}

zmq_msg_t zparallel::job_t::Encode(task_t* p)
{
 	zmq_msg_t t;
	pdispatch->Encode(p);
 	zmq_msg_init_size(&t, p->taskInfo.size());
	memcpy(zmq_msg_data(&t), p->taskInfo.c_str(), p->taskInfo.size());
	return t;
}

void zparallel::job_t::RollBack()
{
	if (pending != nullptr)
	{
		setTasks.insert(pending);
		pending = nullptr;
	}
}

int zparallel::job_t::OkTask(task_t*t)
{
	unFinshed--;
	return unFinshed;
}

zparallel::job_t::job_t()
{
	if (pdispatch != nullptr)
		pdispatch = dispatch::GetInstanse();
	else
		;
}

zparallel::job_t::~job_t()
{
	for (auto t: deqTasks)
	{
		delete t;
	}
}

void zparallel::job_t::Commit()
{
	pending = nullptr;
}
