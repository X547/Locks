#ifndef _AUTOLOCK_H_
#define _AUTOLOCK_H_

#include "RecursiveLock.h"

template <typename RecursiveLock>
class AutoLock
{
private:
	RecursiveLock *fLock;
	bool fIsLocked;

public:
	AutoLock(RecursiveLock &lock): fLock(&lock)
	{
		fIsLocked = fLock->Acquire() >= B_OK;
	}

	~AutoLock()
	{
		if (fIsLocked)
			fLock->Release();
	}

	RecursiveLock &Get() const {return *fLock;}

	status_t Acquire(uint32 flags = 0, bigtime_t timeout = B_INFINITE_TIMEOUT)
	{
		status_t res = B_OK;
		if (!fIsLocked) {
			res = fLock->Acquire(flags, timeout);
			fIsLocked = res >= B_OK;
		}
		return res;
	}
	
	void Release()
	{
		if (fIsLocked) {
			fLock->Release();
			fIsLocked = false;
		}
	}
	
	bool IsLocked() const {return fIsLocked;}
	
};

#endif	// _AUTOLOCK_H_
