#include "Mutex.h"

#include <private/system/syscalls.h>
#include <private/system/user_mutex_defs.h>
#include <stdio.h>

#define CheckRet(err) {status_t _err = (err); if (_err < B_OK) return _err;}


Mutex::Mutex(bool acquireFirst): fValue(acquireFirst? B_USER_MUTEX_LOCKED: 0)
{}

status_t Mutex::Acquire(uint32 flags, bigtime_t timeout)
{
	int32 oldValue;

	// can be repeated for spinlock
	oldValue = atomic_or(&fValue, B_USER_MUTEX_LOCKED);
	if (
		(oldValue & (B_USER_MUTEX_LOCKED | B_USER_MUTEX_WAITING)) == 0 ||
		(oldValue & B_USER_MUTEX_DISABLED) != 0
	) {
		return B_OK;
	}

	status_t res;
	do {
		res = _kern_mutex_lock(&fValue, NULL /* name */, flags, timeout);
	} while (res == B_INTERRUPTED);

	return res;
}

void Mutex::Release(bool releaseAll)
{
	int32 oldValue = atomic_and(&fValue, ~(int32)B_USER_MUTEX_LOCKED);
	if (
		(oldValue & B_USER_MUTEX_WAITING) != 0 &&
		(oldValue & B_USER_MUTEX_DISABLED) == 0
	) {
		// will set B_USER_MUTEX_LOCKED
		_kern_mutex_unlock(&fValue, releaseAll ? B_USER_MUTEX_UNBLOCK_ALL : 0);
	}
}

void Mutex::ReleaseIfWaiting(bool releaseAll)
{
	// keep B_USER_MUTEX_LOCKED flag
	if ((atomic_get(&fValue) & B_USER_MUTEX_WAITING) != 0)
		Release(releaseAll);
}

status_t Mutex::SwitchFrom(Mutex &src, uint32 flags, bigtime_t timeout)
{
// equivalent code
#if 0
ATOMIC {
	src.Release();
	return Acquire(flags, timeout);
}
#endif
	status_t res = _kern_mutex_switch_lock(&src.fValue, &fValue, NULL /* name */, flags, timeout);
	while (res == B_INTERRUPTED) {
		res = _kern_mutex_lock(&fValue, NULL /* name */, flags, timeout);
	}
	return res;
}

void Mutex::PrintToStream()
{
	int32 value = fValue;
	printf("{");
	bool first = true;
	for (int i = 0; i < 32; i++) {
		if (((1 << i) & value) != 0) {
			if (first) {first = false;} else {printf(", ");}
			switch (i) {
			case 0: printf("locked"); break;
			case 1: printf("waiting"); break;
			case 2: printf("disabled"); break;
			default: printf("?(%d)", i);
			}
		}
	}
	printf("}");
}
