#include <Filters.h>

float testFrequency = 2;                     // test signal frequency (Hz)
float testAmplitude = 100;                   // test signal amplitude
float testOffset = 100;

float windowLength = 20.0/testFrequency;     // how long to average the signal, for statistist

float testSignalSigma = testAmplitude / sqrt(2.0);         // the RMS amplitude of the test signal
float testSignal3dbSigma = testSignalSigma / sqrt(2.0);    // the RMS amplitude of the test signal, down -3db

float printPeriod = 5.0;

// return the current time
float time() {
  return float( micros() ) * 1e-6;
}

void setup() {
  Serial.begin( 57600 );    // start the serial port
}

void testOnePoleFilters() {
  // filters are test with a sine wave input, keep track of those values here for a sanity check
  RunningStatistics inputStats;                 // create statistics to look at the raw test signal
  inputStats.setWindowSecs( windowLength );
  
  FilterOnePole filterOneLowpass( LOWPASS, testFrequency );   // create a one pole (RC) lowpass filter
  RunningStatistics filterOneLowpassStats;                    // create running statistics to smooth these values
  filterOneLowpassStats.setWindowSecs( windowLength );
  
  FilterOnePole filterOneHighpass( HIGHPASS, testFrequency );  // create a one pole (RC) highpass filter
  RunningStatistics filterOneHighpassStats;                    // create running statistics to smooth these values
  filterOneHighpassStats.setWindowSecs( windowLength );
  
  float startTime = time();
  float nextPrintTime = time();
  
  while( true ) {
    // update all real time classes
    float inputValue = testAmplitude + testAmplitude*sin( TWO_PI * testFrequency * time() );

    // update the test value statistics
    inputStats.input( inputValue);
    
    // update the one pole lowpass filter, and statistics
    filterOneLowpass.input( inputValue );
    filterOneLowpassStats.input( filterOneLowpass.output() );
    
    // update the one pole highpass filter, and statistics
    filterOneHighpass.input( inputValue );
    filterOneHighpassStats.input( filterOneHighpass.output() );
    
    if( time() > nextPrintTime ) {
      // display current values to the screen
      nextPrintTime += printPeriod;   // update the next print time
      
      Serial.print( "\n" );
      Serial.print( "time: " ); Serial.print( time() );
      
      // output values associated with the inputValue itsel
      Serial.print( "\tin: " ); Serial.print( inputStats.mean() ); Serial.print( " +/- " ); Serial.print( inputStats.sigma() );
      Serial.print( " (" ); Serial.print( testOffset ); Serial.print( " +/- " ); Serial.print( testSignalSigma ); Serial.print( ")" );
      
      // output values associated with the lowpassed value
      Serial.print( "\tLP1: " ); Serial.print( filterOneLowpassStats.mean() ); Serial.print( " +/- " ); Serial.print( filterOneLowpassStats.sigma() );
      Serial.print( " (" ); Serial.print( testOffset ); Serial.print( " +/- " ); Serial.print( testSignal3dbSigma ); Serial.print( ")" );

      // output values associated with the highpass value
      Serial.print( "\tHP1: " ); Serial.print( filterOneHighpassStats.mean() ); Serial.print( " +/- " ); Serial.print( filterOneHighpassStats.sigma() );
      Serial.print( " (" ); Serial.print( "0.0" ); Serial.print( " +/- " ); Serial.print( testSignal3dbSigma ); Serial.print( ")" );
    }
  }
}

void testTwoPoleFilters() {
  float factor = sqrt(10);  // sqrt 10 will make the attenuation 10x
  
  // filters are test with a sine wave input, keep track of those values here for a sanity check
  RunningStatistics inputStats;                 // create statistics to look at the raw test signal
  inputStats.setWindowSecs( windowLength );

  // standard Lowpass, set to the corner frequency
  FilterTwoPole filterTwoLowpass;                                       // create a two pole Lowpass filter
  filterTwoLowpass.setAsFilter( LOWPASS_BUTTERWORTH, testFrequency );

  RunningStatistics filterTwoLowpassStats;
  filterTwoLowpassStats.setWindowSecs( windowLength );

  // Lowpass, set above corner frequency
  FilterTwoPole filterTwoLowpassAbove;                                       // create a two pole Lowpass filter
  filterTwoLowpassAbove.setAsFilter( LOWPASS_BUTTERWORTH, testFrequency*factor );

  RunningStatistics filterTwoLowpassAboveStats;
  filterTwoLowpassAboveStats.setWindowSecs( windowLength );

  // Lowpass, set below corner frequency
  FilterTwoPole filterTwoLowpassBelow;                                       // create a two pole Lowpass filter
  filterTwoLowpassBelow.setAsFilter( LOWPASS_BUTTERWORTH, testFrequency/factor );

  RunningStatistics filterTwoLowpassBelowStats;
  filterTwoLowpassBelowStats.setWindowSecs( windowLength );

  float startTime = time();
  float nextPrintTime = time();
  
  while( true ) {
    // update all real time classes
    float inputValue = testAmplitude + testAmplitude*sin( TWO_PI * testFrequency * time() );
        
    // update the test value statistics
    inputStats.input( inputValue);
        
    // update the two pole Lowpass filter, and statistics
    filterTwoLowpass.input( inputValue );
    filterTwoLowpassStats.input( filterTwoLowpass.output() );
    
    // update the two pole Lowpass filter, and statistics
    filterTwoLowpassAbove.input( inputValue );
    filterTwoLowpassAboveStats.input( filterTwoLowpassAbove.output() );

    // update the two pole Lowpass filter, and statistics
    filterTwoLowpassBelow.input( inputValue );
    filterTwoLowpassBelowStats.input( filterTwoLowpassBelow.output() );
    
    if( time() > nextPrintTime ) {
      // display current values to the screen
      nextPrintTime += printPeriod;   // update the next print time
      
      Serial.print( "\n" );
      Serial.print( "time: " ); Serial.print( time() );
      
      // output values associated with the inputValue itsel
      Serial.print( "\tin: " ); Serial.print( inputStats.mean() ); Serial.print( " +/- " ); Serial.print( inputStats.sigma() );
      Serial.print( " (" ); Serial.print( testOffset ); Serial.print( " +/- " ); Serial.print( testSignalSigma ); Serial.print( ")" );
            
      // output values associated with the Lowpass value
      Serial.print( "\tLP2: " ); Serial.print( filterTwoLowpassStats.mean() ); Serial.print( " +/- " ); Serial.print( filterTwoLowpassStats.sigma() );
      Serial.print( " (" ); Serial.print( "0.0" ); Serial.print( " +/- " ); Serial.print( testSignal3dbSigma ); Serial.print( ")" );
      
      // output values associated with the Lowpass value, set above corner frequency (should be almost full)
      Serial.print( "\tLP2A: " ); Serial.print( filterTwoLowpassAboveStats.mean() ); Serial.print( " +/- " ); Serial.print( filterTwoLowpassAboveStats.sigma() );

      // output values associated with the Lowpass value, set above corner frequency (should be lower amplitude)
      Serial.print( "\tLP2B: " ); Serial.print( filterTwoLowpassBelowStats.mean() ); Serial.print( " +/- " ); Serial.print( filterTwoLowpassBelowStats.sigma() );
    }
  }
}

void loop() {
  //testOnePoleFilters();
  testTwoPoleFilters();
}
