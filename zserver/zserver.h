#include "zmq.h"
#include <string>
#include <vector>


typedef int (*zfuncRep)(void*ctx, void*data, zmq_msg_t*);


struct ZServer
{
	zmq_msg_t				zMsg;									//������Ϣ�壬���ǲ���
	std::vector<HANDLE>		vPara;									//��������ҵ���߼�����
	void*					socketPush;								//socket push
	void*					socketReq;								//socket reply
	char*					pushAddr;								//push address
	char*					reqAddr;								//reply address
	void*					context;								//����������			
	int		ReqCenterMsg(char*data,int size);						//��������ڵ�ҵ���߼�����
	int		RecvCenterMsg(char*data,int size);						//��������ڵ�����
};

struct ZCenterNode{
	zmq_msg_t			zMsg;						//���ǲ���
	std::vector<HANDLE>	vPara;						//����������ڵ㣬��ҵ���߼�����
	void*	socketRep;
	void*	socketReq;
	void*	context;
	char*	repAddr;
	char*	reqAddr;			//if top center null,other not null
	int		RepMsg(zfuncRep z,void* data);
};

struct ZSink{
	zmq_msg_t			zMsg;				//�����շ�
	std::vector<HANDLE>	vSinkDist;			//���ݻ���Э��ջ
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


