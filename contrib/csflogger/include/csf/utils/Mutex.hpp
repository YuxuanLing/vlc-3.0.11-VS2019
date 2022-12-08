#pragma once

#include "../logger/CSFLoggerCommon.h"

namespace csf {

namespace csflogger {

struct mutex_privdata;

/**
 * A simple re-entrant mutex.
 * Designed for use on Android x86 for Cius.  Exception handling and RTTI are not used.
 *
 * On Windows this is implemented using a CRITICAL_SECTION.
 * This presents a couple of specific requirements, as outlined in the comments for lock and unlock below.
 *
 * Full details on CRITICAL_SECTION can be found in this MSDN article:
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms682530%28v=vs.85%29.aspx
 */
class CSF_LOGGER_API Mutex
{
  friend class Condition;

public:
  Mutex();
  ~Mutex();

  /**
   * Locks this mutex. This should not be called directly by clients, rather the Mutex should be passed to a
   * ScopedLock that will balance the lock and unlock calls and use RAII to ensure the Mutex is unlocked as the stack unwinds.
   *
   * Note that on Windows, lock calls may timeout resulting in a Structured Exception that will crash the process.
   * The timeout value is specified by the following registry key and by default is set to 2592000 seconds (30 days):
   *
   * HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\CriticalSectionTimeout
   */
  int lock();

  /**
   * Unlocks this mutex. This should not be called directly by clients, rather the Mutex should be passed to a
   * ScopedLock that will balance the lock and unlock calls and use RAII to ensure the Mutex is unlocked as the stack unwinds.
   *
   * Note that on Windows, calling unlock without first having called lock may result in a deadlock in subsequent calls to lock. 
   */
  int unlock();

 private:
  struct mutex_privdata *pdata;

  Mutex(const Mutex&);  // no copy ctor
  Mutex& operator=(const Mutex&);  // no assignment
};

}

} // namespace csf

