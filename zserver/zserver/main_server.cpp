#include "zmq.h"
#include "zmq.h"
#include "zhelpers.h"
#include <process.h>
#include <iostream>
#include <fstream>
#include "zserver.h"
using namespace std;
void* context1 = zmq_ctx_new();

char* gCenterAddr = "tcp://127.0.0.1:9981";
char* gServerAddr = "tcp://127.0.0.1:9982";
int gMsgsize1 = 100 * 1024 * 1024;

int zTestFunc1(void*ctx,void*data,zmq_msg_t* z)
{
	return 0;
}

void main()
{
	ZServer zserver;
	ZSink zsink;
	ZCenterNode zct;
	zserver.context = context1;
	zserver.reqAddr = gCenterAddr;
	zserver.pushAddr = gServerAddr;
	zsink.context = context1;
	zct.context = context1;
	zct.repAddr = gCenterAddr;
	int ret = InitialCenterConf0(&zct);
	ret = InitialServerConf(&zserver);
	ret = InitialSinkConf(&zsink);

	char* msg = new char[gMsgsize1];
	ret = zserver.ReqCenterMsg(msg,gMsgsize1);
	//zct.RepMsg();
	return;
}