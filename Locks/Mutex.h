#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <OS.h>

class Mutex
{
public:
	int32 fValue;

public:
	Mutex(bool acquireFirst = false);
	status_t Acquire(uint32 flags = 0, bigtime_t timeout = B_INFINITE_TIMEOUT);
	void Release(bool releaseAll = false);
	void ReleaseIfWaiting(bool releaseAll = false);
	status_t SwitchFrom(Mutex &src, uint32 flags = 0, bigtime_t timeout = B_INFINITE_TIMEOUT);
	void PrintToStream();
};

#endif	// _MUTEX_H_
