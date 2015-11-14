#pragma once
#include "zframeworkbase.h"
#include <queue>
#include <map>
#include "NodeCmdLine.h"
/*#include <set>*/

#define LOCATION "location"
#define VOCANOL	"vocanol"
#define TAG "tag"


class ztop:public TopCmdLine
{
public:
	ztop();
	~ztop();
public:
	void*								ctx;			//context
	std::vector<zmq_pollitem_t>			vItems;			//poll�?
	//基本消息路由实例
	void*				stateDownSocket;					//sub ，vantilator-sink，状态服务器通道
	void*				jobSocket;						//pull ，任务接收通道	
	void*				backEndSocket;					//push ，返回连�?
	//基本消息通道地址
	std::string			backEndAddr;
	std::string			stateDownAddr;
	std::string			jobAddr;
	//服务插件通道
	std::map<std::string, void*>	pairs;			//pull->push
/*	std::set<void*>		jobSendSoketSets;*/
	//插件服务地址
	std::vector<std::pair<std::string, std::string>>	vServiceToAddr;
private:
	//NodeCmdLine* nodecmd;
public:
	int		signal();						//框架函数
public:
	static bool	InitialTopConf(ztop*ztop, zparallel::zconf* zconf);
	static void InitialJob(std::string, ztop*pztop);
	static bool InitialService(std::vector<std::pair<std::string, std::string>>, ztop*pztop);
	static bool InitialState(std::string stateAddr,ztop*pztop);
	static void InitialFeedBack(std::string feedback,ztop*pztop);
	
	bool	InitialTop();
};
