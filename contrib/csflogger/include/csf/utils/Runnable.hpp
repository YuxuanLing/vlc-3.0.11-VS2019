/*
 * Created on: Sep 7, 2010
 *     Author: Frank Sauvage (frsauvage@cisco.com)
 *
 * $Id: Runnable.hpp 42447 2011-04-28 11:47:07Z phbernar $
 */

#ifndef CSF_RUNNABLE_INCLUDED
#define CSF_RUNNABLE_INCLUDED

namespace csf {

namespace csflogger {

/**
 * @brief A runnable object.
 *
 * The Runnable class defines the interface that
 * needs to be implemented by object which requires to be
 * executed in a separate thread.
 */
class Runnable {

public:
	Runnable();
        virtual ~Runnable();
	
	/**
	 * The run method needs to be implemented by any object that
	 * aims to be run in a thread.
	 * When creating and starting the thread, this method will be called.
	 * The thread will end it's execution when this method returns.
	 */
	virtual void run()=0;

}; // End of Runnable class 

}

} // namespace csf

#endif // CSF_RUNNABLE_INCLUDED
