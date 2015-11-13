#include "dispatch.h"

namespace zparallel{

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