#include "zmq.h"
#include <string>
#include <vector>


typedef int (*zfuncRep)(void*ctx, void*data, zmq_msg_t*);


struct ZServer
{
	zmq_msg_t				zMsg;									//传递消息体，考虑不用
	std::vector<HANDLE>		vPara;									//服务器及业务逻辑参数
	void*					socketPush;								//socket push
	void*					socketReq;								//socket reply
	char*					pushAddr;								//push address
	char*					reqAddr;								//reply address
	void*					context;								//网络上下文			
	int		ReqCenterMsg(char*data,int size);						//请求中央节点业务逻辑参数
	int		RecvCenterMsg(char*data,int size);						//接收中央节点数据
};

struct ZCenterNode{
	zmq_msg_t			zMsg;						//考虑不用
	std::vector<HANDLE>	vPara;						//中央服务器节点，及业务逻辑参数
	void*	socketRep;
	void*	socketReq;
	void*	context;
	char*	repAddr;
	char*	reqAddr;			//if top center null,other not null
	int		RepMsg(zfuncRep z,void* data);
};

struct ZSink{
	zmq_msg_t			zMsg;				//数据收发
	std::vector<HANDLE>	vSinkDist;			//数据汇总协议栈
	void*		context;
	void*		pullSocket;
	void*		reqSocket;
	char		pullAddr[512];
	char		reqAddr[512];
	int			RecvWorkerMsg();
};




int InitialServerConf(ZServer*conf);
int InitialSinkConf(ZSink*conf);
int InitialCenterConf0(ZCenterNode*conf);
int InitialCenterConf(ZCenterNode*conf);



int SendWorker(void*socket,zmq_msg_t*msg);


