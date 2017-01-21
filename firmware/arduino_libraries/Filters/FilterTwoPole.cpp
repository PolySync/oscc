#include "FilterTwoPole.h"

#ifdef ARDUINO_SAM_DUE
#define ARM_FLOAT
#endif

// The driven, damped harmonic oscillator equation is:
// http://en.wikipedia.org/wiki/Harmonic_oscillator)
//
//    a + 2*ζ*ω0*v + ω0²*x = F(t)/m
//
// where the quality factor is related to the damping factor by
//
//    Q = ½ζ
//
// It is useful to normalize the force to the spring constant,
// so a force of F will result in the oscillator resting at a position x=F
// The allows the oscillator to be used as a lowpass filter, 
// where the position X functions as the output voltage V.
// Setting a=v=0, and solving for m, gives
//
//    m = 1/w0
//
// and the final equation is
//
//    a + 2*ζ*ω0*v + ω0²*x = ω0²*F(t)
//
// for determining the energy, you must know the effective spring constant, which is
// found from the equation (for undampened, where w0 = w)
//
//    w0² = k/m
//    w0 = k
//
// this makes the energy
//
//   E = 0.5*k*x² + 0.5*m*v²
//     = 0.5*w0*x² + 0.5*v²/w0
//
//
// Filter types (such as Bessel or Butterworth) are defined by 
// specific quality factors ... the quality factor also defines a 
// relationship between f0 (the undamped resonance frequency)
// and the -3 dB frequency of the filter
//
// For a Butterworth filter, these values are 
// (see figures 8.26 and 8.32 Bessel)
// (See Analog Devices note AN-649)
// http://www.analog.com/static/imported-files/application_notes/447952852AN-649_0.pdf
// http://www.analog.com/library/analogDialogue/archives/43-09/EDCh%208%20filter.pdf)
//
// Butterworth
//	F_0=1
// 	F_(-3dB)=1
// 	Q=1/√2
//
// Bessel
//	F_0=1.2754
//	F_(-3dB)=1
//	Q=1/√3
//
// (note – ramp time for a Bessel filter is about 1/(2F_0 ) )


FilterTwoPole::FilterTwoPole( float frequency0, float qualityFactor, float xInit ) {
  X = xInit;              // start it some arbitrary position
  Vprev = 0;              // initially stopped
  IsHighpass = false;     // by default, a normal oscillator

  setQ( qualityFactor );
  setFrequency0( frequency0 );

  LastTimeUS = micros();
}

void FilterTwoPole::setQ( float qualityFactor ) {
  // zero will result in divide by zero, upper value keeps it stable
  qualityFactor = constrain( qualityFactor, 1e-3, 1e3 );

  Q = qualityFactor;
}

void FilterTwoPole::setFrequency0( float f ) {
  W0 = TWO_PI*abs(f);
}

void FilterTwoPole::setAsFilter( OSCILLATOR_TYPE ft, float frequency3db, float initialValue ) {
  // if this is a highpass filter, set to invert the transfer function on the output
  //if( ft == HIGHPASS_BESSEL || ft == HIGHPASS_BUTTERWORTH ) {
  //  IsHighpass = true;
  //}
  //else {
    IsHighpass = false;
  //}

  X = initialValue;
  
  if( ft == LOWPASS_BESSEL ) {
    setFrequency0( frequency3db * 1.28 );
    setQ( 0.5774 );
  }
  //else if( ft == HIGHPASS_BESSEL ) {
  //  setFrequency0( frequency3db * 1.28 );
  //  setQ( 0.5774 );
  //}
  
  else if( ft == LOWPASS_BUTTERWORTH ) {
    // set as butterworth
    setFrequency0( frequency3db );
    setQ( 0.7071 );
  }
  //else if( ft == HIGHPASS_BUTTERWORTH ) {
    // set as butterworth
  //  setFrequency0( frequency3db );
  //  setQ( 0.7071 );
  //}
  
  
}

float FilterTwoPole::input( float drive ) {
  Fprev = drive;                      // needed when using filter as a highpass

  long now = micros();                      // get current time
  float dt = 1e-6*float(now - LastTimeUS);  // find dt
  LastTimeUS = now;                         // save the last time
  
  // constrain the dt 
  // if input has not been called frequently enough
  // the velocity and position can fly off to extremly high values
  // ... constraining the dt effectively "pauses" the motion during delays in updating
  // note this will result in an incorrect answer, but if dt is too large
  // the answer will be incorrect, regardless.
  dt = constrain( dt, 0, 1.0/W0 );

  float A = sq(W0)*drive - W0/Q*Vprev - sq(W0)*X; // *** compute acceleration
  float V = Vprev + A * dt;                       // step velocity
  Vavg = .5*(V+Vprev);
  X += Vavg * dt;                                 // step position, using average V to reduce error
                                                  // (trapezoidal integration)

  Vprev = V;                                      // save the last V

  // normally, this returns output
  // use it here to figure out how to return highpass
  
  //return Q/W0*Vavg;
  return output();
}

float FilterTwoPole::output() {
  //  if( IsHighpass )
  //    return Fprev-X-Q/W0*Vavg;   // this is almost correct ...
  //  else
    return X;          // the filtered value (position of oscillator)
}

// as a measure for the energy of the oscillator, returns the maxium amplitude
float FilterTwoPole::getMaxAmp() {
  // first, calculate the energy
  // E = 0.5*w0*x² + 0.5*v²/w0
  
  float E = 0.5 * W0 * sq(X) + 0.5 * sq(Vprev) / W0;
  
  // calculate use this to calculate max amplitude
  // E = 0.5*w0*x²
  // x = sqrt(2*E/w0)
#ifdef ARM_FLOAT
  return sqrtf(2.0*E/W0);
#else
  return sqrt(2.0*E/W0);
#endif
}

void FilterTwoPole::print() {
  Serial.print(" X: ");        Serial.print( X );
  Serial.print(" Vprev: ");    Serial.print( Vprev );
  Serial.println("");
}

void FilterTwoPole::test() {
  float updateInterval = .1;
  float nextupdateTime = 1e-6*float(micros());

  float inputValue = 0;
  FilterTwoPole osc( 0.2, 4, 0);

  while( true ) {
    float now = 1e-6*float(micros());

    // switch input values on a 20 second cycle
    if( round(now/50.0)-(now/50.0) < 0 )
      inputValue = 100;
    else
      inputValue = 150;

    osc.input(inputValue);

    analogWrite(10,osc.output() ); // hardcoded the dial pin

    if( now > nextupdateTime ) {
      nextupdateTime += updateInterval;

      Serial.print("inputValue: "); Serial.print( inputValue );
      Serial.print("\t output: "); Serial.print( osc.output() );
      Serial.println();
    }
  }
}
