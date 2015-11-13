#include "zmq.h"
#include "zmq.h"
#include "zhelpers.h"
#include <process.h>
#include <iostream>
#include <fstream>
//#include <iosfwd>
using namespace std;

void* context = zmq_ctx_new();
HANDLE handle = CreateEvent(nullptr, true, false, "bigbird");

void worker(void* argslist)
{
	WaitForSingleObject(handle, 0xffffffff);
	//std::cerr << "123" << std::endl;
	void* s = zmq_socket(context, ZMQ_PULL);
	auto z = zmq_connect(s, "inproc://#1");
	std::cerr << z << std::endl;
	auto buf = s_recv(s);
	if (buf != nullptr)
		std::cerr << buf << std::endl;
	std::cerr << "over" << std::endl;
}


void main_1()
{
	//开启worker线程
	_beginthread(worker, 0, nullptr);
	_beginthread(worker, 0, nullptr);
	_beginthread(worker, 0, nullptr);
	_beginthread(worker, 0, nullptr);
	_beginthread(worker, 0, nullptr);
	//建立server
	void *sender = zmq_socket(context, ZMQ_PUSH);
	zmq_bind(sender,"inproc://#1");	
	//读取文件
	char*str = new char[1024*1024*10];
	memset(str, 0, 1024 * 1024 * 10);
	std::ifstream input("D:\\vs\\vc\\vcvarsall.bat", std::ios::in | std::ios::binary);
	input >> str;
	SetEvent(handle);
//	Sleep(1000);
	s_send(sender,str);
	s_send(sender, str);
	s_send(sender, str);
	cerr << "service ok"<<endl;
	system("pause");
}

