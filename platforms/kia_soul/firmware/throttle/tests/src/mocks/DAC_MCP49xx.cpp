/*
 * Microchip MCP4901 / MCP4911 / MCP4921 / MCP4902 / MCP4912 / MCP4922 8/10/12-bit DAC driver
 * Thomas Backman, 2012-07 (made a proper library 2011-07-30, 3 weeks after initial)
 * serenity@exscape.org
 * Support for MCP49x2 (MCP4902, MCP4912, MCP4922) added 2012-08-30,
 * with a patch and testing from Jonas Gruska
 
 * Code license: BSD/MIT. Whatever; I *prefer* to be credited,
 * and you're *not* allowed to claim you wrote this from scratch.
 * Apart from that, do whatever you feel like. Commercial projects,
 * code modifications, etc.

 Pins used: 
 * Arduino pin 11 (for Uno; for Mega: 51) to device SDI (pin 4) - fixed pin
 * Arduino pin 13 (for Uno; for Mega: 52) to device SCK (pin 3) - fixed pin
 * Any digital pin to device LDAC (DAC pin 5)  (except with PortWrite, see README)
 * Any digital pin to device CS   (DAC pin 2)  (as above)
 *
 * Other DAC wirings:  
 * Pin 1: VDD, to +5 V
 * Pin 5: LDAC, either to an Arduino pin, or ground to update vout automatically
 * Pin 6: VREF, to +5 V (or some other reference voltage 0 < VREF <= VDD)
 * Pin 7: VSS, to ground
 * Pin 8: vout
 * (Pin 9, for the DFN package only: VSS)

 * Only tested on MCP4901 (8-bit) and MCP4922 (dual 12-bit), but it should work on the others as well.
 * Tested on an Arduino Uno R3.
 */

// #include <SPI.h>
#include "DAC_MCP49xx.h"
#include <stdio.h>

DAC_MCP49xx::DAC_MCP49xx(DAC_MCP49xx::Model _model, int _ss_pin, int _LDAC_pin)
{
}

void DAC_MCP49xx::setBuffer(boolean _buffer)
{
}

void DAC_MCP49xx::setPortWrite(boolean _port_write)
{
}


// Only relevant for the MCP49x2 dual DACs.
// If set, calling output2() will pull the LDAC pin low automatically,
// which causes the output to change.
// Not required if the LDAC pin is shorted to ground, however in that case,
// there will be a delay between the updating of channel A and channel B.
// If sync is desired, wire the LDAC pin to the Arduino and set this to true.
boolean DAC_MCP49xx::setAutomaticallyLatchDual(bool _latch) {
  return true;
}

// Sets the gain. These DACs support 1x and 2x gain.
// vout = x/2^n * gain * VREF, where x = the argument to out(), n = number of DAC bits
// Example: with 1x gain, set(100) on a 8-bit (256-step) DAC would give
// an output voltage of 100/256 * VREF, while a gain of 2x would give
// vout = 100/256 * VREF * 2
boolean DAC_MCP49xx::setGain(int _gain) {
  return true;
}

// Sets the SPI clock frequency. The final frequency will be the 
// main Arduino clock divided by the divider selected here.
boolean DAC_MCP49xx::setSPIDivider(int _div) {
  return true;
}

// Shuts the DAC down. Shutdown current is about 1/50 (typical) of active mode current.
// My measurements say ~160-180 µA active (unloaded vout), ~3.5 µA shutdown.
// Time to settle on an output value increases from ~4.5 µs to ~10 µs, though (according to the datasheet).
void DAC_MCP49xx::shutdown(void) {
}

// For MCP49x1
void DAC_MCP49xx::output(unsigned short data) {
}

// For MCP49x2
void DAC_MCP49xx::outputA(unsigned short data) {
  // printf("OutputA: %d\n", data);
}

// For MCP49x2
void DAC_MCP49xx::outputB(unsigned short data) {
    // printf("OutputB: %d\n", data);
}

// MCP49x2 (dual DAC) only.
// Send a set of new values for the DAC to output in a single function call
void DAC_MCP49xx::output2(unsigned short data_A, unsigned short data_B) {

}

// These DACs have a function where you can change multiple DACs at the same
// time: you call output() "sequentially", one DAC at a time, and *then*,
// when they've all received the output data, pull the LDAC pin low on
// all DACs at once. This function pulls the LDAC pin low for long enough 
// for the DAC(s) to change the output.
// If this function is undesired, you can simply tie the LDAC pin to ground.
// When tied to ground, you need *NOT* call this function!
void DAC_MCP49xx::latch(void) {

}
