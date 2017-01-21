#ifndef RunningStatistics_h
#define RunningStatistics_h

#include "FilterOnePole.h"

struct RunningStatistics {
  // in statistics, SigmaSqr is:
  //   σ^2 = <x^2> - <x>^2
  // averages can be taken by low-pass smoothing with a (two-pole) filter
  
  float AverageSecs;   // seconds to average over
  
  FilterOnePoleCascade averageValue;
  FilterOnePoleCascade averageSquareValue;
  
  void input( float inVal );
  
  // constructor
  RunningStatistics();
  
  void setWindowSecs( float windowSecs );
  
  void setInitialValue( float initialMean, float initialSigma = 0 );
    
  float mean();
  
  float variance();
  
  float sigma();
  
  float CV();
};

void testRunningStatistics();

#endif