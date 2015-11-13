#pragma once
#include "zventilator.h"
#include "zframeworkbase.h"

class locationventilator:public zventilator
{
public:
	locationventilator();
	~locationventilator();
private:
	int								emitterCount;								//����������
public:
	std::vector<std::string>		vTaskParameter;
	void*							sloatSocket;								//pull
	void*							signalSocket;								//push
public:
	virtual int		ApplyTask(zmq_msg_t* zmsg);
	virtual int		DistributeTask(void*taskData);				//���ͽӿ�
private:
	virtual void	SetParameter(std::vector<std::string>&);
	int				single(void*);									//�ź���������������
	int				sloat();										//�����̣߳����������뷢�Ͷ���
};
