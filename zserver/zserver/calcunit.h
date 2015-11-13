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

// 	struct task_t
// 	{
// 		int		livenese;			//存活次数
// 		int		heartBeatInterval;	//心跳间隔
// 		int		taskId;				//任务id
// 		int		reCallTimes;		//重启次数
// 		int		jobId;
// 		int		cost;
// 		clock_t		heartBeatAt;		//下一跳时间
// 		std::string	demonTag;		//talk To Demon
// 		std::string	workerTag;		//map worker
// 		std::string	taskInfo;
// 		std::string worker;
// 		task_t(int,int);
// 	};
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
		zmq_msg_t	Encode(task_t*);
		task_t*	GetTask();
		void	Commit();
		void	RollBack();
		int		OkTask(task_t*);
		job_t();
		~job_t();
	};
};
