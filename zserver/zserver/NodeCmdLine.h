#pragma once
#include "json.h"
#include <map>

struct NodeCmdLine
{
	NodeCmdLine();
	~NodeCmdLine();
	Json::Value v;
	static std::string keyType;
	std::string valueType;
	Json::Value CreateCmd(std::map<std::string,std::string>,
		std::map<int,std::string>,
		std::map<int,int>,
		std::map<std::string,std::vector<std::string>>);
	Json::Value Parse(std::string&);
	void AppendType() ;
	virtual bool Validate() ;
private:
// 	void del_sp(char *src);
// 	void del_sp(std::string &str);   // É¾³ýstring×Ö·û´®ÖÐµÄ¿Õ¸ñ
};

struct WorkerCmdline :public NodeCmdLine
{
	//static std::string valueType ;
	static std::string task;
	static std::string heartBeaten;
	static std::string state;
	bool Validate();
	WorkerCmdline();
};

struct VentilatorCmdLine:public NodeCmdLine
{
	//static std::string type /*= "ventilatorcmd"*/;
	static std::string proxy;
	static std::string taskAssign;
	static std::string proxyState;
	static std::string heartBeaten;
	static std::string demon;
	static std::string function;
	bool Validate();
	VentilatorCmdLine();
};

struct TopCmdLine :public NodeCmdLine
{
	//static std::string type ;
	static std::string backEnd;
	static std::string job;
	static std::string stateUp;
	static std::string ventilator;
	static std::string stateDown;
	TopCmdLine();
	bool Validate();
};