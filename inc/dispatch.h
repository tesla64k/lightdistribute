#include "queue"
#include <time.h>
#include <string>
#include "zframeworkbase.h"
namespace zparallel
{
	struct task_t
	{
		int		livenese;			//������
		int		heartBeatInterval;	//�������
		int		taskId;				//����id
		int		reCallTimes;		//��������
		int		jobId;
		int		cost;
		clock_t		heartBeatAt;		//��һ��ʱ��
		std::string	demonTag;		//talk To Demon
		std::string	workerTag;		//map worker
		std::string	taskInfo;
		std::string worker;
/*		task_t(int, int);*/
		static inline task_t Task_t(int jid, int tid)
		{
			task_t t;
			t.jobId = jid;
			t.taskId = tid;
			return t;
		}
	};
	
	struct dispatch
	{
		friend void RegisterDispatch(dispatch*p);
	public:
		virtual std::deque<task_t*> GroupBy(std::string jobStr,std::string strerr,int*ret) = 0;
		virtual std::deque<task_t*> GroupBy(zparallel::ztaskstatusframe* zfs, std::string strerr, int*ret) = 0;
		virtual void Encode(task_t*pTask) = 0;
	private:
		static dispatch* share_pdispatch;
		static void Hook(dispatch*pdispatch);
	public:
		static dispatch* GetInstanse();
	};
	
	void RegisterDispatch(dispatch*p);
}

