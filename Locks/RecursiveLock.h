#ifndef _RECURSIVELOCK_H_
#define _RECURSIVELOCK_H_

#include <OS.h>

#include "Mutex.h"


class RecursiveLock
{
public:
	Mutex fMutex;
	thread_id fHolder;
	int32 fLockCnt;

public:
	RecursiveLock();
	status_t Acquire(uint32 flags = 0, bigtime_t timeout = B_INFINITE_TIMEOUT);
	void Release();
	status_t Yield(uint32 flags = 0, bigtime_t timeout = B_INFINITE_TIMEOUT);
	inline thread_id Holder() {return fHolder;}
	inline int32 LockCount() {return fLockCnt;}
};

#endif	// _RECURSIVELOCK_H_
