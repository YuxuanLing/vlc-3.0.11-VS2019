
/* $Id: Thread.cpp 166394 2014-02-06 10:08:32Z alcronin $ */

#include <windows.h>
#include <process.h>
#include <stdio.h>

#include "csf/utils/Thread.hpp"
#include "thread_privdata.hpp"

namespace csf {

namespace csflogger {

unsigned int __stdcall threadMain(void *arg)
{
    thread_privdata *data = (thread_privdata *) arg;
    data->runnable->run();

    data->running = false;

    if (data->threadId == ::GetCurrentThreadId())
    {
    }

    return 0;
}

Thread::Thread()
{
    initialize("Unknown name");
}

Thread::Thread(const std::string & name)
{
    initialize(name);
}

Thread::~Thread()
{
	delete threaddata;
    //printf("~Thread()\n");
    // TODO: want to detach here, but is there any such concept on Windows?
}

int Thread::initialize(const std::string & name)
{
	/* XXX No error handling */
	threaddata = new struct thread_privdata;

    threaddata->name = name;
    threaddata->stackSize = DEFAULT_STACK_SIZE;
    threaddata->running = false;
	threaddata->hThread = NULL;

    return 0;
}

int Thread::setStackSize(int size)
{
    // Illegal to call when thread is already running
    if (threaddata->running)
    {
        return 1;
    }
    threaddata->stackSize = size;
    return 0;
}

int Thread::getStackSize() const 
{
    return threaddata->stackSize;
}

int Thread::start(Runnable & runnable)
{
	// The return value is going to be ERROR_SUCCESS (0) on success. If there was a problem
	// during the execution of this method then the error placed on the stack will be used as
	// the return value. This value is gotten from GetLastError(). The error returned is from
	// the last operation that failed.
	int returnValue = ERROR_SUCCESS;

    threaddata->threadDataMutex.lock();

	// Clear the last error code in thread local storage. This ensures the error (if any) is
	// for the thread creation instead of a previous call.
	SetLastError(returnValue);

    threaddata->runnable = &runnable;

    // MSDN documentation for _beginthreadex() says:
    // "the stack_size argument can be 0, in which case the operating system
    //  uses the same value as the stack specified for the main thread"
    threaddata->hThread = (HANDLE) _beginthreadex(
        NULL, // default security
        threaddata->stackSize,
        &threadMain,
        threaddata,
        0, // run immediately
        &threaddata->threadId
        );

    if(threaddata->hThread == NULL)
    {
		// There was an issue, get the error placed on the stack from _beginthreadex.
		returnValue = GetLastError();
        printf("Error creating thread: %d.\n", returnValue);
    }

	// Check the error code recieved from GetLastError(), not the thread handle.
	// The thread in certain cases can exit quickly and the handle can be set to NULL.
	if(returnValue == ERROR_SUCCESS)
	{
		threaddata->running = true;
	}

    threaddata->threadDataMutex.unlock();

    return returnValue;
}

std::string Thread::getName() const
{
    return threaddata->name;
}

std::string Thread::name() const
{
    return getName();
}

void Thread::setName(const std::string & name)
{
    threaddata->name = name;
}

int Thread::join()
{
    //printf("Thread.join()\n");
    int ret = ::WaitForSingleObject(threaddata->hThread, INFINITE);
    if (ret != WAIT_OBJECT_0)
    {
        printf("Error waiting for thread: %d\n", ::GetLastError());
    }

	if(threaddata->hThread != NULL)
	{
		ret = ::CloseHandle(threaddata->hThread);
		threaddata->hThread = NULL;
	}

    if (ret == 0) {
        printf("Error closing thread handle\n");
        return 1;
    } else {
        return 0;
    }
}

bool Thread::isRunning() const
{
    return threaddata->running;
}

void
Thread::sleep(unsigned long milliseconds)
{
    Sleep((DWORD) milliseconds);
}

}

} // namespace csf
