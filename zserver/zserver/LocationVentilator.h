#pragma once
#include "zventilator.h"
#include "zframeworkbase.h"

class locationventilator:public zventilator
{
public:
	locationventilator();
	~locationventilator();
private:
	int								emitterCount;								//发射器数量
public:
	std::vector<std::string>		vTaskParameter;
	void*							sloatSocket;								//pull
	void*							signalSocket;								//push
public:
	virtual int		ApplyTask(zmq_msg_t* zmsg);
	virtual int		DistributeTask(void*taskData);				//发送接口
private:
	virtual void	SetParameter(std::vector<std::string>&);
	int				single(void*);									//信号量，启动任务发送
	int				sloat();										//另起线程，接收任务传入发送队列
};
