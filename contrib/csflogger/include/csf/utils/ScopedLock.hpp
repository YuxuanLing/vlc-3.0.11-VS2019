#pragma once

#include "../logger/CSFLoggerCommon.h"

namespace csf {

namespace csflogger {

class Mutex;

/**
 * @brief A lock that acquires and releases a Mutex when it is created and destroyed.
 *
 * Uses 'Resource Acquisition Is Initialization' idiom.
 * Designed for use on Android x86 for Cius.  Exception handling and RTTI are not used.
 */
class CSF_LOGGER_API ScopedLock
{
 public:
  /**
   * @brief Creates a new ScopedLock.
   * @param mutex The Mutex to manipulate.
   */
  ScopedLock(Mutex& mutex);
  ~ScopedLock();

  /**
   * @brief Unlocks the associated mutex.
   *
   * Only has effect the first time it is called.
   */
  int unlock();

 private:
  Mutex* _pMutex;

  ScopedLock(const ScopedLock&);  // no copy ctor
  ScopedLock& operator=(const ScopedLock&);  // no assignment
};

}

} // namespace csf
