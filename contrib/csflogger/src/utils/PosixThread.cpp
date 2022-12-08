/*
 * Created on: Sep 7, 2010
 *     Author: Frank Sauvage (frsauvage@cisco.com)
 *
 * $Id: Thread.cpp 166394 2014-02-06 10:08:32Z alcronin $
 */

#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "csf/utils/Thread.hpp"
#include "thread_privdata.hpp"

namespace csf {

namespace csflogger {

/*
 * the threadMain method is used to be start by the thread.
 * It is responsible to call the runnable->run() method, and when the method returns,
 * performs the required cleaning of the thread.
 */
void* threadMain(void *);
void* threadMain(void *arg){
	// Retrieving data
    struct thread_privdata *data = (struct thread_privdata *)arg;
    // Call the run method from the runnable object
    data->runnable->run();

    // Stack-local copy for use below in this function
    const pthread_t threadId = data->threadId;

    // the thread is now complete. lets change the flag.
    data->running = false;

    // When the thread finish the execution of the runnable, it need to be destroyed
    // No need to check result as the thread destroy itself
    if (pthread_equal(threadId, pthread_self())) {
    	pthread_exit(NULL);
    }

    return 0;
}


/*
 * Constructors
 */
Thread::Thread(){
	initialize("Unknown name");
}

Thread::Thread(const std::string& name){
	initialize(name);
}

int Thread::initialize(const std::string& name){
	/* XXX No error handling */
	threaddata = new struct thread_privdata;

	threaddata->name = name;
	threaddata->stackSize = DEFAULT_STACK_SIZE;
	threaddata->running = false;
	return 0;
}


/*
 * Destructor
 */
Thread::~Thread(){
    // Detach the Posix thread if it is still running
    if (isRunning()) {
        pthread_detach(threaddata->threadId);
    }

    // Fix Rally defect DE35109.
    // Clean up the threaddata control block.
    // 
    // If the control block does not show that this Thread object is running,
    // then either Thread::start() was not called, or the new execution thread
    // has finished running threadMain().  Either way, delete the control
    // block now because the execution thread will not use it again.
    if (!isRunning()) {
    	delete threaddata;
    }
    // If the execution thread is still running, leak the control block.
    // Fixing this less common leak is left for another time.
    // One approach: add a new flag to the control block, and here in the
    // destructor, set it to tell the execution thread 'please release me'.
    else {
        // empty
    }
}

/*
 * Start the thread using the provided object that implement the runnable interface.
 */
int Thread::start(Runnable& runnable)
{
	// The return value is from the last operation that failed. On success it will be 0.
	// Unlike the Windows implementation the error will be retrieved from the method
	// directly.
	int returnValue = 0;

	// Lock the thread data structure
	threaddata->threadDataMutex.lock();
	// Store the runnable to be passed to the thread
	threaddata->runnable = &runnable;

	// Initialise the pthread attributes structure. This will determine any options set
	// on the new thread itself.
	pthread_attr_t attr;
	int attrInitError = pthread_attr_init(&attr);
	if(attrInitError != 0)
	{
		printf("Failed to init attribute for thread %s: %d\n", threaddata->name.c_str(), attrInitError);
		returnValue = attrInitError;
	}

	// Only set the stack size if the initalisation completed and the new stack size is greater than 0.
	if(returnValue == 0 && threaddata->stackSize > 0)
	{
		int attrStackSizeError = pthread_attr_setstacksize(&attr, threaddata->stackSize);
		if(attrStackSizeError != 0)
		{
			printf("Invalid stacksize %d specified for thread %s: %d\n",
	                    threaddata->stackSize, threaddata->name.c_str(), attrStackSizeError);
			returnValue = attrStackSizeError;
		}
	}

	// Only if the initialisation completed successfully do we create the thread.
	if(attrInitError == 0)
	{
		// Now create the thread itself.
		void *data = threaddata;
		int threadCreationError = pthread_create(&threaddata->threadId, &attr, threadMain, data);
		if(threadCreationError != 0)
		{
			printf("Creation of thread %s failed: %d\n", threaddata->name.c_str(), threadCreationError);
			returnValue = threadCreationError;
		}
		else
		{
			// Now the thread is running set the flag.
			threaddata->running = true;
		}
	}

	// Unlock the thread data structure.
	threaddata->threadDataMutex.unlock();

	return returnValue;
}

/*
 * Thread Naming
 */
std::string Thread::name() const{
	return std::string(threaddata->name.c_str());
}

std::string Thread::getName() const {
	return std::string(threaddata->name.c_str());
}

void Thread::setName(const std::string& name){
//	threaddata->threadDataMutex.lock();
	threaddata->name = name;
//	threaddata->threadDataMutex.unlock();
}

/*
 * Stack size
 */
int Thread::setStackSize(int size){
//	threaddata.threadDataMutex.lock();
	// Check the thread is not yet running
	if(threaddata->running){
		//threaddata->threadDataMutex.unlock();
		return 1; // TODO Error codes
	}
	// Now change the stack size
	threaddata->stackSize = size;
//	threaddata->threadDataMutex.lock();
	return 0;
}

int Thread::getStackSize() const{
	return threaddata->stackSize;
}

/*
 * Join method
 */
int Thread::join(){

	unsigned int result = pthread_join(threaddata->threadId, NULL);
	if(result == 0){
		return 0; // OK
	}
	else {
		return 1;
		// TODO Error codes from pthreads:
		// ESRCH: no undetached thread found to the thread id.
		// EDEADLK: deadlock detected.
	}
}

/*
 * State
 */
bool Thread::isRunning() const{
	return threaddata->running;
}

/*
 * Static utilities
 */
void Thread::sleep(unsigned long milliseconds){
	struct timespec timeout0;
	struct timespec timeout1;
	struct timespec* tmp; // used to swap
	struct timespec* t0 = &timeout0;
	struct timespec* t1 = &timeout1;

	t0->tv_sec = milliseconds / 1000;
	t0->tv_nsec = (milliseconds % 1000) * (1000 * 1000);

	// Loop until the time is done.
	// nanosleep will return if a signal is catch, so keep doing it
	// until time is elapsed
	// TODO: a check to "&& (errno == EINTR)" should be added to iron the code.
	while ((nanosleep(t0, t1) == (-1)) ){
		tmp = t0;
		t0 = t1;
		t1 = tmp;
	}
}

}

} // namespace csf

