#include "zserver.h"


int SendWorker(void*socket, zmq_msg_t*msg)
{
	
	return 0;
}

int InitialServerConf(ZServer*conf)
{
	conf->socketPush = zmq_socket(conf->context, ZMQ_PUSH);
	conf->socketReq = zmq_socket(conf->context, ZMQ_REQ);
	int ret = zmq_bind(conf->socketPush, conf->pushAddr);
	ret = zmq_connect(conf->socketReq, conf->reqAddr);
	return ret;
}

int InitialSinkConf(ZSink*conf)
{
	conf->pullSocket = zmq_socket(conf->context, ZMQ_PULL);
	int ret = zmq_bind(conf->pullSocket, conf->pullAddr);
	return ret;
}

int InitialCenterConf0(ZCenterNode*conf)
{
	conf->socketRep = zmq_socket(conf->context, ZMQ_REP);
	int ret = zmq_bind(conf->socketRep, conf->repAddr);
/*	ret = zmq_connect(conf->socketReq, conf->reqAddr);*/
	return ret;
}

int ZServer::ReqCenterMsg(char*data, int size)
{
	zmq_msg_t	msg;
	zmq_msg_init_size(&msg,size);
	return zmq_msg_send(&msg, this->socketReq, ZMQ_DONTWAIT);
}

int ZCenterNode::RepMsg(zfuncRep z,void* data)
{
	int rc = 0;
	while (true)
	{
		zmq_msg_t		msg;
		int rc = zmq_msg_init(&msg);
		rc = zmq_msg_recv(&msg, socketRep, 0);
		zmq_msg_close(&msg);
		z(context, data, &msg);
		if (!zmq_msg_more(&msg))
			break;
	}
	return rc;
}
