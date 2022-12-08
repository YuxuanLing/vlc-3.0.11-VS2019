#ifndef BLOCKING_TASK_H
#define BLOCKING_TASK_H

#include "Task.hpp"
#include "Event.hpp"

namespace csf
{

namespace csflogger {

/**
 * A class derived from Task which allow a thread to block until another thread has completed execution of the task.
 * The Event 'taskComplete' is exposed publicly so that the blocking thread can call "myTask->taskComplete.wait();"
 * in order to block until another thread completes the task.
 */
 
	class BlockingTask : public Task
	{
	public:
		BlockingTask(TaskPtr task) : _task(task) {};
		void execute() { _task->execute(); taskComplete.set(); }
		Event taskComplete;
	
	private:
		TaskPtr _task;
	};

	typedef SMART_PTR_NS::shared_ptr<csf::csflogger::BlockingTask> BlockingTaskPtr;
}
}
#endif // BLOCKING_TASK_H
