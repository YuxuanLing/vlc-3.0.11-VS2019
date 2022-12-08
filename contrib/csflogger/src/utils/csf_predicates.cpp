#include <iostream>

#include "csf/utils/csf_predicates.h"
#include "csf/utils/Thread.hpp"

	/*
	 * Implementation of the time predicate
	 */
	TimedPredicate::TimedPredicate(	Predicate* predicate,
									unsigned int timeLimitMillis, 
									unsigned int retryPeriodMillis) :
    predicate(predicate),
    timeLimitMillis(timeLimitMillis),
    retryPeriodMillis(retryPeriodMillis)
	{
	// Check if the time limit is more than 5 minutes, and if so print a Warning.
	  if(timeLimitMillis > (5 * 60 * 1000)) {
		  std::cout << "WARNING: Test will wait for longer than 5 minutes." << std::endl;
	  }
	}

	/*
	 * Destructor
	 */
	TimedPredicate::~TimedPredicate() { }


	/**
	 * Evaluate the provided predicate until the time limit occurs
	 */
	bool TimedPredicate::evaluate() const 
	{
		// Keep time sloppy for now, only counting the sleep periods
		unsigned int remainingTimeMillis = timeLimitMillis;

		for (;;) {
			// Check if predicate match
			if (predicate->evaluate()) {
				return true;
			}
			else{
				// Record the time passage
				remainingTimeMillis -= retryPeriodMillis;

				// Break from loop if time has run out
				if (remainingTimeMillis <= 0) {
					break;
				}
				else{
					// Sleep and process messages until a new evaluation is required
					//::sleep(retryPeriodMillis/1000);
					csf::csflogger::Thread::sleep(retryPeriodMillis);
				}	
			}
		}
    // Time ran out, so force return to false.
	std::cout << "Time ran out => returning false\n";
    return false;
  }

