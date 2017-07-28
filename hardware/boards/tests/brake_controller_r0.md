# Brake Controller Revision 0 Test Plan

The tests for the Brake Control module are split into three parts: Bench,
installation and integration testing. The bench tests ensure the module itself functions as expected
and the installation tests ensure the module is integrated correctly.

## Microcontroller Testing

1. Supply a stable voltage to the microcontroller via USB and check the Arduino using the
blink tutorial.

2. Confirm the correct voltages at the supply voltage pins (0V, 3.3V and 5V)
using a digital multi-meter thus checking the voltage level converter.

3. Attach the Brake Control board to the Arduino, supply 12 volts to the
module using the screw terminals and verify the blink tutorial runs.

4. Upload the test firmware to Arduino and connect the test harness. Run the
interactive tests which test:
  - Power
  - PWM drivers
  - Pump relay
  - Brake light switch relay
  - CAN send/receive



## Installation Testing
These tests are to be run while the Brake Control module is connected to a computer
via a USB cable. These tests are to be run with the vehicle off the ground
so that the wheels can turn freely.

1. Install the Brake Control module into the vehicle.

2. Initiate the in-vehicle test routine to test:
 - Power
 - Accumulator pressure
 - Brake pressure control
 - Brake pedal input sensing

3. Upload the Brake control firmware.

4. Using a laptop, command the vehicle to brake. Press on the brake pedal and ensure
that the brake module disables control.

5. While the control modules is braking the vehicle, press the emergency stop button and
check that normal braking is possible.

## Integration Testing
These tests are to be run with the vehicle off the ground so that the wheels can
turn freely. All other modules should be installed in the vehicle and
connected via the Control CAN bus.

1. Using the joystick commander utility, enable control and brake the vehicle.

2. Quit the joystick commander utility and check that the brake module disables control.

3. Using the joystick commander utility, enable control and brake the vehicle.
While braking, press the soft-stop button and check that normal braking is possible.
