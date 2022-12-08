/*
 * Created on: Sep 7, 2010
 *     Author: Frank Sauvage (frsauvage@cisco.com)
 *
 * $Id: Thread.hpp 146253 2013-09-13 18:37:06Z kennward $
 */

#ifndef CSF_THREAD_INCLUDED
#define CSF_THREAD_INCLUDED

#include <string>
#include "Mutex.hpp"
#include "Condition.hpp"
#include "Runnable.hpp"

namespace csf {

namespace csflogger {

/**
 * @brief Default stack size allocated to thread.
 * Size 0 means to use the platform default size.
 */
#ifdef __APPLE__
  #if TARGET_OS_IPHONE
    // iOS and iOS simulator
    const unsigned int DEFAULT_STACK_SIZE = 0;
  #else
    const unsigned int DEFAULT_STACK_SIZE = 0;
  #endif
#elif _WIN32
    const unsigned int DEFAULT_STACK_SIZE = 0;
#elif __ANDROID__
    const unsigned int DEFAULT_STACK_SIZE = 0;
#else
  // August 2013:
  // This value 61440 is low, and unchanged since early in the CSF2G project.
  // Platforms can increase it, independently from the Apple settings above,
  // or let their platform choose the default size.
  const unsigned int DEFAULT_STACK_SIZE = 61440;
#endif

struct thread_privdata;

/**
 * @brief The Thread class represent a thread that can
 * be created and used by an application to run
 * specifics Tasks using the Runnable interfaces
 */
class Thread {

public: 

	/**
	 * @brief Default constructor.
	 * Initialize the thread with default values
	 */
	Thread();
	/**
	 * @brief Constructor.
	 * @param [in] name The name of the thread that will be used for debugging purpose.
	 */
	Thread(const std::string& name);
	/**
	 * @brief Default destructor.
	 */
	~Thread();
	
	/**
	 * @brief Returns the name of the thread.
	 * @return Name of the thread.
	 */
	std::string name() const;
	/**
	 * @brief Returns the name of the thread.
	 * @return Name of the thread.
	 */
	std::string getName() const;
	/**
	 * @brief Set the name of the thread.
	 * @param [in] name The name of the thread that will be used for debugging purpose.
	 */
	void setName(const std::string& name);

	/**
	 * @brief Sets the initial stack size allocated
	 * to the thread. This method can only be called when thread are not running
	 * @param [in] size Size of the stack.
	 * @return 0 if success or an error code.
	 */
	int setStackSize(int size);

	/**
	 * @brief Gets the initial stack size of the thread.
	 * @return Initial stack size of the thread.
	 */
	int getStackSize() const;

	/**
	 * @brief Creates and start the thread processing.
	 * The thread will execute the run() method of the object
	 * until it returns.
	 * When returning, the thread will be destroyed.
	 * @param [in] runnable Object that implements the runnable interface.
	 * @return 0 if success or an error code.
	 */
	int start(Runnable& runnable);

	/**
	 * @brief Join to this thread.
	 * Wait for this thread to complete. if the thread is already complete, return immediately
	 * @return 0 if success or an error code.
	 */
	int join();

	/**
	 * @brief Tells whether this Thread is running.
         * @return True if this Thread is running, false otherwise.
	 */
	bool isRunning() const;

	/**
	 * @brief Put the current thread in sleep for a specific time.
	 * @param[in] milliseconds number of millisecond to sleep.
	 */
	static void sleep(unsigned long milliseconds);

private:
        // no copy
        Thread(const Thread&);
        // no assign
        Thread& operator=(const Thread&);

	/**
	 * @brief internal method used by constructors to initialize the object.
	 */
	int initialize(const std::string& name);

	/* Internal data */
	struct thread_privdata *threaddata;
}; // End of Thread Class

}

} // namespace csf

#endif // CSF_THREAD_INCLUDED
