
#ifndef CSF_THREAD_PRIVDATA_H_
#define CSF_THREAD_PRIVDATA_H_

namespace csf {

namespace csflogger {

/*
 * Internal structure to store the tread relative data.
 */
struct thread_privdata {
        std::string name; 			// Name of the thread.
        unsigned int stackSize;		// Size allocated to the thread heap.
        Runnable  *runnable;		// Pointer to the Runnable interface to use.
#ifdef _WIN32
        HANDLE hThread;
        unsigned threadId;
#else
        pthread_t threadId;			// Id of the thread.
#endif
        bool running;				// Is the thread running.
        Mutex threadDataMutex;		// Mutex to this structure
};

}

}

#endif /* CSF_THREAD_PRIVDATA_H_ */

