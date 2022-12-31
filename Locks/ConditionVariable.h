#ifndef _CONDITIONVARIABLE_H_
#define _CONDITIONVARIABLE_H_

#include <OS.h>

#include "Mutex.h"

class RecursiveLock;


class ConditionVariable
{
private:
	RecursiveLock *fMutex;
	int32 fWaiterCnt;
	Mutex fLock;

public:
	ConditionVariable();
	status_t Acquire(RecursiveLock &mutex, uint32 flags = 0, bigtime_t timeout = B_INFINITE_TIMEOUT);
	void Release(bool releaseAll = false);
};

#endif	// _CONDITIONVARIABLE_H_
