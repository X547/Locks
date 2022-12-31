#include <stdio.h>
#include <functional>

#include <OS.h>

#include "Locks/Mutex.h"
#include "Locks/RecursiveLock.h"
#include "Locks/ConditionVariable.h"

RecursiveLock gPrintLock;

Mutex gMutex, gMutex2;
RecursiveLock gLock;
ConditionVariable gCondVar;


class Thread {
public:
	int fId;
	std::function<status_t(Thread&)> fEntry;
	thread_id fThread;
	
public:
	Thread(int id, std::function<status_t(Thread&)> entry): fId(id), fEntry(entry)
	{
		char str[256];
		sprintf(str, "thread %d", fId);
		fThread = spawn_thread(Entry, str, B_NORMAL_PRIORITY, this);
		resume_thread(fThread);
	}

	static status_t Entry(void *arg)
	{
		Thread &t = *(Thread*)arg;
		printf("+Thread(%d)\n", t.fId);
		status_t res = t.fEntry(t);
		printf("-Thread(%d)\n", t.fId);
		return res;
	}
};

void Do1()
{
	status_t res;
	gMutex.Acquire();
	
	Thread *t1 = new Thread(1,
		[](Thread &t) -> status_t {
			gLock.Acquire();
			gCondVar.Acquire(gLock);
			return B_OK;
		}
	);
	Thread *t2 = new Thread(2,
		[](Thread &t) -> status_t {
			snooze(100000);
			gLock.Acquire();
			snooze(200000);
			gLock.Release();
			return B_OK;
		}
	);
	Thread *t3 = new Thread(3,
		[](Thread &t) -> status_t {
			snooze(200000);
			gCondVar.Release();
			return B_OK;
		}
	);
	
	snooze(10000);

#if 0
	gMutex2.Acquire();
	printf("[1] gMutex: "); gMutex.PrintToStream(); printf(", gMutex2: "); gMutex2.PrintToStream(); printf("\n");
	gMutex2.SwitchFrom(gMutex);
	printf("[2] gMutex: "); gMutex.PrintToStream(); printf(", gMutex2: "); gMutex2.PrintToStream(); printf("\n");
#endif
/*
	snooze(10000);
	printf("[WAIT]"); getc(stdin); gMutex.Release(false); printf("gMutex: "); gMutex.PrintToStream(); printf("\n");
	printf("[WAIT]"); getc(stdin); gMutex.Release(false); printf("gMutex: "); gMutex.PrintToStream(); printf("\n");
	printf("[WAIT]"); getc(stdin); gMutex.Release(false); printf("gMutex: "); gMutex.PrintToStream(); printf("\n");
	printf("[WAIT]"); getc(stdin); gMutex.Release(false); printf("gMutex: "); gMutex.PrintToStream(); printf("\n");
*/

	wait_for_thread(t1->fThread, &res);
	wait_for_thread(t2->fThread, &res);
	wait_for_thread(t3->fThread, &res);
}

void Do2()
{
	status_t res;

	Thread *t1 = new Thread(1,
		[](Thread &t) -> status_t {
			gMutex.Acquire();
			printf("[WAIT]"); getc(stdin);
			gMutex.Release();
			return B_OK;
		}
	);

	snooze(10000);
	gMutex.Acquire();
	
	wait_for_thread(t1->fThread, &res);
}

int main()
{
	Do2();
	return 0;
}
