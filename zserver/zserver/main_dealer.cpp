#include "zmq.h"
#include "zmq.h"
#include "zhelpers.h"
#include <process.h>
#include <iostream>
#include <fstream>
#include "zserver.h"
using namespace std;
void* context = zmq_ctx_new();
HANDLE handle = CreateEvent(nullptr, true, false, "bigbird");

void worker_(void* argslist)
{
	WaitForSingleObject(handle, 0xffffffff);
	//std::cerr << "123" << std::endl;
	void* s = zmq_socket(context, ZMQ_SUB);
	auto z = zmq_connect(s, "inproc://#1");
	//  Subscribe for all messages.
	int rc = zmq_setsockopt(s, ZMQ_SUBSCRIBE, "", 0);
	std::cerr << z << std::endl;
	auto buf = s_recv(s);
	if (buf != nullptr)
		std::cerr << buf << std::endl;
	std::cerr << "over" << std::endl;
}

void main1()
{

/*	zServerConf z;*/
	//开启worker线程
	_beginthread(worker_, 0, nullptr);
	_beginthread(worker_, 0, nullptr);
	_beginthread(worker_, 0, nullptr);
	_beginthread(worker_, 0, nullptr);
	_beginthread(worker_, 0, nullptr);
	//建立server
	
	void *sender = zmq_socket(context, ZMQ_ROUTER);
	zmq_bind(sender, "inproc://#1");
	//读取文件
	char*str = new char[1024 * 1024 * 10];
	memset(str, 0, 1024 * 1024 * 10);
	std::ifstream input("D:\\vs\\vc\\vcvarsall.bat", std::ios::in | std::ios::binary);
	input >> str;
	SetEvent(handle);
	Sleep(1000);
	int size = s_send(sender, str);
 	s_send(sender, str);
// 	s_send(sender, str);
	cerr << "service ok:" <<size<< endl;
	system("pause");
}

void req(void* argslist)
{
	int id = GetCurrentThreadId ();
	WaitForSingleObject(handle, 0xffffffff);
	//std::cerr << "123" << std::endl;
	void* s = zmq_socket(context, ZMQ_REQ);
	auto z = zmq_connect(s, "inproc://#1");
	//  Subscribe for all messages.
	//int rc = zmq_setsockopt(s, ZMQ_SUBSCRIBE, "", 0);
	//std::cerr << z << std::endl;
	char t[20] = { 0 };
	_itoa_s(id,t,10);
	auto l = s_send(s,t);
	//cerr << "wait" << endl;
	auto b = s_recv(s);
	if (b != nullptr)
		std::cerr <<t<<":"<< b << std::endl;
//	std::cerr << "over" << std::endl;
}

void main2()
{
	//开启worker线程
	_beginthread(req, 0, nullptr);
	_beginthread(req, 0, nullptr);
	_beginthread(req, 0, nullptr);
	_beginthread(req, 0, nullptr);
	_beginthread(req, 0, nullptr);
	_beginthread(req, 0, nullptr);
	_beginthread(req, 0, nullptr);
	_beginthread(req, 0, nullptr);
	_beginthread(req, 0, nullptr);
	_beginthread(req, 0, nullptr);
	//开启服务器
	void *sender = zmq_socket(context, ZMQ_ROUTER);
	zmq_bind(sender, "inproc://#1");
	
	//
	char*str = new char[1024 * 1024 * 10];
	memset(str, 0, 1024 * 1024 * 10);
	std::ifstream input("D:\\vs\\vc\\vcvarsall.bat", std::ios::in | std::ios::binary);
	input >> str;
	SetEvent(handle);
	char* s;
	while ( (s = s_recv(sender)) != nullptr)
	{
		s_send(sender, s);
		Sleep(1000);
	//	break;
		//cerr << s << std::endl;
	}
	system("pause");
}