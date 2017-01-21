#ifndef FilterOnePole_h
#define FilterOnePole_h

#include <Arduino.h>

enum FILTER_TYPE {
  HIGHPASS,
  LOWPASS,
  INTEGRATOR,
  DIFFERENTIATOR
};

// the recursive filter class implements a recursive filter (low / pass / highpass
// note that this must be updated in a loop, using the most recent acquired values and the time acquired
//   Y = a0*X + a1*Xm1
//              + b1*Ylast
struct FilterOnePole {
  FILTER_TYPE FT;
  float TauUS;       // decay constant of the filter, in US
  float TauSamps;    // tau, measued in samples (this changes, depending on how long between input()s

  // filter values - these are public, but should not be set externally
  float Y;       // most recent output value (gets computed on update)
  float Ylast;   // prevous output value

  float X;      // most recent input value

  // elapsed times are kept in long, and will wrap every
  // 35 mins, 47 seconds ... however, the wrap does not matter,
  // because the delta will still be correct (always positive and small)
  float ElapsedUS;   // time since last update
  long LastUS;       // last time measured

  FilterOnePole( FILTER_TYPE ft=LOWPASS, float fc=1.0, float initialValue=0 );
  
  // sets or resets the parameters and state of the filter
  void setFilter( FILTER_TYPE ft, float tauS, float initialValue );

  void setFrequency( float newFrequency );
  
  void setTau( float newTau );

  float input( float inVal );

  float output();

  void print();

  void test();
  
  void setToNewValue( float newVal );  // resets the filter to a new value
};

// two pole filter, these are very useful
struct FilterOnePoleCascade {

  FilterOnePole Pole1;
  FilterOnePole Pole2;
  
  FilterOnePoleCascade( float riseTime=1.0, float initialValue=0 );  // rise time to step function, 10% to 90%
  
  // rise time is 10% to 90%, for a step input
  void setRiseTime( float riseTime );
  
  void setToNewValue( float newVal );
  
  float input( float inVal );
  
  float output();
  
  void test();
};


#endif

