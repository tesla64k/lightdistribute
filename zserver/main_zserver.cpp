#include <process.h>
#include <iostream>
#include <fstream>
#include "ztop.h"
#include "zhelpers.h"
/*#include "locationventilator.h"*/
#include "json.h"


// void main3()
// {
// 	int in;
// 	Json::Reader r;
// 	Json::Value v;
// 	auto s = v["tag"];
// 	void* zctx = zmq_ctx_new();
// 	zparallel::zconf zconf;
// 	zconf.ctx = zctx;
// 	ztop* pztop = new ztop;	
//  	zconf.caseNode = "tcp://127.0.0.1:5556";
// 	zconf.state = "tcp://127.0.0.1:5557";
// 	zconf.backEnd = "tcp://127.0.0.1:5558";
// 	//std::vector<std::vector>
// /*	ztop::InitialTopConf(pztop,&zconf);*/
// 	pztop->signal();
// 	std::cin >> in;
// 	//	pztop->SetItems();
// 
// 	//pztop->LoopFrameWork();
// 
// }

void main()
{
	char buff[320] = { 0 };
	::GetPrivateProfileString("top", "ventilator", "", 
		buff, sizeof buff, "D:\\work\\ini\\server.ini");
	Json::Value vv;
	Json::Reader r;
	auto buff0 = "{\"name\" : \"Ð¡Â¥Ò»Ò¹Ìý´ºÓê\",\"age\" : 27}";
	r.parse(buff, vv);
	//vv.
	auto err = r.getFormattedErrorMessages();
	auto t = vv.toStyledString();
	Json::Value v0;
	Json::Value k(t);
	v0["123"] = k;//Json::Value(t);
	v0["224"] = k;
	auto s = vv.toStyledString();
	void* zctx = zmq_ctx_new();
	void*socket = zmq_socket(zctx, ZMQ_ROUTER);
	char buf[1000] = { 0 };
	int rc = zmq_bind(socket, "tcp://127.0.0.1:5556");
	zmq_msg_t msg;
	
	while (true)
	{
		zmq_msg_init(&msg);
		rc = zmq_msg_recv(&msg,socket,0);
		char* p = (char*)zmq_msg_data(&msg);
		std::string aa = p;
		rc = zmq_msg_recv(&msg, socket, 0);
		rc = zmq_msg_recv(&msg, socket, 0);
		std::cerr << zmq_strerror(zmq_errno()) << std::endl;
		zmq_msg_close(&msg);
		//rc = zmq_send()
	}
	
}
