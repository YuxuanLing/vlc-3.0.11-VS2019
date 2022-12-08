
/* $Id: Condition.cpp 43257 2012-04-10 15:35:32Z phbernar $ */

#include <sys/time.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "csf/utils/Condition.hpp"

/*
 * Try to select the best way to implement Condition::timedWait()
 * depending on the system.
 */
#if defined(__MACH__) && defined(__APPLE__)     /* OS X and iOS */
#define USE_PTHREAD_COND_TIMEDWAIT_RELATIVE_NP
#elif defined(__linux) && !defined(__ANDROID__) /* Linux only, not Android */
#define USE_CLOCK_MONOTONIC
#endif

namespace csf {

namespace csflogger {

struct mutex_privdata {
    pthread_mutex_t _mutex;
    pthread_mutexattr_t _attr;
};

struct cond_privdata
{
  pthread_cond_t _cond;
#ifdef USE_CLOCK_MONOTONIC
  pthread_condattr_t _condattr;
#endif
};

Condition::Condition()
{
  cond = new struct cond_privdata; // no error handling
#ifdef USE_CLOCK_MONOTONIC
  ::pthread_condattr_init(&cond->_condattr);
  if (::pthread_condattr_setclock(&cond->_condattr, CLOCK_MONOTONIC) != 0) {
    ::printf("pthread_condattr_setclock: failed to set CLOCK_MONOTONIC\n");
    ::abort();
  }

  if (::pthread_cond_init(&cond->_cond, &cond->_condattr) != 0) {
    ::printf("pthread_cond_init: failed to initialize condition variable\n");
    ::abort();
  }
#else
  if (::pthread_cond_init(&cond->_cond, NULL) != 0) {
    ::printf("pthread_cond_init: failed to initialize condition variable\n");
    ::abort();
  }
#endif
}

Condition::~Condition()
{
  ::pthread_cond_destroy(&cond->_cond);  // discard return
#ifdef USE_CLOCK_MONOTONIC
  ::pthread_condattr_destroy(&cond->_condattr);  // discard return
#endif
  delete cond;
}

/**
 * Signals this condition variable.
 */
int Condition::signal()
{
  return ::pthread_cond_signal(&cond->_cond);
}

/**
 * Signals this condition variable for all waiters.
 */
int Condition::broadcast()
{
  return ::pthread_cond_broadcast(&cond->_cond);
}

/**
 * Waits on this condition variable.
 */
int Condition::wait(Mutex& mutex)
{
  return ::pthread_cond_wait(&cond->_cond, &mutex.pdata->_mutex);
}

static struct timespec
milliseconds_to_timespec(long ms)
{
  struct timespec ts;

  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms - ts.tv_sec * 1000) * 1000000;

  return ts;
}

#ifdef USE_PTHREAD_COND_TIMEDWAIT_RELATIVE_NP
/*
 * Implementation of Condition::timedWait() using the non-portable
 * pthread_cond_timedwait_relative_np() function provided by Mac OS X.
 */
int Condition::timedWait(Mutex& mutex, long timeout_ms)
{
  struct timespec ts;

  ts = milliseconds_to_timespec(timeout_ms);
  int e = ::pthread_cond_timedwait_relative_np(&cond->_cond,
                    &mutex.pdata->_mutex, &ts);

  if (e == EINVAL) {
    ::printf("Condition::timedWait: pthread_cond_timedwait_relative_np:"
             " EINVAL\n");
  }

  return e;
}
#else   /* !USE_PTHREAD_COND_TIMEDWAIT_RELATIVE_NP */

static struct timespec
timespec_add(struct timespec ts1, struct timespec ts2)
{
  struct timespec ts;
  const ldiv_t ld = ::ldiv(ts1.tv_nsec + ts2.tv_nsec, 1000000000L);
  ts.tv_sec = ts1.tv_sec + ts2.tv_sec + ld.quot;
  ts.tv_nsec = ld.rem;

  return ts;
}

static struct timespec
currentTimeTimespec()
{
#ifdef USE_CLOCK_MONOTONIC
  struct timespec tsnow;

  if (::clock_gettime(CLOCK_MONOTONIC, &tsnow) != 0) {
    ::perror("currentTimeTimespec: clock_gettime");
    tsnow.tv_sec = tsnow.tv_nsec = 0;
  }

  return tsnow;
#else   /* !USE_CLOCK_MONOTONIC */
  struct timeval tvnow;

  if (::gettimeofday(&tvnow, NULL) != 0) {
    ::perror("currentTimeTimespec: gettimeofday");
    tvnow.tv_sec = tvnow.tv_usec = 0;
  }

  /* Convert timeval to timespec */
  struct timespec tsnow;
  const ldiv_t ld = ::ldiv(tvnow.tv_usec * 1000L, 1000000000L);
  tsnow.tv_sec = tvnow.tv_sec + ld.quot;
  tsnow.tv_nsec = ld.rem;

  return tsnow;
#endif  /* USE_CLOCK_MONOTONIC */
}

/*
 * Implementation of Condition::timedWait() using the standard
 * pthread_cond_timedwait() function.  The timespec argument is the
 * *absolute* time.
 * currentTimeTimespec() is used to retrieve the current time.  The
 * underlying implementation may use CLOCK_MONOTONIC if available
 * (good), or, if not, gettimeofday(2) (bad, since it's not a monotonic
 * clock).
 */
int Condition::timedWait(Mutex& mutex, long timeout_ms)
{
  struct timespec ts;

  ts = milliseconds_to_timespec(timeout_ms);
  ts = timespec_add(ts, currentTimeTimespec());
  int e = ::pthread_cond_timedwait(&cond->_cond, &mutex.pdata->_mutex, &ts);

  if (e == EINVAL) {
    ::printf("Condition::timedWait: pthread_cond_timedwait: EINVAL\n");
  }

  return e;
}
#endif  /* USE_PTHREAD_COND_TIMEDWAIT_RELATIVE_NP */

}

} // namespace csf

