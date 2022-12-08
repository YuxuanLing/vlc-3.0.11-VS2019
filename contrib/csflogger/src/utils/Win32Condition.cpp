/*
 * Condition.cpp
 * This avoids the Windows API ConditionVariable, which is not supported on Windows XP.
 * $Id: Condition.cpp 44060 2012-09-11 13:24:35Z janoonan $
 */

#include <windows.h>
#include <climits> // When ole is used over ole2, windows.h does not include limits.h
#include <stdio.h>

#include "csf/utils/Condition.hpp"

namespace csf {

namespace csflogger {

struct cond_privdata
{
	HANDLE semaphore;
	CRITICAL_SECTION csection;
	unsigned long num_waiting;
	unsigned long num_wake;
	unsigned long generation;
};

Condition::Condition()
{
	/* XXX No error handling */
	cond = new struct cond_privdata;
	::memset(cond, 0, sizeof (struct cond_privdata));

	cond->semaphore = ::CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	if (cond->semaphore == NULL) {
		//printf("Condition() cond->semaphore = %x\n", cond->semaphore);
	}

	::InitializeCriticalSection(&cond->csection);
}

Condition::~Condition()
{
	//printf("~Condition()\n");
	::CloseHandle(cond->semaphore);
	::DeleteCriticalSection(&cond->csection);
	delete cond;
}

int Condition::signal()
{
	unsigned int wake = 0;

	::EnterCriticalSection(&cond->csection);
    //printf("signal() cond->num_waiting = %d, cond->num_wake = %d\n", cond->num_waiting, cond->num_wake);
	if (cond->num_waiting > cond->num_wake) {
		wake = 1;
		cond->num_wake++;
		cond->generation++;
	}
	::LeaveCriticalSection(&cond->csection);

	if (wake) {
		::ReleaseSemaphore(cond->semaphore, 1, NULL);
	}

	return 0;
}

int Condition::broadcast()
{
	unsigned long num_wake = 0;

	::EnterCriticalSection(&cond->csection);
	if (cond->num_waiting > cond->num_wake) {
		num_wake = cond->num_waiting - cond->num_wake;
		cond->num_wake = cond->num_waiting;
		cond->generation++;
	}
	::LeaveCriticalSection(&cond->csection);

	if (num_wake) {
		::ReleaseSemaphore(cond->semaphore, num_wake, NULL);
	}

	return 0;
}

int Condition::wait(Mutex & mutex)
{
	return timedWait(mutex, LONG_MAX);  // 'infinite'
}

int Condition::timedWait(Mutex & mutex, long timeout_ms)
{
	//printf("timedWait() start, timeout_ms = %dl\n", timeout_ms);
	int rv;
	unsigned int wake = 0;
	unsigned long generation;

	::EnterCriticalSection(&cond->csection);
	cond->num_waiting++;
	generation = cond->generation;
	::LeaveCriticalSection(&cond->csection);

	mutex.unlock();

	do {
		DWORD res = ::WaitForSingleObject(cond->semaphore, timeout_ms);
		// res: 258 WAIT_TIMEOUT  if the interval elapsed and the state of the object is non-signaled
		//        0 WAIT_OBJECT_0 if the state of the object is signaled
	    //printf("timedWait() after wait, res = %d\n", res);

		::EnterCriticalSection(&cond->csection);

		if (cond->num_wake) {
			//printf("timedWait() cond->num_wake = %d\n", cond->num_wake);
			if (cond->generation != generation) {
			    //printf("timedWait() cond->generation = %d, generation = %d\n", cond->generation, generation);
				cond->num_wake--;
				cond->num_waiting--;
				rv = 0; // success
				break;
			} else {
				wake = 1;
			}
		}
		else if (res != WAIT_OBJECT_0) {
			cond->num_waiting--;
			rv = 1; // timeout
			break;
		}

		::LeaveCriticalSection(&cond->csection);

		if (wake) {
			wake = 0;
			::ReleaseSemaphore(cond->semaphore, 1, NULL);
		}

	} while (1);

	::LeaveCriticalSection(&cond->csection);
	mutex.lock();

	return rv;
}

}

} // namespace csf
