#ifndef CSF_PREDICATES_H
#define CSF_PREDICATES_H

/**
 * Base Predicate Class
 *
 * This is used to create a test that doesn't necessarily fall
 * under those provided by Google Test. One implementation is
 * waiting for asynchronous events.
 */
class Predicate {
public:
	// Evaluates the truth value of the predicate
	virtual bool evaluate() const = 0;
    virtual ~Predicate() {} ;
};

/**
 * TimedPredicate Class : periodically checks condition
 *
 * Tests periodically the condition defined by the predicate.
 * This test can end when either the condition is met, (such
 * as when the response is received) or if the time limit is
 * exceeded.
 */
class TimedPredicate {
public:
	/*
	 * A timed predicate needs a predicate to evalutate if a test condition match
	 * It also use a time limit, as well as an interval to which evaluate the predicate.
	 */
	TimedPredicate(Predicate* predicate, unsigned int timeLimitMillis, unsigned int retryPeriodMillis);
	// Destructor
	virtual ~TimedPredicate();
	
	/*
	 *	Evaluate the provided predicate to check if the condition match.
	 */
	virtual bool evaluate() const;

private:
	// Predicate used to evaluate the condition
	const Predicate* predicate;
	const unsigned int timeLimitMillis;
	const unsigned int retryPeriodMillis;
};

#endif // CSF_PREDICATES_H
