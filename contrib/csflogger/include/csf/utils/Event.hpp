#pragma once

namespace csf
{

namespace csflogger
{

class EventData;

class Event
{
public:
    /**
      Creates an Event. If autoReset is true, the Event is automatically reset after an await() successfully returns.
      If autoReset is specified as false, then this is a so-called "manual-reset event". A manual-reset event object
      remains signalled until it is set explicitly to the non-signaled state by calling the reset() function.

      When constructed, an Event object is initially in the non-signalled state, whether it was constructed as an
      auto-reset or manual-reset event (autoReset specified as true/false respectively).

      Note: For a manual-reset event any number of waiting threads, or threads that subsequently begin a wait operation
      on the specified event object (by calling one of the await functions), will be released immediately while the
      object's state is signalled.

      The state of an auto-reset event object remains signalled until a single waiting thread is released, at which
      time the system automatically sets the state to non-signaled. If no threads are waiting, the event object's
      state remains signalled indefinitely until a thread waits on the event at which point that single thread is
      released and the Event auto resets itself, and can be used again.

      Important: The onus is on the user of this object to ensure that the object is not destroyed while there
                 is another thread currently using it, e.g. blocked in the await() function.
     */
    Event (bool autoReset = true);

    ~Event();

    /**
      Signals the event.

      A manual-reset Event object remains signaled until it is set explicitly to the non-signaled state by
      calling Event::reset(). Any number of waiting threads that are waiting at the point in time, or in
      fact threads that subsequently call Event::await() for the specified Event object after this
      time, can be released while the object remains in this signalled state.

      An auto-reset Event object remains signaled until such time as a single waiting thread is released,
      at which time the system automatically sets the Event state to non-signaled. If no threads are waiting
      at the time Event::set() is called, then the Event object's state remains signalled until such
      time as a single thread calls Event::await(). This is imporant, as otherwise there would be a
      potential race condition in a thread attempting to reach get to the await() function in time for the set()
      to be called. Because of the way Event is implemented it doesn't matter if the set() is called before
      a thread gets to wait on it.

      Setting a Event that is already set has no effect.

      If you "set" an auto-reset Event with multiple threads waiting, which threads gets signalled is
      indeterminate (cannot be assumed to be any specific thread).

      If it's a manual-reset Event (autoReset=false), all waiting threads will resume execution when
      Event::set() is called.

      @return Returns true if successfully set, false if there was an error.
      */
    bool set();

    /**

      The state of a Event object remains non-signalled until it is explicitly set to signalled
      by the Event::set() function. When non-signalled the Event will block any threads that
      call Event::await().

      The Event::reset() function is for manual-reset Event objects, which must be set
      explicitly to the non-signalled state. Auto-reset event objects automatically change from signalled
      to non-signalled after a single waiting thread is released.

      Resetting a Event that is already reset has no effect.

      Note: If the Event was created as an autoReset event, then this function should not be used,
            as the Event automatically resets itself when a waiting thread unblocks.

      @return Returns true if this is a manual Event, false otherwise. For a manual Event it
              is safe to call reset even if the event is already in an unsignalled state.
     */
    bool reset ();

    /**
      Waits up to the specified period of time for the Event to become signalled.

      @param milliseconds - An integer value specifying the maximum length of time to wait for the
                            event to become signalled. The supported value ranges from 0-0x7FFFFFFF ms,
                            or 0 to ~596 hours. If you specify 0 then this function acts immediately
                            checks to see if the event is signalled or not, without blocking for any
                            time, returning true if the event is set (signalled). A value of false
                            indicates that the event is currently not in a signalled state. If a negative
                            value is specified then it will immediately return false.

      @return Assuming a positive value was specified for timeout this function returns true if the event
              became signalled within the specified period, and false if the event remains in the
              non-signalled state within the time period specified. If 0 is specified then this function
              check the current state of the event and return true if it's currently signalled and false
              if it's not (no delay).
              If a negative value was specified this function will immediately return false.

    */

    bool timedWait (long milliseconds);

    /*
      Waits for an indefinite period of time for an Event to become signalled.

     @return This function returns true if the event became signalled. This function will potentially block forever until the event signals.
     	 	 Assuming the event signals at some point then this function will return true.
     */
    bool wait();


private:
    Event(const Event& rhs);
    Event& operator=(const Event& rhs);
    EventData * _data;
};

}

}//End CSF Namespace
