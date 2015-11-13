#include "zmq.h"

struct ZWorkerConf
{
	void*	socketPull;					//socket pull
	void*	socketPush;					//socket push
	void*	socketReq;					//socket req
	char	serverAddr[512];			//push address
	char	sinkAddr[512];				//push address
	char	centerAddr[512];			//���Ľڵ��ַ
};

struct ZTransit
{
	void* socketReq;					//req socket ask server for data
	void* socketRouter;					//
	char  RouterAddr[512];				//push address
};

int InitialWorkConf(ZWorkerConf*conf);