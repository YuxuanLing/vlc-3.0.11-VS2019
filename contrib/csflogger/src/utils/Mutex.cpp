#include "csf/utils/Mutex.hpp"

#ifdef WIN32
#include <windows.h>
#include <stdio.h>

namespace csf {

namespace csflogger {

struct mutex_privdata {
  CRITICAL_SECTION m_criticalSection;
};

Mutex::Mutex()
{
    pdata = new struct mutex_privdata;
    if (pdata == 0) {
        printf("Error in Mutex::Mutex(): could not allocate memory\n" );
        return;
    }

    InitializeCriticalSection(&pdata->m_criticalSection);
}

Mutex::~Mutex()
{
    if (pdata != NULL) {
        DeleteCriticalSection(&pdata->m_criticalSection);
        delete pdata;
    }
}

int Mutex::lock()
{
    // Check for poorly-constructed object
    if (pdata == NULL)  {
        return 1;
    }

    EnterCriticalSection(&pdata->m_criticalSection);
    return 0;
}

int Mutex::unlock()
{
    // Check for poorly-constructed object
    if (pdata == NULL)  {
        return 1;
    }

    LeaveCriticalSection(&pdata->m_criticalSection);
    return 0;
}

}

} // namespace csf


#else

#include <pthread.h>

namespace csf {

namespace csflogger {

struct mutex_privdata {
    pthread_mutex_t _mutex;
    pthread_mutexattr_t _attr;
};

Mutex::Mutex()
{
  /* XXX error handling */
  pdata = new struct mutex_privdata;
  ::pthread_mutexattr_init(&pdata->_attr);  // discard return
  ::pthread_mutexattr_settype(&pdata->_attr, PTHREAD_MUTEX_RECURSIVE);
  ::pthread_mutex_init(&pdata->_mutex, &pdata->_attr);  // discard return
}

Mutex::~Mutex()
{
  ::pthread_mutex_destroy(&pdata->_mutex); // discard return
  ::pthread_mutexattr_destroy(&pdata->_attr); // discard return
  delete pdata;
}

/**
 * Locks this mutex.
 */
int Mutex::lock()
{
   return ::pthread_mutex_lock(&pdata->_mutex);
}

/**
 * Unlocks this mutex.
 */
int Mutex::unlock()
{ 
  return ::pthread_mutex_unlock(&pdata->_mutex);
}

}

} // namespace csf

#endif