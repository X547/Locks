#include "RecursiveLock.h"

#define CheckRet(err) {status_t _err = (err); if (_err < B_OK) return _err;}


RecursiveLock::RecursiveLock():
	fHolder(-1),
	fLockCnt(0)
{}

status_t RecursiveLock::Acquire(uint32 flags, bigtime_t timeout)
{
	thread_id thread = find_thread(NULL);
	if (thread != fHolder) {
		CheckRet(fMutex.Acquire(flags, timeout));
		fHolder = thread;
	}
	fLockCnt++;
	return B_OK;
}

void RecursiveLock::Release()
{
	if (find_thread(NULL) != fHolder) {
		debugger("RecursiveLock unlocked by non-holder thread!");
		return;
	}
	fLockCnt--;
	if (fLockCnt == 0) {
		fHolder = -1;
		fMutex.Release();
	}
}

status_t RecursiveLock::Yield(uint32 flags, bigtime_t timeout)
{
	if (find_thread(NULL) != fHolder) {
		debugger("Yield called by non-holder thread!");
		return B_ERROR;
	}
	int32 oldCount = LockCount();
	fHolder = -1;
	fLockCnt = 0;
	fMutex.Release();

	CheckRet(Acquire(flags, timeout));
	fLockCnt = oldCount;
	return B_OK;
}
