#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "Mutex.hpp"
#include "../logger/SmartPointer.hpp"
#include "Task.hpp"
#include <deque>
#include <string>

/*
 * Created on: May 2012
 * Author: Barry Kearns (bakearns@cisco.com)
 */
namespace csf
{

namespace csflogger
{
	class PoolWorker;
	class Runnable;

	typedef std::deque<TaskPtr> TaskQueue;
	typedef std::shared_ptr<TaskQueue> TaskQueuePtr;

    /**
     * @brief A cross platform ThreadPool. 
     *
     * Allows tasks derived from csf/Task to be scheduled for execution. 
     * 
     * Once created, Tasks can be added using the enqueue method. 
     * Tasks are then queued and executed in a FIFO queue. However, if the 
     * parameter 'urgent' is true, the task is enqueued after the topmost urgent task
     * in the queue. 
     *
     * The ThreadPool is created with minThreadCount threads. During execution,
     * the number of threads can increase up to maxThreadCount. If a thread is 
     * 'idle' for a period of 'expireTimeSec', and the number of active 
     * threads exceeds minThreadCount, the thread will expire. 
     *
     * Tasks must be passed as a smart pointer. The ThreadPool can therefore 
     * guarantee the Task exists until after they are executed. Once executed, 
     * ThreadPool's copy of the Task Smart Pointer is released. If there is no
     * other Smart Pointer to the Task, the Task is deleted after execution. 
     * 
     * The ThreadPool can be deleted at any point. 
     *
     * Once deleted, all currently executing tasks will complete. No Tasks still
     * in the queue will be executed, and the ThreadPool will release its Smart 
     * Pointer reference. Although ThreadPool is then deleted, it is implemented 
     * by an internal PoolWorker class; PoolWorker is not deleted until the last 
     * thread completes. The Smart Pointer reference to any Tasks is released
     * when the Task executes or when it is removed from the queue when
     * ThreadPool is deleted. 
     *
     * The Threads are run in a 'detached' manner. Therefore, there is no need
     * to 'join' the threads after they complete. The Threads are posix threads,
     * except on Windows. 
     *
     * A Task can be enqueued with 'blocking' equal to True. In this case, enqueue
     * will not return until the enqueued Task is completed.
     *
     * Clearly, while ThreadPool manages memory of Tasks and itself, it cannot be 
     * responsible for any other memory. Therefore, care should be taken not to 
     * delete memory a Task might reference before that Task executes. 
     *
     */
	class ThreadPool
	{
	public:
	/**
	 * @brief Constructor.
	 * @param [in] minThreadCount The minimum number of threads that will be maintained in the thread pool
     *              at all times.
	 * @param [in] maxThreadCount The maximum number of threads that can be created by the thread pool
 	 * @param [in] expiryTimeSec The number of second that a thread will remain idle in the thread pool 
     *              before terminating if there are more than minThreadCount threads in use.
	 */
		ThreadPool(unsigned int minThreadCount, unsigned int maxThreadCount, unsigned int expiryTimeSec = 60);

	/**
	 * @brief Constructor.
	 * @param [in] minThreadCount The minimum number of threads that will be maintained in the thread pool
	 *              at all times.
	 * @param [in] maxThreadCount The maximum number of threads that can be created by the thread pool
	 * @param [in] name The name to be assigned to the threads in this thread pool
	 * @param [in] expiryTimeSec The number of second that a thread will remain idle in the thread pool
	 *              before terminating if there are more than minThreadCount threads in use.
	 */
		ThreadPool(unsigned int minThreadCount, unsigned int maxThreadCount, const std::string & name, unsigned int expiryTimeSec = 60);
		~ThreadPool();

	/**
	 * @brief Enqueue a new task into the work queue to be processed by the thread pool
	 * @param[in] task The task to be executed
	 * @param[in] blocking Enqueue will block until Task is executed.
	 * @param[in] enqueueAtFront Task is added to the front of the queue instead of the end
	 */
		void enqueue(std::shared_ptr<Task> task, bool blocking = false, bool urgent = false);

	/**
	 * @brief Clear all the tasks in the work queue
	 */
		void clearQueue();

	/**
	 * @brief Return the size of the work queue
	 * @param [out] work queue size
	 */
		unsigned int queueSize() const;

	private:
		PoolWorker* _threadWorker;
		TaskQueuePtr _taskQueuePtr;
		TaskQueuePtr _urgentTaskQueuePtr;
		std::shared_ptr<Mutex> _taskMutex;
		unsigned int _minThreads, _maxThreads;

		void init(unsigned int expiryTimeSec, const std::string & name);

		friend class PoolWorker;
	};
}

}

#endif
