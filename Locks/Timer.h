#ifndef _TIMER_H_
#define _TIMER_H_

#ifndef ASSERT
#define ASSERT(cond) if (!(cond)) abort();
#endif

#include <stdlib.h>
#include <OS.h>
#include <private/kernel/util/DoublyLinkedList.h>
#include "RecursiveLock.h"
#include "Sem.h"
#include <AutoDeleterOS.h>

class Timer: public DoublyLinkedListLinkImpl<Timer> {
private:
	bigtime_t fTime;
	friend class TimerRoster;

public:
	Timer();
	virtual ~Timer();
	virtual void Do() = 0;
};

class TimerRoster {
private:
	RecursiveLock fLock;
	DoublyLinkedList<Timer> fTimers;
	thread_id fThread;
	Sem fLooperControl;
	bool fRun;
	bigtime_t fScheduledTime;

	status_t ThreadEntry();
	void Reschedule();

public:
	TimerRoster();
	~TimerRoster();

	void Schedule(Timer *t, bigtime_t time);
	void Cancel(Timer *t);
};

extern TimerRoster gTimerRoster;


template <typename CallbackFn>
class TimerTemplate final: public Timer {
private:
	CallbackFn fCallback;

public:
	TimerTemplate(const CallbackFn &callback): fCallback(callback) {}

	void Do() final {fCallback();}
};

#endif	// _TIMER_H_
