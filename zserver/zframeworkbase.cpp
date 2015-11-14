#include "zframeworkbase.h"
#include "zmq.h"
#include "ztop.h"
#include "zventilator.h"
#include "NodeCmdLine.h"
#include "worker.h"


void* ctx_servicesocketgod = zmq_ctx_new();

NodeCmdLine* Reflect(NodeCmdLine*pnode);

std::string OnCMD(std::string cmd);

int zparallel::zMsgRecvOnce(void*socekt, zmq_msg_t*pzmsg,int mode)
{
 	zmq_msg_init(pzmsg);
	return zmq_msg_recv(pzmsg, socekt, mode);
}


zmq_msg_t zparallel::CreateStateMsg(int jobId, int taskId, int type, int stateCase, std::string info /*=""*/)
{
	zmq_msg_t msg;
	Json::Value v;
	v[KEY_STATEJOBID] = jobId;
	v[KEY_STATETASKID] = taskId;
	v[KEY_STATETYPE] = type;
	v[KEY_STATECASE] = stateCase;
	v[KEY_STATEINFO] = info;
	auto s = v.toStyledString();
	zmq_msg_init_size(&msg, s.size());
	memcpy(zmq_msg_data(&msg), s.c_str(), s.size());
	return msg;
}

std::string OnCMD(std::string cmd)
{
	/*NodeCmdLine::Parse(cmd);*/
	NodeCmdLine nodecmd;
	nodecmd.Parse(cmd);
	return nodecmd.v[nodecmd.keyType].asString();
}

zparallel::_servicetype zparallel::GetService(std::string cmd)
{
	if (ctx_servicesocketgod == nullptr)
	{
		std::cerr << "context initial fail!" << std::endl;
		return NONE;
	}
	NodeCmdLine nodecmd;
	NodeCmdLine*pnodecmd = nullptr;
	std::string type = OnCMD(cmd);
	zparallel::_servicetype ret = NONE;
	if (type == TopCmdLine().valueType )
	{
		pnodecmd = new TopCmdLine;
		ret = TOPPROXY;
	}
	else if(type == VentilatorCmdLine().valueType)
	{
		pnodecmd = new VentilatorCmdLine;
		ret = VENTILATOR;
	}
	else if (type == WorkerCmdline().valueType)
	{
		pnodecmd = new WorkerCmdline;
		ret = SLAVER;
	}
	if (pnodecmd)
		pnodecmd->Parse(cmd);
	else
	{
		ret = NONE;
		goto endline;
	}
	
	if (pnodecmd->Validate())
	{
		ret = Reflect(pnodecmd) != nullptr? ret : NONE ;
	}
	else
	{
		ret = NONE;
	}
endline:
	return ret;
}
NodeCmdLine* G_PNode = nullptr;

bool zparallel::TopLoop()
{
 	ztop* ptop = dynamic_cast<ztop*>(G_PNode);
	auto ret = ptop->InitialTop();
	ret = ptop->signal();
	return ret;
}

bool zparallel::VentilatorLoop()
{
	zventilator* pven = dynamic_cast<zventilator*>(G_PNode);
	auto ret = pven->StartVentilator();
	pven->VentilatorLoop();
	return ret;
}

bool zparallel::StartWork()
{
	worker* pven = dynamic_cast<worker*>(G_PNode);
	pven->StartWork();
	return true;
}

NodeCmdLine* Reflect(NodeCmdLine*pnode)
{	
	if (pnode->valueType == WorkerCmdline().valueType )
	{
		WorkerCmdline*pworkercmd = dynamic_cast<WorkerCmdline*>(pnode);
		G_PNode = new worker;
		worker* pworker = dynamic_cast<worker*>(G_PNode);
		pworker->taskAddr = pworkercmd->v[WorkerCmdline::task].asString();
		pworker->heartBeatAddr = pworkercmd->v[WorkerCmdline::heartBeaten].asString();
		pworker->stateAddr = pworkercmd->v[WorkerCmdline::state].asString();
		pworker->ctx = ctx_servicesocketgod;
	}
	else if (pnode->valueType == TopCmdLine().valueType)
	{
		TopCmdLine*ptopcmd = dynamic_cast<TopCmdLine*>(pnode);
		G_PNode = new ztop;
		ztop* ptop = dynamic_cast<ztop*>(G_PNode);
		ptop->backEndAddr = ptopcmd->v[TopCmdLine::backEnd].asString();
		//ptop->stateUp = ptopcmd->stateUp;
		ptop->jobAddr = ptopcmd->v[TopCmdLine::proxy].asString(); //ptopcmd->job;
		ptop->stateDownAddr = ptopcmd->v[TopCmdLine::stateDown].asString();//ptopcmd->stateDown;
		ptop->ctx = ctx_servicesocketgod;
		auto r = ptopcmd->v[TopCmdLine::ventilator];
		for (size_t i=0;i< ptopcmd->v[TopCmdLine::ventilator].size();i++)
		{
			ptop->vServiceToAddr.push_back(std::make_pair(
				ptopcmd->v[TopCmdLine::ventilator][i][VentilatorCmdLine::function].asString(),
				ptopcmd->v[TopCmdLine::ventilator][i][TopCmdLine::jobSender].asString()
				));
		}
		
	}
	else if (pnode->valueType == VentilatorCmdLine().valueType)
	{
		VentilatorCmdLine*pvencmd = dynamic_cast<VentilatorCmdLine*>(pnode);
		G_PNode = new zventilator;
		zventilator* pven = dynamic_cast<zventilator*>(G_PNode);
		pven->taskAssignerAddr = pvencmd->v[VentilatorCmdLine::taskAssigner].asString() ;
		pven->proxyAddr = pvencmd->v[VentilatorCmdLine::jobReceiver].asString();
		pven->stateSenderAddr = pvencmd->v[VentilatorCmdLine::stateSender].asString();
		pven->heartBeatenAddr = pvencmd->v[VentilatorCmdLine::heartBeaten].asString();
		pven->ctx = ctx_servicesocketgod;
	}
	return G_PNode;
}

