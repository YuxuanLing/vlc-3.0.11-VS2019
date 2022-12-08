/* $Id: Condition.hpp 42447 2011-04-28 11:47:07Z phbernar $ */

#ifndef CSF_CONDITION_INCLUDED
#define CSF_CONDITION_INCLUDED

#include "Mutex.hpp"

namespace csf {

namespace csflogger {

struct cond_privdata;

/**
 * A condition variable.
 * Designed for use on Android x86 for Cius.  Exception handling and RTTI are not used.
 */
class Condition
{
 public:
  Condition();
  ~Condition();

  /**
   * Signals this condition variable.
   */
  int signal();

  /**
   * Signals this condition variable to all waiters.
   */
  int broadcast();

  /**
   * Waits on this condition variable.
   */
  int wait(Mutex& mutex);

  int timedWait(Mutex & mutex, long timeout_ms);

 private:
  struct cond_privdata *cond;

  Condition(const Condition&);  // no copy ctor
  Condition& operator=(const Condition&);  // no assignment
};

}

} // namespace csf

#endif // CSF_CONDITION_INCLUDED

