#include "csf/utils/Event.hpp"
#include "csf/utils/Mutex.hpp"
#include "csf/utils/ScopedLock.hpp"
#include "csf/utils/Condition.hpp"

namespace csf {

namespace csflogger {

class EventData
{
public:
    bool m_autoReset;

    volatile bool m_isSignalled;

    Mutex m_mutex;
    Condition m_condition;

    EventData (bool autoReset = true) : m_autoReset(autoReset), m_isSignalled(false) {}
};

Event::Event(bool autoReset)
{
	_data = new EventData(autoReset);
}

Event::~Event()
{
	delete _data;
}

bool Event::set()
{
    ScopedLock scopedLock(_data->m_mutex);

    if (_data->m_isSignalled)
    {
        return true;
    }

    _data->m_isSignalled = true;

    if (_data->m_autoReset)
    {
        _data->m_condition.signal();
    }
    else
    {
        _data->m_condition.broadcast();
    }

    return true;
}

bool Event::reset()
{
    ScopedLock scopedLock(_data->m_mutex);

    if (_data->m_autoReset)
    {
        return false;
    }

    _data->m_isSignalled = false;

    return true;
}

bool Event::wait()
{
    ScopedLock scopedLock(_data->m_mutex);

    while(!_data->m_isSignalled)
    {
       _data->m_condition.wait(_data->m_mutex); // unlock and wait
    }

    if (_data->m_autoReset)
    {
       _data->m_isSignalled = false;
    }

    return true;

}

bool Event::timedWait(long milliseconds)
{
    ScopedLock scopedLock(_data->m_mutex);

    if( milliseconds < 0)
    {
    	//If a negative number is passed in then immediately return false.
    	return false;
    }

    bool timedOut = false;

    while(!_data->m_isSignalled)
    {

        if (_data->m_condition.timedWait(_data->m_mutex, milliseconds) > 0)
        {
            //Timed out
            timedOut = true;
            break;
        }
    }

    if ((!timedOut) && (_data->m_autoReset))
    {
        _data->m_isSignalled = false;
    }

    return !timedOut;
}

}

}//End CSF Namespace
