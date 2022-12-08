#pragma once

#include "../logger/CSFLoggerCommon.h"

namespace csf {

namespace csflogger {

class Mutex;

/**
 * @brief A lock that releases and acquires a Mutex when it is created
 *        and destroyed.
 *
 * Uses 'Resource Acquisition Is Initialization' idiom.
 * Designed for use on Android x86 for Cius.  Exception handling and RTTI are not used.
 */
class CSF_LOGGER_API ScopedUnlock
{
 public:
  /**
   * @brief Creates a new ScopedUnlock and unlocks the given Mutex object.
   * @param mutex The Mutex to manipulate.
   */
  ScopedUnlock(Mutex& mutex);
  /**
   * @brief Locks the associated Mutex object and release the ScopedUnlock
   * object.
   */
  ~ScopedUnlock();

 private:
  Mutex* _pMutex;

  ScopedUnlock(const ScopedUnlock&);  // no copy ctor
  ScopedUnlock& operator=(const ScopedUnlock&);  // no assignment
};

}

} // namespace csf