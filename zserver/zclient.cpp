#include "zclient.h"

int InitialWorkConf(ZWorkerConf*conf)
{
	int ret = zmq_connect(conf->socketPull, conf->serverAddr);
	ret = zmq_connect(conf->socketPush, conf->sinkAddr);
	ret = zmq_connect(conf->socketReq, conf->centerAddr);
	return ret;
}

