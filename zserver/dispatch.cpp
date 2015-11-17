#include "dispatch.h"
#include "zframeworkbase.h"

namespace zparallel{

	bool dispatch::GroupBy(zparallel::ztaskstatusframe* zfs, std::deque<task_t*>*, std::string&strerr)
	{
		return false;
	}

	Json::Value dispatch::EnCode(task_t*pTask)
	{
		return Json::Value();
	}

	void dispatch::DeCode(Json::Value* pvalue)
	{

	}

	dispatch* dispatch::share_pdispatch = nullptr;

	void dispatch::Hook(dispatch*pdispatch)
	{
		share_pdispatch = pdispatch;
	}

	dispatch* dispatch::GetInstanse()
	{
		return share_pdispatch;
	}

	void RegisterDispatch(dispatch*p)
	{
		dispatch::Hook(p);
	}
}