#ifndef ASYNCDELAY_H
#define ASYNCDELAY_H

#include <Arduino.h>

typedef void (*delay_callback)(void);

class AsyncDelay
{
  public:
  	long WakeDurration;     // milliseconds of on-time
  	long SleepDurration;    // milliseconds of off-time
    long MilliSecRemaining;

  private:
  	// These maintain the current state
  	bool _delay;             		
  	unsigned long _previousMillis;  
    unsigned long _currentMillis;
  
    delay_callback delayStartCallback;
    delay_callback delayStopCallback;

  public:
    AsyncDelay(long wakeDurration, long sleepDurration)
    {    	  
    	WakeDurration = wakeDurration;
    	SleepDurration = sleepDurration;
    	
    	_delay = false; 
    	_previousMillis = 0;
      MilliSecRemaining = 0;
    }

    void Start(delay_callback f);
    void Stop(delay_callback f);

  void Loop()
  {
    // check to see if it's time to change the state
    _currentMillis = millis();
     MilliSecRemaining = _currentMillis - _previousMillis;
    if((_delay == true) && (MilliSecRemaining >= WakeDurration))
    {
    	_delay = false;  // Turn it off
      _previousMillis = _currentMillis;  // Remember the time
      (*delayStopCallback)();
    }
    else if ((_delay == false) && (MilliSecRemaining >= SleepDurration))
    {
      _delay = true;  // turn it on
      _previousMillis = _currentMillis;   // Remember the time
     (*delayStartCallback)();
    }
  }
};
#endif


