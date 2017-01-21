#ifndef FilterTwoPole_h
#define FilterTwoPole_h
#include <Arduino.h>
#define ATTODUINO

enum OSCILLATOR_TYPE {
  LOWPASS_BESSEL,
  //HIGHPASS_BESSEL,
  LOWPASS_BUTTERWORTH,
  //HIGHPASS_BUTTERWORTH,
};

// implements a driven harmonic oscillator, which can be used as a filter
// mass is normalized to unity (does not appear explicity in the equations),
// and the driving force is writtin in units of spring constant, so a static force
// of X will cause the oscillator come to rest at X
//
// The oscillator parameters are
//   W0 (undampened resonant frequency) ... the user sets this in Hz (as F0)
//   Q  (quality factor)
//
// In addition, the ocillator can be configured as a (two-pole) lowpass or high filter,
// since these circuits are directly analogous to harmonic oscillators.

// note that the two-pole lowpass Bessel, the rise time (for a step) and
// and impulse response width are approx tau/2.0, or 1/(2*w0):
//  a tau = 1 is w0 (and wc) of TWO_PI
//  this has a rise time / impulse response of about 0.4
// (i.e., the intergration time is about 0.4 * tau)

struct FilterTwoPole {

  //OSCILLATOR_TYPE FT;
  float X;        // position
  float Vprev;    // previously computed velocity
  float Vavg;     // average of the last two calculated velocities
  float Fprev;    // previous driving force (not frequency!)

  float Q;        // quality factor, must be > 0
  float W0;       // undamped resonance frequency

  bool IsHighpass;  // false for normal output, true will make a lowpass into a highpass

  long LastTimeUS;  // last time measured

  FilterTwoPole( float frequency0 = 1, float qualityFactor = 1, float xInit = 0);

  void setQ( float qualityFactor );

  void setFrequency0( float f );

  void setAsFilter( OSCILLATOR_TYPE ft, float frequency3db, float initialValue=0 );

  float input( float drive = 0 );

  float output();
  
  // as a measure for the energy of the oscillator, returns the maxium amplitude
  float getMaxAmp();

  void print();

  void test();

};


#endif

