#include "FilterOnePole.h"
#include "FloatDefine.h"

FilterOnePole::FilterOnePole( FILTER_TYPE ft, float fc, float initialValue ) {
  setFilter( ft, fc, initialValue );
}

void FilterOnePole::setFilter( FILTER_TYPE ft, float fc, float initialValue ) {
  FT = ft;
  setFrequency( fc );

  Y = initialValue;
  Ylast = initialValue;
  X = initialValue;

  LastUS = micros();
}

float FilterOnePole::input( float inVal ) {
  long time = micros();
  ElapsedUS = float(time - LastUS);   // cast to float here, for math
  LastUS = time;                      // update this now

  // shift the data values
  Ylast = Y;
  X = inVal;                          // this is now the most recent input value
  
  // filter value is controlled by a parameter called X
  // tau is set by the user in microseconds, but must be converted to samples here
  TauSamps = TauUS / ElapsedUS;
  
  float ampFactor;
#ifdef ARM_FLOAT
  ampFactor = expf( -1.0 / TauSamps );     // this is 1 if called quickly
#else
  ampFactor = exp( -1.0 / TauSamps );      // this is 1 if called quickly
#endif
  
  Y = (1.0-ampFactor)*X + ampFactor*Ylast;     // set the new value

  return output();
}

void FilterOnePole::setFrequency( float newFrequency ) {
  setTau( 1.0/(TWO_PI*newFrequency ) ); // τ=1/ω
}

void FilterOnePole::setTau( float newTau ) {
  TauUS = newTau * 1e6;
}

float FilterOnePole::output() {
    // figure out which button to read
  switch (FT) {
    case LOWPASS:         
      // return the last value
      return Y; 
      break;
    case INTEGRATOR:      
      // using a lowpass, but normaize
      return Y * (TauUS/1.0e6);
      break;
    case HIGHPASS:       
      // highpass is the _difference_
      return X-Y;
      break;
    case DIFFERENTIATOR:
      // like a highpass, but normalize
      return (X-Y)/(TauUS/1.0e6);
      break;
    default:
      // should never get to here, return 0 just in case
      return 0;
  }
}

void FilterOnePole::print() {
  Serial.println("");
  Serial.print(" Y: ");        Serial.print( Y );
  Serial.print(" Ylast: ");      Serial.print( Ylast );
  Serial.print(" X ");         Serial.print( X );
  Serial.print(" ElapsedUS ");  Serial.print( ElapsedUS );
  Serial.print(" TauSamps: ");  Serial.print( TauSamps );
  //Serial.print(" ampFactor " );       Serial.print( ampFactor );
  Serial.print(" TauUS: ");     Serial.print( TauUS );
  Serial.println("");
}

void FilterOnePole::test() {
  float tau = 10;
  float updateInterval = 1;
  float nextupdateTime = millis()*1e-3;

  float inputValue = 0;
  FilterOnePole hp( HIGHPASS, tau, inputValue );
  FilterOnePole lp( LOWPASS, tau, inputValue );

  while( true ) {
    float now = millis()*1e-3;

    // switch input values on a 20 second cycle
    if( round(now/20.0)-(now/20.0) < 0 )
    inputValue = 0;
    else
    inputValue = 100;

    hp.input(inputValue);
    lp.input(inputValue);

    if( now > nextupdateTime ) {
      nextupdateTime += updateInterval;

      Serial.print("inputValue: "); Serial.print( inputValue );
      Serial.print("\t high-passed: "); Serial.print( hp.output() );
      Serial.print("\t low-passed: "); Serial.print( lp.output() );
      Serial.println();
    }
  }
}

void FilterOnePole::setToNewValue( float newVal ) {
  Y = Ylast = X = newVal;
}


// stuff for filter2 (lowpass only)
// should be able to set a separate fall time as well
FilterOnePoleCascade::FilterOnePoleCascade( float riseTime, float initialValue ) {
  setRiseTime( riseTime );
  setToNewValue( initialValue );
}

void FilterOnePoleCascade::setRiseTime( float riseTime ) {
  float tauScale = 3.36;      // found emperically, by running test();

  Pole1.setTau( riseTime / tauScale );
  Pole2.setTau( riseTime / tauScale );
}

float FilterOnePoleCascade::input( float inVal  ) {
  Pole2.input( Pole1.input( inVal ));
  return output();
}

// clears out the values in the filter
void FilterOnePoleCascade::setToNewValue( float newVal ) {
  Pole1.setToNewValue( newVal );
  Pole2.setToNewValue( newVal );
}

float FilterOnePoleCascade::output() {
  return Pole2.output();
}

void FilterOnePoleCascade::test() {
  // make a filter, how fast does it run:
  
  float rise = 1.0;
  FilterOnePoleCascade myFilter( rise );
  
  // first, test the filter speed ...
  long nLoops = 1000;
  
  Serial.print( "testing filter with a rise time of ");
  Serial.print( rise ); Serial.print( "s" );
  
  Serial.print( "\n running filter speed loop ... ");
  
  float startTime, stopTime;
  
  startTime = millis()*1e-3;
  for( long i=0; i<nLoops; ++i ) {
    myFilter.input( PI );   // use pi, so it will actually do a full calculation
  }
  stopTime = millis()*1e-3;
  
  Serial.print( "done, filter runs at " );
  Serial.print( float(nLoops) / (stopTime - startTime) );
  Serial.print( " hz " );
  Serial.print( "\n filter value: " ); Serial.print( myFilter.output() );

  myFilter.setToNewValue( 0.0 );
  Serial.print( "\n after reset to 0: "); Serial.print( myFilter.output() );
  
  Serial.print( "\n testing rise time (10% to 90%) ...");
  
  bool crossedTenPercent = false;
  while( myFilter.output() < 0.9 ) {
    myFilter.input( 1.0 );
    if( myFilter.output() > 0.1 && !crossedTenPercent ) {
      // filter first crossed the 10% point
      startTime = millis()*1e-3;
      crossedTenPercent = true;
    }
  }
  stopTime = millis()*1e-3;
  
  Serial.print( "done, rise time: " ); Serial.print( stopTime-startTime );
  
  Serial.print( "testing attenuation at f = 1/risetime" );
  
  myFilter.setToNewValue( 0.0 );
  
  float maxVal = 0;
  float valWasOutputThisCycle = true;
  
  float lastFilterVal = 0;
  
  while( true ) {
    float now = 1e-3*millis();
    
    float currentFilterVal = myFilter.input( sin( TWO_PI*now) );

    if( currentFilterVal < 0.0 ) {
      if( !valWasOutputThisCycle ) {
        // just crossed below zero, output the max
        Serial.print( maxVal*100 ); Serial.print( " %\n" );
        valWasOutputThisCycle = true;
      }
      
    }
    
  }
  
    
  
}
