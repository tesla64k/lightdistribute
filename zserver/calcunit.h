#pragma once
#include "zmq.h"
#include <string>
#include <map>
#include <deque>
#include <vector>
#include <time.h>
#include <set>
#include "dispatch.h"

typedef __int64 i64;
typedef unsigned int u32;
namespace zparallel
{
	struct calcresource_t
	{
		int coreNum;
		int idle;
		int memResource;
		inline void SetCoreResource(int r){
			idle = coreNum = r;
		}
		inline int BusyPlus(int count)
		{
			return idle -= count;
		}
		inline int IdlePlus(int count)
		{
			return idle += count;
		}
	};

	struct job_t
	{
		zmq_msg_t				msg;
		int						jobId;
		int						unFinshed;
		int						uniteCost;
		task_t*					pending;
		std::deque<task_t*>		deqTasks;	//任务队列
		std::set<task_t*>		setTasks;	//失败任务
		zparallel::dispatch*	pdispatch;	//
		bool		GroupBy(std::string , calcresource_t* r = nullptr);
		/*zmq_msg_t	Encode(task_t*);*/
		task_t*	GetTask();
		void	Commit();
		void	RollBack();
		int		OkTask(task_t*);
		job_t();
		~job_t();
	};
};
