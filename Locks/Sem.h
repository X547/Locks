#ifndef _SEM_H_
#define _SEM_H_

#include <OS.h>

class Sem
{
private:
	int32 fValue;

public:
	Sem(int32 value);
	status_t Acquire(uint32 flags = 0, bigtime_t timeout = B_INFINITE_TIMEOUT);
	void Release();
};

#endif	// _SEM_H_
