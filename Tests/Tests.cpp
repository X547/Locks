#include <stdio.h>
#include <signal.h>

#include <Application.h>
#include <Window.h>
#include <Button.h>
#include <LayoutBuilder.h>

#include <OS.h>

#include "Locks/RecursiveLock.h"
#include "Locks/ConditionVariable.h"

enum {
	releaseMsg = 1,
};


RecursiveLock gPrintLock;


class Window: public BWindow
{
private:
	BButton *fBtn;
	thread_id fThread;

	RecursiveLock fLock;
	ConditionVariable fCondVar;
	bool fRun;

public:
	Window(): BWindow(
		BRect(), "Locks", B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
		B_ASYNCHRONOUS_CONTROLS | B_AUTO_UPDATE_SIZE_LIMITS
	)
	{
		fRun = true;
		fThread = spawn_thread(ThreadEntry, "thread", B_NORMAL_PRIORITY, this);
		resume_thread(fThread);

		struct sigaction oldAction, action;
		sigemptyset(&action.sa_mask);
		action.sa_handler = (__sighandler_t)SignalHandler;
		action.sa_flags = SA_SIGINFO;
		action.sa_userdata = NULL;
		sigaction(SIGUSR1, &action, &oldAction);

		BLayoutBuilder::Group<>(this, B_VERTICAL, B_USE_SMALL_SPACING)
			.SetInsets(B_USE_SMALL_SPACING)
			.Add(fBtn = new BButton("btn", "Release", new BMessage(releaseMsg)))
			.End()
		;
		fBtn->MakeFocus(true);
	}

	static void SignalHandler(int signal, siginfo_t *signalInfo, ucontext_t *ctx)
	{
		printf("SignalHandler\n");
	}

	static status_t ThreadEntry(void *arg)
	{
		Window &w = *(Window*)arg;
		gPrintLock.Acquire(); printf("+ThreadEntry\n"); gPrintLock.Release();
		w.fLock.Acquire();
		while (w.fRun) {
			w.fCondVar.Acquire(w.fLock, B_ABSOLUTE_TIMEOUT, system_time() + 1000000);
			gPrintLock.Acquire(); printf("ConditionVariable released\n"); gPrintLock.Release();
		}
		w.fLock.Release();
		gPrintLock.Acquire(); printf("-ThreadEntry\n"); gPrintLock.Release();
		return B_OK;
	}

	bool QuitRequested()
	{
		fLock.Acquire();
		fRun = false;
		fCondVar.Release(true);
		fLock.Release();
		gPrintLock.Acquire(); printf("Quit\n"); gPrintLock.Release();
		return BWindow::QuitRequested();
	}

	void MessageReceived(BMessage *msg)
	{
		switch (msg->what) {
		case releaseMsg:
			fLock.Acquire();
			gPrintLock.Acquire(); printf("releaseMsg\n"); gPrintLock.Release();
			fCondVar.Release();
			fLock.Release();
			return;
		}
		BWindow::MessageReceived(msg);
	}
	
};

int main()
{
	BApplication app("application/x-vnd.Test-App");

	Window *wnd = new Window();
	wnd->SetFlags(wnd->Flags() | B_QUIT_ON_WINDOW_CLOSE);
	wnd->CenterOnScreen();
	wnd->Show();

	app.Run();
	return 0;
}
