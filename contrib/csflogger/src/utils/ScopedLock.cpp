#include "csf/utils/ScopedLock.hpp"
#include "csf/utils/Mutex.hpp"

namespace csf {

namespace csflogger {

ScopedLock::ScopedLock(Mutex& mutex) : _pMutex(&mutex)
{
  _pMutex->lock();
}

ScopedLock::~ScopedLock()
{
  unlock();
}

int ScopedLock::unlock()
{
  // only has effect the first time it is called
  if(_pMutex) {
    int r = _pMutex->unlock();
    _pMutex = 0;
    return r;
  } else {
    return 0;
  }
}

}

} // namespace csf

