#ifndef TASK_CSF_H
#define TASK_CSF_H


#include "../logger/SmartPointer.hpp"
#include <sstream>
#include <string>
#include <typeinfo>

namespace csf
{

namespace csflogger {
	/**
     * \class Task
     *
     * @brief Base class that is executed by ThreadPool.
     * 
     * To use, derive a class from this (e.g
     *  class MyTask : public csf::Task 
     *  ). 
     *  
     * Tasks should be created as Smart Pointers
     * e.g.  TaskPtr t(new MyTask());
     *
     */
    class Task
	{
	public:

        /**
         * \method execute
         * 
         * @brief Executed by thread pool.
         */
        virtual void execute() = 0;

		virtual ~Task() {};

        /**
         * \method name
         *
         * @brief Returns a string representation of the class
         */
		virtual std::string name() const
		{
			std::stringstream n;
			n << typeid(*this).name() << " [" << this << "]"; 
			return n.str();
		}

	};
	
	typedef SMART_PTR_NS::shared_ptr<csf::csflogger::Task> TaskPtr;
}
}
#endif // TASK_CSF_H
