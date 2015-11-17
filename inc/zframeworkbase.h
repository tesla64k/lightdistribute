#pragma once
//#include "zhelpers.h"
#include "zmq.h"
#include <string>
#include <vector>
#include "json.h"

//
namespace zparallel{

	static const int CASE_TASKSTART = 0xAB00;
	static const int CASE_TASKING = 0xAB01;
	static const int CASE_TASKFAILED = 0xAB02;
	static const int CASE_TASKWARNING = 0xAB02;
	static const int CASE_TASKOVER = 0xABCD;
	
	static const char* KEY_STATEJOBID = "\"job\"";
	static const char* KEY_STATETASKID = "\"task\"";
	static const char* KEY_STATETYPE = "\"type\"";
	static const char* KEY_STATECASE = "\"case\"";
	static const char* KEY_STATEINFO = "\"info\"";
	static const char* KEY_PROCESS = "\"process\"";
	static const int TYPE_Ventilator = 1;
	static const int TYPE_Worker = 2;

	static const bool HEARTBEAT = false;
	static const bool TASKSTATE = true;
	
	enum _servicetype
	{
		NONE,
		TOPPROXY = 0xAB,
		VENTILATOR,
		SLAVER
	};

	struct zconf
	{
		std::string				caseNode;
		std::string				state;
		std::string				backEnd;
		void*					ctx;
		std::vector<std::pair<std::string, std::string>>	vServiceToAddr;
	};

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
// 		inline bool Read(Json::Value&v)
// 		{
// 			valueTag = v["tag"];
// 			valueJobId = v["jobid"];
// 			valueTaskId = v["taskid"];
// 			valueCase = v["case"];
// 			valueState = v["state"];
// 			if (valueTag.isNull() || valueJobId.isNull() || valueTaskId.isNull()
// 				|| valueCase.isNull() || valueState.isNull())
// 				return false;
// 			return true;
// 		}
		inline bool ReadState(Json::Value&v)
		{
			valueJobId = v["jobid"];
			valueTaskId = v["taskid"];
			valueCase = v["case"];
			valueProcess = v["rate"];
			valueState = v["state"];
			if (valueJobId.isNull() || valueTaskId.isNull() || valueProcess.isNull()
				|| valueCase.isNull() || valueState.isNull())
				return false;
			return true;
		}
		inline bool ReadTask(Json::Value&v)
		{
			valueJobId = v["jobid"];
			valueTaskId = v["taskid"];
			valueParam = v["param"];
			if ( valueJobId.isNull() || valueTaskId.isNull()
				|| valueParam.isNull())
				return false;
			return true;
		}
		inline bool ReadJob(Json::Value&v)
		{
			valueJobId = v["jobid"];
			valueJobMeta = v["jobmeta"];
			valueJobParam = v["jobparam"];
			if (valueJobId.isNull() || valueJobMeta.isNull()||valueJobParam.isNull())
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
	
	//worker
	bool StartWork();
	void ApplyTask();
	void EndWorkSignal();
#define WORKERBENGIN while(true){/*StartWork();*/
#define WAITEINGTASK ztaskstatusframe frame = ApplyTask();
#define ENDSIGNAL EndWorkSignal();
#define ENDTASK }
};
