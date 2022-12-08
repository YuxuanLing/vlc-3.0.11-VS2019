
/* $Id: ScopedUnlock.cpp 42678 2011-07-28 17:07:40Z phbernar $ */

#include "csf/utils/ScopedUnlock.hpp"
#include "csf/utils/Mutex.hpp"

namespace csf {

namespace csflogger {

ScopedUnlock::ScopedUnlock(Mutex& mutex) : _pMutex(&mutex)
{
  _pMutex->unlock();
}

ScopedUnlock::~ScopedUnlock()
{
  _pMutex->lock();
}

}

} // namespace csf

