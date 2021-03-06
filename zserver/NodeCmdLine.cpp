#include "NodeCmdLine.h"
#include <algorithm>
//#include "worker.h"


NodeCmdLine::NodeCmdLine()
{
}


NodeCmdLine::~NodeCmdLine()
{
}

Json::Value NodeCmdLine::CreateCmd(std::map<std::string, std::string>a1, 
	std::map<int, std::string>a2, 
	std::map<int, int>a3, std::map<std::string, std::vector<std::string> >a4)
{
	AppendType();
	std::for_each(a1.begin(), a1.end(), [&](std::pair<std::string,std::string>i){
		if (i.first == TopCmdLine::ventilator)
		{
			Json::Reader r;
			Json::Value vlocal;
			r.parse(i.second, vlocal);
			v[i.first] = vlocal;
		}
		else
		{
			v[i.first] = Json::Value(i.second);
		}
	});
	std::for_each(a2.begin(), a2.end(), [&](std::pair<int, std::string>i){
		v[i.first] = Json::Value(i.second);
	});
	std::for_each(a3.begin(), a3.end(), [&](std::pair<int, int>i){
		v[i.first] = Json::Value(i.second);
	});
	std::for_each(a4.begin(), a4.end(), [&](std::pair<std::string, std::vector<std::string> >i){
		std::for_each(i.second.begin(), i.second.end(), [&](std::string j){v[i.first].append(j); });
	});
	return v;
}

Json::Value NodeCmdLine::Parse(std::string& s)
{
	Json::Reader r;
	r.parse(s, v);
	return v;
}

void NodeCmdLine::AppendType()
{
	v[keyType] = Json::Value(valueType);
}

bool NodeCmdLine::Validate()
{
	return false;
}


std::string NodeCmdLine::keyType = "keytype";

std::string WorkerCmdline::task = "taskaddr";

std::string WorkerCmdline::heartBeaten = "heartbeaten";

std::string WorkerCmdline::state = "state";

std::string VentilatorCmdLine::function = "function";

std::string VentilatorCmdLine::jobReceiver = "jobreceiver";

std::string VentilatorCmdLine::taskAssigner = "taskassigner";

std::string VentilatorCmdLine::stateSender = "statesender";

std::string VentilatorCmdLine::heartBeaten = "heartbeaten";

std::string TopCmdLine::backEnd = "backend";

std::string TopCmdLine::proxy = "proxy";

std::string TopCmdLine::stateUp = "stateup";

bool WorkerCmdline::Validate()
{
	return v[WorkerCmdline::heartBeaten].isNull() == false && 
		v[WorkerCmdline::state].isNull() == false && v[WorkerCmdline::task].isNull() == false;
}


WorkerCmdline::WorkerCmdline()
{
	valueType = "workercmd";
}


bool VentilatorCmdLine::Validate()
{
	return v[VentilatorCmdLine::heartBeaten].isNull() == false &&
		v[VentilatorCmdLine::jobReceiver].isNull() == false && 
		v[VentilatorCmdLine::taskAssigner].isNull() == false && 
		v[VentilatorCmdLine::stateSender].isNull() == false;
}

VentilatorCmdLine::VentilatorCmdLine()
{
	valueType = "ventilatorcmd";
}


bool TopCmdLine::Validate()
{
	return v[TopCmdLine::backEnd].isNull() == false &&
		v[TopCmdLine::proxy].isNull() == false &&
		v[TopCmdLine::stateUp].isNull() == false &&
		v[TopCmdLine::stateDown].isNull() == false &&
		v[TopCmdLine::ventilator].isNull() == false;
}

TopCmdLine::TopCmdLine()
{
	valueType = "topcmd";
}


//std::string TopCmdLine::type = "topcmd";

std::string TopCmdLine::ventilator = "ventilator";

std::string TopCmdLine::stateDown = "statedown";

std::string TopCmdLine::jobSender ="jobsender";
