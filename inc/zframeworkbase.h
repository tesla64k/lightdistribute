#pragma once
//#include "zhelpers.h"
#include "zmq.h"
#include <string>
#include <vector>
#include "json.h"
struct dispatch;
//
namespace zparallel{
	//key_case value:
	static const int CASE_TASKSTART = 0xAB00;
	static const int CASE_TASKING = 0xAB01;
	static const int CASE_TASKFAILED = 0xAB02;
	static const int CASE_TASKWARNING = 0xAB02;
	static const int CASE_TASKOVER = 0xABCD;
	

	static const char* KEY_TAG = "\"tag\"";
	static const char* KEY_JOBID = "\"job\"";
	static const char* KEY_TASKID = "\"task\"";
	static const char* KEY_TASKCASE = "\"case\"";
	static const char* KEY_STATEINFO = "\"info\"";
	static const char* KEY_PROCESS = "\"rate\"";
	static const char* KEY_PARAM = "\"param\"";
	static const char* KEY_META = "\"meta\"";
	enum _servicetype
	{
		NONE,
		TOPPROXY = 0xAB,
		VENTILATOR,
		SLAVER
	};

// 	struct zconf
// 	{
// 		std::string				caseNode;
// 		std::string				state;
// 		std::string				backEnd;
// 		void*					ctx;
// 		std::vector<std::pair<std::string, std::string>>	vServiceToAddr;
// 	};

	struct ztaskstatusframe
	{
		Json::Value valueTag;
		Json::Value valueJobId;
		Json::Value valueTaskId;
		Json::Value valueJobMeta;
		Json::Value valueCase;
		Json::Value valueState;
		Json::Value valueParam;
		Json::Value valueJobParam;
		Json::Value valueProcess;

		inline bool ReadState(Json::Value&v)
		{
			valueJobId = v[KEY_JOBID];
			valueTaskId = v[KEY_TASKID];
			valueCase = v[KEY_TASKCASE];
			valueProcess = v[KEY_PROCESS];
			valueState = v[KEY_STATEINFO];
			if (valueJobId.isNull() || valueTaskId.isNull() || valueProcess.isNull()
				|| valueCase.isNull() || valueState.isNull())
				return false;
			return true;
		}
		inline bool ReadTask(Json::Value&v)
		{
			valueJobId = v[KEY_JOBID];
			valueTaskId = v[KEY_TASKID];
			valueParam = v[KEY_PARAM];
			if ( valueJobId.isNull() || valueTaskId.isNull()
				|| valueParam.isNull())
				return false;
			return true;
		}
		inline bool ReadJob(Json::Value&v)
		{
			valueTag = v[KEY_TAG];
			valueJobId = v[KEY_JOBID];
			valueJobMeta = v[KEY_META];
			valueJobParam = v[KEY_PARAM];
			if (valueTag.isNull()||valueJobId.isNull() || valueJobMeta.isNull()||valueJobParam.isNull())
				return false;
			return true;
		}
	};
	int zMsgRecvOnce(void*socekt,zmq_msg_t*pzmsg,int model = 0);
	zmq_msg_t CreateStateMsg(int jobId,int taskId,int stateCase,int process,std::string info ="");

	//demon interface
	bool CreateCommand(std::string cmd);
	bool Monitoring();

	//service interface
	_servicetype GetService(std::string cmd);
	
	//proxy interface
	bool TopLoop();
	bool VentilatorLoop();
/*	void RegisterDispatch(dispatch*p);*/

	//worker
	bool StartWork();
	void ApplyTask();
	void EndWorkSignal();
#define WORKERBENGIN while(true){/*StartWork();*/
#define WAITEINGTASK ztaskstatusframe frame = ApplyTask();
#define ENDSIGNAL EndWorkSignal();
#define ENDTASK }
};
