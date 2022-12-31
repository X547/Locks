#include "ConditionVariable.h"
#include "RecursiveLock.h"

#include <private/system/syscalls.h>
#include <private/system/user_mutex_defs.h>
#include <stdio.h>


ConditionVariable::ConditionVariable(): fMutex(NULL), fWaiterCnt(0), fLock(true)
{}

status_t ConditionVariable::Acquire(RecursiveLock &mutex, uint32 flags, bigtime_t timeout)
{
	if (mutex.Holder() != find_thread(NULL))
		return EPERM;

	if (fMutex != NULL && fMutex != &mutex)
		return EINVAL;

	fMutex = &mutex;
	fWaiterCnt++;

	// switch mutex from mutex to fLock
	int32 oldCount = mutex.LockCount();
	mutex.fHolder = -1;
	mutex.fLockCnt = 0;
	status_t status = fLock.SwitchFrom(mutex.fMutex, flags, timeout);

	// switch back
	mutex.Acquire();
	mutex.fLockCnt = oldCount;

	fWaiterCnt--;
	if (fWaiterCnt == 0)
		fMutex = NULL;

	return status;
}

void ConditionVariable::Release(bool releaseAll)
{
	fLock.ReleaseIfWaiting(releaseAll);
}
