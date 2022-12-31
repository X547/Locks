#include "Sem.h"

#include <private/system/syscalls.h>


static int32 atomic_add_if_greater(int32* value, int32 amount, int32 testValue)
{
	int32 current = atomic_get(value);
	while (current > testValue) {
		int32 old = atomic_test_and_set(value, current + amount, current);
		if (old == current)
			return old;
		current = old;
	}
	return current;
}


Sem::Sem(int32 value):
	fValue(value)
{
}

status_t Sem::Acquire(uint32 flags, bigtime_t timeout)
{
	int32* sem = &fValue;
	int32 oldValue = atomic_add_if_greater(sem, -1, 0);
	if (oldValue > 0)
		return B_OK;

	status_t res;
	do {
		res = _kern_mutex_sem_acquire(sem, NULL /* name */, flags, timeout);
	} while (res == B_INTERRUPTED);

	return res;
}

void Sem::Release()
{
	int32* sem = &fValue;
	int32 oldValue = atomic_add_if_greater(sem, 1, -1);
	if (oldValue > -1)
		return;

	_kern_mutex_sem_release(sem);
}
