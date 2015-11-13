#include "ztop.h"
#include <map>
#include <iostream>
#include "json.h"


ztop::ztop()
{
	//nodecmd = new TopCmdLine;
}


ztop::~ztop()
{
	//delete nodecmd;
}

int ztop::signal()
{
	auto $parseJson = [](zmq_msg_t*pmsg, Json::Reader* reader, Json::Value*pvalue)
	{
		void*data = zmq_msg_data(pmsg);
		bool ret = reader->parse((char*)data, "\0", *pvalue);
		if (!ret)
		{
			std::string err = reader->getFormattedErrorMessages();
			std::cerr << __FUNCTION__ << ":" << err << std::endl;
		}
		if (!ret)
		{
			std::string err = reader->getFormattedErrorMessages();
			std::cerr << __FUNCTION__ << ":" << err << std::endl;
		}
	};
	auto $sendTask = [](zmq_msg_t*pmsg, Json::Value*pvalue,ztop*This){
		int rc = 0;
		void* distSocket = nullptr;
		Json::Value&value = *pvalue;
		Json::Value tmpvalue;
		if (value.type() == Json::objectValue)
		{
			tmpvalue = value[TAG];
		}
		else
			std::cerr << __FUNCTION__ << ":" << "unknown recv:" << value.toStyledString() << std::endl;
		if (tmpvalue.type() == Json::stringValue)
		{
			std::string tagvalue = value[TAG].asString();
			distSocket = This->pairs[tagvalue];	
			if (distSocket == nullptr)
			{
				std::cerr << __FUNCTION__ << ":" << "unknown service ===>"
					<<tagvalue << std::endl;
			}
			else
				rc = zmq_msg_send(pmsg, distSocket, ZMQ_DONTWAIT);
		}
		if (rc == -1)
		{
			std::cerr << __FUNCTION__ << " send task err:" << value[TAG] << 
				"==========>" <<
				zmq_strerror(zmq_errno()) << std::endl;
		}
	};


	int rc = 0;
	zmq_msg_t msg;
	Json::Value value;
	Json::Reader reader;
	while (true)
	{
		rc = zmq_poll(&vItems[0], vItems.size(), -1);
		rc = zmq_errno();
		for (auto i = 0; i < vItems.size();i++)
		{
			zmq_pollitem_t& t = vItems[i];
			if ((t.revents &ZMQ_POLLIN) && i == 0)						//按照初始化顺序，case通道第一
			{
				rc = zparallel::zMsgRecvOnce(t.socket, &msg);
				if (rc == -1)
				{
					std::cerr << __FUNCTION__ << ":" << "task recv failed" << std::endl; 
					continue;
				}				
				$parseJson(&msg, &reader, &value);
				$sendTask(&msg, &value, this);
			}
			else if ((t.revents &ZMQ_POLLIN) && i == 1)					//状态通道
			{
				;
			}
			else if ((t.revents &ZMQ_POLLIN) && i == 2)
			{
				;
			}
		}
	}
}

bool ztop::InitialTopConf(ztop*pztop, zparallel::zconf*zconf)
{ 
	pztop->ctx = zconf->ctx;
	InitialJob(zconf->caseNode, pztop);
	bool s = InitialState(zconf->state, pztop);
	s = InitialService(zconf->vServiceToAddr, pztop);
	InitialFeedBack(zconf->backEnd, pztop);
	return s;
}

bool ztop::InitialService(std::vector<std::pair<std::string, std::string>> servicePair, ztop*pztop)
{

	auto lambda_setsocket = [](std::pair<std::string, std::string>& sPair, ztop*pztop){
		void*s = zmq_socket(pztop->ctx, ZMQ_PUSH);
		int rc = zmq_bind(s, sPair.second.c_str());
		if (rc != -1)
		{
			pztop->pairs.insert(std::make_pair(sPair.first, s));
			pztop->vItems.push_back({ s, 0, ZMQ_POLLIN, 0 });
		}
		else
			std::cerr << __FUNCTION__ << ":"<<"service not start,with err:=====>" << zmq_strerror(zmq_errno());
	};
	for_each(servicePair.begin(), servicePair.end(), [&](std::pair<std::string, std::string>& sPair){
		lambda_setsocket(sPair, pztop);
	});
	return true;
}



//状态队列
bool ztop::InitialState(std::string stateAddr, ztop*pztop)
{
	void*s = zmq_socket(pztop->ctx, ZMQ_SUB);
	int rc = zmq_bind(s, (pztop->stateDownAddr = stateAddr).c_str());
	if (rc != -1)
	{
		pztop->vItems.push_back({ s, 0, ZMQ_POLLIN, 0 });
		pztop->stateDownSocket = s;
		return false;
	}
	else
		std::cerr << __FUNCTION__ << ":" << "state service not start,with err:=====>" << zmq_strerror(zmq_errno());
	return true;
}

void ztop::InitialFeedBack(std::string feedback, ztop*pztop)
{
	void*s = zmq_socket(pztop->ctx, ZMQ_PUSH);
	int rc = zmq_connect(s, feedback.c_str());
	if (rc != -1)
	{
		pztop->backEndAddr = feedback;
		pztop->backEndSocket = s;
	}
	else
	{
		std::cerr << __FUNCTION__ << ":" << "cant connect backend service: ====>" << feedback << " ,with err:=====>" << zmq_strerror(zmq_errno());
		Sleep(1000);
		InitialFeedBack(feedback, pztop);
	}
}

void ztop::InitialJob(std::string casenode, ztop*pztop)
{
	void*s = zmq_socket(pztop->ctx, ZMQ_PULL);
	pztop->jobAddr = casenode;
	int ret = zmq_connect(s, pztop->jobAddr.c_str());
	if (ret == -1)
	{
		std::cerr << "cant connect over top service======>" << 
			zmq_strerror(zmq_errno())<<"again......"<<std::endl;
		Sleep(1000);
		InitialJob(casenode, pztop);
		return;
	}
	pztop->jobSocket = s;
	pztop->vItems.push_back({ pztop->jobSocket, 0, ZMQ_POLLIN, 0 });
}
