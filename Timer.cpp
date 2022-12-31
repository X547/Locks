#include "Timer.h"
#include "RecursiveLock.h"
#include "ConditionVariable.h"
#include <stdio.h>

TimerRoster gTimerRoster;


Timer::Timer()
{
}

Timer::~Timer()
{}


TimerRoster::TimerRoster(): fLooperControl(0), fRun(true)
{
	printf("+TimerRoster\n");
	fThread = spawn_thread([](void *arg) -> status_t {return ((TimerRoster*)arg)->ThreadEntry();}, "timer", B_NORMAL_PRIORITY, this);
	resume_thread(fThread);
}

TimerRoster::~TimerRoster()
{
	printf("-TimerRoster\n");
	return;

	fLock.Acquire();
	fRun = false;
	fLooperControl.Release();
	fLock.Release();
	int32 res;
	wait_for_thread(fThread, &res);
}

void TimerRoster::Schedule(Timer *t, bigtime_t time)
{
	fLock.Acquire();
	t->fTime = time;
	Timer *it = fTimers.First();
	while (it != NULL && !(time - it->fTime < 0))
		it = fTimers.GetNext(it);
	fTimers.InsertBefore(it, t);
	Reschedule();
	fLock.Release();
}

void TimerRoster::Cancel(Timer *t)
{
	fLock.Acquire();
	fTimers.Remove(t);
	Reschedule();
	fLock.Release();
}

status_t TimerRoster::ThreadEntry()
{
	bool running = true;
	while (running) {
		bigtime_t waitUntil = B_INFINITE_TIMEOUT;
		if (fLock.Acquire() >= B_OK) {
			if (!fTimers.IsEmpty())
				waitUntil = fTimers.First()->fTime;
			fScheduledTime = waitUntil;
			fLock.Release();
		}
		status_t err = fLooperControl.Acquire(B_ABSOLUTE_TIMEOUT, waitUntil);
		if (err == B_TIMED_OUT) {
			while (fLock.Acquire() >= B_OK && fRun && !fTimers.IsEmpty() && system_time() - fTimers.First()->fTime >= 0) {
				Timer *event = fTimers.RemoveHead();
				fLock.Release();
				event->Do();
			}
			if (!fRun) running = false;
			if (fLock.Holder() >= B_OK) fLock.Release();
		}
	}
	return B_OK;
}

void TimerRoster::Reschedule()
{
	if (!fTimers.IsEmpty() && fTimers.First()->fTime < fScheduledTime)
		fLooperControl.Release();
}

/*
Bugs:
1. Freeze on exit
2. Handler sometimes not called
*/
