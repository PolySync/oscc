Arduino library for the Microchip MCP49x1/MCP49x2 series DACs
Thomas Backman <serenity@exscape.org>, 2012

Version 1.1 (October 2, 2012)
Changes from v1.0:
* Support for MCP49x2 series DACs added; thanks to Jonas Gruska for idea/code/testing
* Library renamed from DAC_MCP49x1 to DAC_MCP49xx

Supported models:
* MCP4901 (8-bit, single)
* MCP4911 (10-bit, single)
* MCP4921 (12-bit, single)
* MCP4902 (8-bit, dual)
* MCP4912 (10-bit, dual)
* MCP4922 (12-bit, dual)

Example usage:

/////////////////////////////////

#include <SPI.h>         // Remember this line!
#include <DAC_MCP49xx.h>

DAC_MCP49xx dac(DAC_MCP49xx::MCP4901, 10, 7); // DAC model, SS pin, LDAC pin

void setup() { } // No setup code is necessary

void loop() {
  dac.output(255);
  delay(2500);
  dac.output(0);
  delay(2500);
}

/////////////////////////////////

See the example sketches for slightly more detailed examples.

A note on latching/LDAC:
	The way these DACs work is as follows: when a value is sent to them via
	the output*() methods, the output does not instantly change.
	Instead, the DAC waits until the LDAC pin has been pulled low. Therefore,
	if that pin is tied to ground, the output *will* instantly change.
	However, if that pin is connected to the Arduino, you will need to latch
	the output in order for a change to happen. There are two ways to do this:
	1) Call latch(), which creates a low pulse
	2) MCP49x2 only: call output2(), with the automaticallyLatchDual option set
	   (true by default), which will then call latch() for you

	The main usage of this functionality is to synchronize multiple DACs (2 or more),
	to make their outputs change at the same moment, despite having the data sent
	to them one at a time.

Simple function overview:

Constructor (DAC_MCP49xx, int SS_pin, int LDAC_pin = -1)
	Takes three arguments: the DAC model (MCP4901, MCP4911, MCP4921,
	MCP4902, MCP4912 or MCP4922), the chip select pin, and the LDAC pin.
	If the LDAC functionality isn't wanted (if you don't know, you likely don't
	want it), make sure the LDAC pin is tied to ground, and simply ignore that
	argument (or pass -1).

setBuffer(bool)
	Should the DAC buffer the reference voltage?
	Note: if "true", VREF cannot be VDD (e.g. +5 V). Defaults to false.

setGain(int)
	Sets the gain. The output voltage is x/2^n * VREF * gain, where x = the value
	passed to out and n = the number of bits
	For example, for a 8-bit DAC with gain 2x and VREF = 5 V where out(100) is
	called, vout = 100/256 * 5 * 2 = 3.90625 V.
	Only 1 and 2 are valid values!
	Defaults to 1x.

setSPIDivider(int)
	Sets the SPI clock frequency. See the Arduino docs: http://arduino.cc/en/Reference/SPISetClockDivider
	Uses the same constants (SPI_CLOCK_DIV2 etc.) as in the Arduino docs.

setAutomaticallyLatchDual(bool)
	If true: will automatically pull the LDAC pin low after output2() is called.
	If false: won't do the above.
	Only relevant for the MCP49x2 dual DACs.
	Defaults to true.

shutdown(void)
	Shuts the DAC down to save power. (~3.5 µA in standby, ~150 µA powered up.)
	Settling time after calling set() increases from ~5 to ~10 µs when in standby.

output(unsigned short)
	MCP49x1 (single DAC) only.
	Sends the value to the DAC. If the LDAC pin is tied to ground, Vout will change "instantly".
	If the LDAC pin is connected to the Arduino, nothing will change until latch() is called.

outputA(unsigned short)
	MCP49x2 (dual DAC) only.
	Sends the value to DAC A. Same as output().

outputB(unsigned short)
	MCP49x2 (dual DAC) only.
	Sends the value to DAC B. Otherwise same as output().

output2(unsigned short, unsigned short)
	MCP49x2 (dual DAC) only.
	Sends the two output values to the two outputs of the dual DACs.
	Latches the output automatically unless setAutomaticallyLatchDual() has been
	called with 'false' as the argument.

latch(void)
	Changes the output voltage based on the last value(s) sent using the output*() methods.
	Creates a low pulse on the DAC's LDAC pin (pin 5 on the DAC chip).
	The LDAC pin can be tied to ground instead, to update automatically ASAP, in
	which case this method does nothing.

setPortWrite(bool)
	Note: PortWrite is not supported on the Arduino Leonardo, but should work on
	the Uno and Mega. (One could change this by changing the ports used in the code.)
	*** The CS and LDAC pins set in the constructor are IGNORED when this is set to true!!! ***

	PortWrite is a method that speeds up the code significantly, but only works on fixed pins (see below).
	It's roughly 30 times faster than digitalWrite in my measurements for this code.
	(That doesn't mean that the entire code is 30 times faster, but changing a pin value is.)

	In order to use the faster method e.g. on the Arduino Uno, you must use pin 7 on the Uno for the LDAC pin, 
	and pin 10 for the CS pin. Anything else won't work. If these pins are busy doing something else, you can't use this speed-up
	without modifying the code.

	Use these pins:

	For the LDAC pin: PD7 (Uno: digital pin 7, Mega: digital pin 38)
	For the CS pin: PB2 (Uno: digital pin 10, Mega: digital pin 51)

	Also note that since these are fixed, and CS cannot be shared between devices, *you can't use multiple DACs with PortWrite enabled*!

	So, in short...
	Pro: much faster (only matters for relatively high-frequency stuff;
	the difference is on the order of a few microseconds)
	Cons: only works on fixed pins, only works with 1 DAC, doesn't work on all Arduinos
