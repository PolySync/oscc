#include "RunningStatistics.h"
#include "FloatDefine.h"

RunningStatistics::RunningStatistics() {
  setWindowSecs( 1 );   // setup with one second average
  setInitialValue( 0 ); // start with zero
}

void RunningStatistics::setWindowSecs( float windowSecs ) {
  AverageSecs = windowSecs;
  
  averageValue.setRiseTime( AverageSecs );
  averageSquareValue.setRiseTime( AverageSecs );
}

void RunningStatistics::setInitialValue( float initialMean, float initialSigma ) {
  averageValue.setToNewValue( initialMean );
  averageSquareValue.setToNewValue( sq(initialMean) + sq(initialSigma ) );
}

void RunningStatistics::input( float inVal ) {
  averageValue.input(inVal);              // calculates running average
  averageSquareValue.input(inVal*inVal);  // calculates running average of square
}
  
float RunningStatistics::mean() {
  return averageValue.output();
}
  
float RunningStatistics::variance() {
  float var = averageSquareValue.output() - averageValue.output()*averageValue.output();
    
  // because of the filtering, it's possible that this could be negative ... check!
  if( var < 0 ) var = 0;
    
  return var;
}
  
float RunningStatistics::sigma() {
  
#ifdef ARM_FLOAT
  return sqrtf(variance());
#else
  return sqrt(variance());
#endif

}

float RunningStatistics::CV() {
  static const float maxCV = 1e3;
  float meanTmp = mean();
  
  // prevent divide by zero
  if( meanTmp == 0 ) return maxCV;
  else               return sigma() / meanTmp;
}

void testRunningStatistics() {
  // a speed test for running statistics
  
  RunningStatistics myStats;
  
  myStats.setInitialValue( (1.0/1024)*float(analogRead( A0 )) );
  
  float updateInterval = 1.0;
  float nextUpdateTime = 1e-6*float(micros()) + updateInterval;
  
  long nLoops = 0;
  
  while( true ) {
    myStats.input( (1.0/1024)*float(analogRead( A0 )) );
    nLoops++;
    float t = 1e-6*float(micros());
    
    if( t > nextUpdateTime ) {
      nextUpdateTime += updateInterval;
      
      Serial.print( "mean: "); Serial.print( myStats.mean() );
      Serial.print( "\tsigma: " ); Serial.print( myStats.sigma() );
      Serial.print( "\tHz: "); Serial.print( nLoops );
      
      nLoops = 0;
      Serial.print("\n");
    }
  }
}
