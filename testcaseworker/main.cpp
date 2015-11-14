#include "json.h"
#include "zframeworkbase.h"
#include "txtconf.h"
void main()
{
	Json::Reader r;
	ServiceConf service;
	service.ReadConf("D:\\zserver\\server.ini", ServiceConf::worker.c_str());
	auto __ = service.cmdline;
	std::cerr << __ << std::endl;
	zparallel::_servicetype _type = zparallel::GetService(service.cmdline);
	if (_type == zparallel::_servicetype::TOPPROXY)
	{
		zparallel::TopLoop();
	}
	else if (_type == zparallel::_servicetype::VENTILATOR)
	{
		zparallel::VentilatorLoop();
	}
	else if (_type == zparallel::_servicetype::SLAVER)
	{
		zparallel::StartWork();
	}
	system("pause");
}