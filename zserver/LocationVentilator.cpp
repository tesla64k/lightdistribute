#include "locationventilator.h"


locationventilator::locationventilator()
{
}


locationventilator::~locationventilator()
{
}

void locationventilator::SetParameter(std::vector<std::string>&v)
{

}

int locationventilator::ApplyTask(zmq_msg_t* zmsg)
{
	int rc = zventilator::ApplyTask(zmsg);
	int64_t more = 0;
	size_t more_size = sizeof more;
	do 
	{
		rc = zmq_msg_init(zmsg);
		rc = zmq_msg_recv(zmsg, applyTaskSocket, ZMQ_MORE);
		if (rc == -1)
			goto error;
		rc = zmq_getsockopt(applyTaskSocket, ZMQ_RCVMORE, &more, &more_size);
		if (rc == -1)
			goto error;
		SetParameter(vTaskParameter);
		zmq_msg_close(zmsg);
	} while (more);
	if (pInitalTask != nullptr)
		pInitalTask(vTaskParameter);
	return rc;
error:
	return zmq_errno();
}

int locationventilator::DistributeTask(void*taskData)
{
	return single(taskData);
}

int locationventilator::sloat()
{
	void*pdata = nullptr;
	zmq_msg_t sendmsg;
	int rc = 0;
	do
	{
		rc = zmq_recv(sloatSocket, pdata, sizeof(void*), 0);
		if (rc == -1)
			goto error;
		if (rc == -1)
			goto error;
		sendmsg = pPackageData(pdata);
		rc = zmq_msg_send(&sendmsg, taskDataSendSocket, ZMQ_DONTWAIT);
	error:
		;
	} while (true);

	return rc;
}

int locationventilator::single(void*p)
{
	int rc = zmq_send(signalSocket, p, sizeof(void*), 0);
	return rc;
}
