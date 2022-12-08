#include "csf/utils/ThreadPool.hpp"
#include "csf/utils/Condition.hpp"
#include "csf/utils/ScopedLock.hpp"
#include "csf/utils/Runnable.hpp"
#include "csf/utils/Event.hpp"
#include "csf/utils/BlockingTask.hpp"

#ifdef _WIN32
#include <process.h>
#include <windows.h>
#else
#include <pthread.h>
#endif


namespace csf
{

namespace csflogger
{
	// Pool Threads are the runnables that execute in the thread pool
	class PoolWorker: public Runnable
	{
	public:
		PoolWorker(ThreadPool* parent, TaskQueuePtr taskQueue, TaskQueuePtr urgentTaskQueue,
				   std::shared_ptr<Mutex> taskMutex, long expireTime, const std::string & name = "");
		void run();
		void wakeUpThread();
		void shutdown();
		bool hasCapacity();
		void addThread();
	private:
		bool stayAlive(bool &timedOut, bool &finalThread);
        void createThread(Runnable* runnable);
        
		ThreadPool* _parentPool;
		const std::string _name;
		TaskQueuePtr _taskQueue;
		TaskQueuePtr _urgentTaskQueue;
		std::shared_ptr<Mutex> _taskMutex;
		Condition _taskWaiting;
		long _timeout;
		Mutex _threadCountMutex;
		unsigned int _threadCount, _activeThreads;
	};

	PoolWorker::PoolWorker(ThreadPool* parent, TaskQueuePtr taskQueue,
						   TaskQueuePtr urgentTaskQueue, std::shared_ptr<Mutex> taskMutex, long expireTime, const std::string & name) :
												   _parentPool(parent), _name(name), _taskQueue(taskQueue),
												   _urgentTaskQueue(urgentTaskQueue), _taskMutex(taskMutex),
												   _timeout(expireTime), _threadCount(0u), _activeThreads(0u) {}

	void PoolWorker::wakeUpThread()
	{
		_taskWaiting.signal();
	}

	void PoolWorker::shutdown()
	{
		// Speculative fix for CSCuz67771
		// shutdown() has already been called
		if(_parentPool == NULL)
		{
			return;
		}

		// Empty the task queue
		_taskMutex->lock();
		_taskQueue->clear();
		_urgentTaskQueue->clear();
		_taskMutex->unlock();

		// Remove reference to parent pool
		_threadCountMutex.lock();
		_parentPool = NULL;
		
		if (_threadCount > 0u)
		{
			// Wake up all sleeping threads
			_taskWaiting.broadcast();
			_threadCountMutex.unlock();
		}
		else
		{
			_threadCountMutex.unlock();
			// No threads in pool so we delete the PoolWorker
			delete this;
		}
	}

	bool PoolWorker::hasCapacity()
	{
		// This function returns true if there are idle threads available
		ScopedLock threadCountLock(_threadCountMutex);
		return (_activeThreads < _threadCount);
	}

	void PoolWorker::addThread()
	{
		ScopedLock threadCountLock(_threadCountMutex);
        
        if (_parentPool && _threadCount < _parentPool->_maxThreads)
        {
            createThread(this);
            _threadCount++;
        }
	}

	bool PoolWorker::stayAlive(bool &timedOut, bool& finalThread)
	{
		// This function checks whether the thread should stay alive.
		// The function assumes the thread will terminate if a false is returned.
		ScopedLock threadCountLock(_threadCountMutex);

		if (_parentPool == NULL)
		{
			_threadCount--;
			if (_threadCount == 0u)
			{
				finalThread = true;
			}
			return false;
		}
		else if (timedOut && _threadCount > _parentPool->_minThreads)
		{
			_threadCount--;
			return false;
		}
		else
		{
            timedOut = false;
			return true;
		}
	}
	
#ifdef _WIN32
    const DWORD MS_VC_EXCEPTION = 0x406D1388;
    #pragma pack(push,8)
    typedef struct tagTHREADNAME_INFO
    {
        DWORD dwType; // Must be 0x1000.
        LPCSTR szName; // Pointer to name (in user addr space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags; // Reserved for future use, must be zero.
     } THREADNAME_INFO;
    #pragma pack(pop)

    void SetThreadName(DWORD dwThreadID, const char * name) {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name;
        info.dwThreadID = dwThreadID;
        info.dwFlags = 0;
    #pragma warning(push)
    #pragma warning(disable: 6320 6322)
        __try{
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
        }
        __except (EXCEPTION_EXECUTE_HANDLER){
        }
    #pragma warning(pop)
    }
#else
    void SetThreadName(const std::string & name)
    {
    	std::string lenghtLimitationCompliantName = name;
    	if(name.length() > 15)
    	{
    		lenghtLimitationCompliantName = name.substr(0,14);
    	}
#ifdef __ANDROID__
    	pthread_t thisThreadInfo =  pthread_self();
    	int result = pthread_setname_np(thisThreadInfo, lenghtLimitationCompliantName.c_str());
#elif !defined(__linux__)
    	int result = pthread_setname_np(lenghtLimitationCompliantName.c_str());
#else
    	//pthread_setname_np not available on linux, we don't need thread naming on linux anyway
    	int result = 0;
#endif
		if(result != 0)
		{
			fprintf (stderr, "SetThreadName failed with error code %i\n", result);
		}
    }
#endif

	void SetCurrentThreadName(const std::string & name)
	{
		std::string actualName = name.empty() ? "PoolThread" : name;
#ifdef _WIN32
		SetThreadName(::GetCurrentThreadId(), actualName.c_str());
#else
		SetThreadName(actualName);
#endif
	}

	void PoolWorker::run()
	{
		TaskPtr task;
		bool finalThread = false;
		bool timedOut = false;
		SetCurrentThreadName(_name);
		while (stayAlive(timedOut, finalThread))
		{
			// Wait for tasks to appear in the queue
			_taskMutex->lock();
			while (_parentPool && !timedOut && _urgentTaskQueue->empty() && _taskQueue->empty())
			{
				timedOut = (_taskWaiting.timedWait(*_taskMutex, _timeout) != 0);
			}

			// Process tasks in the queue
			while (_parentPool && !timedOut && (!_urgentTaskQueue->empty() || !_taskQueue->empty()))
			{
				if(!_urgentTaskQueue->empty())
				{
					task = _urgentTaskQueue->front();
					_urgentTaskQueue->pop_front();
				}
				else
				{
					task = _taskQueue->front();
					_taskQueue->pop_front();
				}
				_taskMutex->unlock();

				_threadCountMutex.lock();
				_activeThreads++;
				_threadCountMutex.unlock();

				SetCurrentThreadName(task->name());
				// Execute the task
				task->execute();
				task.reset();
				
				_threadCountMutex.lock();
				_activeThreads--;
				_threadCountMutex.unlock();

				_taskMutex->lock();
			}
			_taskMutex->unlock();
			SetCurrentThreadName(_name);
		}

		/* There should be no access of member variables past this point as the PoolWorker object may be deleted by another thread */

		// Parent ThreadPool is destroyed and this is the last pool thread so delete PoolWorker object.
		if (finalThread)
		{
			delete this;
		}
		
		// Thread is terminating
	}
    
    #ifdef _WIN32
    void poolThreadMain(void* arg)
    {
        Runnable* runnable = (Runnable*) arg;
        runnable->run();
    }
    #else
    void* poolThreadMain(void* arg)
    {
        Runnable* runnable = (Runnable*) arg;
        runnable->run();
        return 0;
    }
    #endif
    
    void PoolWorker::createThread(Runnable* runnable)
	{
    #ifdef _WIN32
        _beginthread(&poolThreadMain, 0, runnable);
    #else
        pthread_t threadId;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        if ( pthread_create(&threadId, &attr, poolThreadMain, runnable) != 0 )
        {
            // System error occurred creating thread
        }
    #endif
    }
    
	ThreadPool::ThreadPool(unsigned int minThreadCount, unsigned int maxThreadCount, unsigned int expiryTimeSec)
		: _taskQueuePtr(new TaskQueue()), _urgentTaskQueuePtr(new TaskQueue()), _taskMutex(new Mutex()),
		  _minThreads(minThreadCount), _maxThreads(maxThreadCount)
	{
		init(expiryTimeSec, "");
	}

	ThreadPool::ThreadPool(unsigned int minThreadCount, unsigned int maxThreadCount, const std::string & name, unsigned int expiryTimeSec)
			: _taskQueuePtr(new TaskQueue()), _urgentTaskQueuePtr(new TaskQueue()), _taskMutex(new Mutex()),
			  _minThreads(minThreadCount), _maxThreads(maxThreadCount)
	{
		init(expiryTimeSec, name);
	}

	void ThreadPool::init(unsigned int expiryTimeSec, const std::string & name)
	{
		_threadWorker = new PoolWorker(this, _taskQueuePtr, _urgentTaskQueuePtr, _taskMutex, expiryTimeSec*1000, name);
		// Initialise the minimum thread count
		for (size_t i = 0; i < _minThreads; i++)
		{
			_threadWorker->addThread();
		}
	}

	ThreadPool::~ThreadPool()
	{
		_threadWorker->shutdown();
	}

	void ThreadPool::enqueue(TaskPtr task, bool blocking, bool urgent)
	{
		BlockingTaskPtr blockingTask;

		// Add task to queue
		if (blocking)
		{
			blockingTask.reset(new BlockingTask(task));
			_taskMutex->lock();
			urgent ? _urgentTaskQueuePtr->push_back(blockingTask) : _taskQueuePtr->push_back(blockingTask);
			_taskMutex->unlock();
		}
		else
		{
			_taskMutex->lock();
			urgent ? _urgentTaskQueuePtr->push_back(task) : _taskQueuePtr->push_back(task);
			_taskMutex->unlock();
		}

		// Wake up an idle thread or create a new one
		if (_threadWorker->hasCapacity())
		{
			_threadWorker->wakeUpThread();
		}
		else
		{
			// Create a new thread to the pool is maxThreads has not been reached
            _threadWorker->addThread();
		}

		// If this is a blocking call - wait for task to complete
		if (blocking)
		{
			blockingTask->taskComplete.wait();
		}
	}

	void ThreadPool::clearQueue()
	{
		_taskMutex->lock();
		_taskQueuePtr->clear();
		_urgentTaskQueuePtr->clear();
		_taskMutex->unlock();
	}

	unsigned int ThreadPool::queueSize() const
	{
		_taskMutex->lock();
		unsigned int queueSize = _taskQueuePtr->size();
		unsigned int urgentQueueSize = _urgentTaskQueuePtr->size();
		_taskMutex->unlock();
		return queueSize + urgentQueueSize;
	}
}

}
