# Steering Controller Revision 0 Test Plan

The tests for the Steering Control module are split into three parts: Bench,
installation and integration testing. The bench tests ensure the module itself functions as expected
and the installation tests ensure the module is integrated correctly.

## Microcontroller Testing

1. Supply a stable voltage to the microcontroller via USB and check the
Arduino using the
blink tutorial.

2. Confirm the correct voltages at the supply voltage pins (0V, 3.3V and 5V)
using a digital multi-meter thus checking the voltage level converter.

3. Attach the Steering Control board to the Arduino, supply 12 volts to the
module using the screw terminals and verify the blink tutorial runs.

4. Upload the test firmware to Arduino and connect the test harness. Run the
interactive tests which test:
  - Power
  - DAC
  - Signal interrupt switch relay
  - CAN send/receive



## Installation Testing
These tests are to be run while the Throttle Control module is connected to a
computer via a USB cable. These tests are to be run with the vehicle in park.

1. Install the Throttle Control module into the vehicle.

2. Initiate the in-vehicle test routine to test:
 - Power
 - Signal interrupt switching
 - Throttle spoofing
 - Throttle sensing

3. Upload the Steering Control firmware.

4. Using a laptop, command the vehicle to accelerate. Pressed the
accelerator pedal and ensure that the throttle module disables control.

5. While the control modules is accelerating the vehicle,
press the emergency stop button and check that normal throttle function is restored.

## Integration Testing
These tests are to be run with the vehicle in park. All other modules should be
installed in the vehicle and connected via the Control CAN bus.

1. Using the joystick commander utility, enable control and accelerate the vehicle.

2. Quit the joystick commander utility and check that the throttle module disables control.

3. Using the joystick commander utility, enable control and accelerate the vehicle.
While accelerating, press the soft-stop button and check that normal throttle function is restored.
