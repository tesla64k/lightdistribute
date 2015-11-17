#pragma once
#include "queue"
#include <time.h>
#include <string>
#include "json.h"
namespace zparallel
{
	struct task_t
	{
		int		livenese;			//������
		int		heartBeatInterval;	//�������
		int		taskId;				//����id
		int		reCallTimes;		//��������
		int		jobId;
		//int		cost;
		clock_t		heartBeatAt;		//��һ��ʱ��
		std::string	demonTag;		//talk To Demon
		std::string	workerTag;		//map worker
		std::string	taskInfo;
		std::string worker;
		static inline task_t Task_t(int jid, int tid)
		{
			task_t t;
			t.jobId = jid;
			t.taskId = tid;
			return t;
		}
	};
	struct ztaskstatusframe;
	struct dispatch
	{
		friend void RegisterDispatch(dispatch*p);
	public:
		virtual bool GroupBy(zparallel::ztaskstatusframe* zfs, std::deque<task_t*>*, std::string&strerr);
		virtual Json::Value EnCode(task_t*pTask) ;
		virtual void DeCode(Json::Value* pvalue) ;
	private:
		static dispatch* share_pdispatch;
		static void Hook(dispatch*pdispatch);
	public:
		static dispatch* GetInstanse();
	};
	
	void RegisterDispatch(dispatch*p);
}

