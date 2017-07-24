<img src="https://raw.githubusercontent.com/wiki/PolySync/OSCC/images/oscc_logo_title.png">


The Open Source Car Control Project is a hardware and software project detailing the conversion of a
late model vehicle into an autonomous driving research and development vehicle.

See the [Wiki](https://github.com/PolySync/OSCC/wiki) for full documentation, details, and other
information.


# Versions

It's important that the correct version of the firmware is used with the
correct versions of the module boards. As the boards are updated with additional
pins and other features, the firmware is modified accordingly to use them.
Mismatched versions will cause problems.

Consult the following table for version compatibility.

| Actuator Board | Firmware  |
| -------------- | --------  |
| >= v1.1.0      | >= v0.7   |

| Sensor Interface Board | Firmware  |
| ---------------------- | --------- |
| >= v1.1.0              | >= v0.7   |


# Repository Contents

* **api** - API used by applications to interface with the modules
* **applications** - Applications to control and interface with the modules
* **firmware** - Arduino code for each of the modules
* **hardware** - Technical drawings, 3D files, and PCB schematics and board designs


# Boards

The sensor interface and actuator control board schematics and design files are located in the
`hardware/boards` directory. If you don't have the time or fabrication resources, the boards can be
purchased as a kit from the [OSCC website](http://oscc.io).

Thanks to [Trey German](https://www.polymorphiclabs.com) and [Macrofab](https://macrofab.com/) for
help designing the boards and getting the boards made.


# Building and Uploading Arduino Firmware

The OSCC Project is built around a number of individual modules that interoperate to create a fully
controllable vehicle. These modules are built from Arduinos and Arduino shields designed specifically
for interfacing with various vehicle components. Once these modules have been programmed with the
accompanying firmware and installed into the vehicle, the vehicle is ready to receive control commands
sent over a CAN bus from a computer running a control program.

## Pre-requisites

You must have Arduino Core and CMake (version 2.8 or greater) installed on
your machine.

```
sudo apt install arduino-core cmake
```

OSCC uses CMake to avoid some of the limitations of the Arduino IDE. Using this method you can build
and upload the firmware from the command-line.

Check out [Arduino CMake](https://github.com/queezythegreat/arduino-cmake) for more information.

## Building the Firmware

Navigate to the `firmware` directory and create a build directory inside of it:

```
cd firmware
mkdir build
cd build
```

To generate Makefiles, tell CMake which platform to build firmware for. If you want to build
the firmware for the Kia Soul:

```
cmake .. -DBUILD_KIA_SOUL=ON
```

By default, your firmware will have debug symbols which is good for debugging but increases
the size of the firmware significantly. To compile without debug symbols and optimizations
enabled, use the following instead:

```
cmake .. -DBUILD_KIA_SOUL=ON -DCMAKE_BUILD_TYPE=Release
```

This will generate the necessary files for building.

Now you can build all of the firmware with `make`:

```
make
```

If you'd like to build only a specific module, you can provide a target name to
`make` for whichever module you'd like to build:

```
make kia-soul-brake
make kia-soul-can-gateway
make kia-soul-steering
make kia-soul-throttle
```

## Uploading the Firmware

Once the firmware is successfully built, you can upload it. When you connect to
an Arduino with a USB cable, your machine assigns a serial device to it with the
path `/dev/ttyACM#` where `#` is a digit starting at 0 and increasing by one with
each additional Arduino connected.

You can upload firmware to a single module or to all modules. By default, CMake
is configured to expect each module to be `/dev/ttyACM0`, so if you connect a
single module to your machine, you can flash it without changing anything:

```
make kia-soul-throttle-upload
```

However, if you want to flash all modules, you need to change the ports in
CMake for each module to match what they are on your machine. The easiest way
is to connect each module in alphabetical order (brake, CAN gateway, steering,
throttle) so that they are assigned `/dev/ttyACM0` through `/dev/ttyACM3` in
a known order. You can then change the ports during the `cmake ..` step:

```
cmake .. -DBUILD_KIA_SOUL=ON -DSERIAL_PORT_BRAKE=/dev/ttyACM0 -DSERIAL_PORT_CAN_GATEWAY=/dev/ttyACM1 -DSERIAL_PORT_STEERING=/dev/ttyACM2 -DSERIAL_PORT_THROTTLE=/dev/ttyACM3
```

Then you can flash all with one command:

```
make kia-soul-all-upload
```

Sometimes it takes a little while for the Arduino to initialize once connected, so if there is an
error thrown initially, wait a little bit and then retry the command.

## Monitoring Arduino Modules

It is sometimes useful to monitor individual Arduino modules to check for proper operation and to
debug. If the modules have been built with the flag `-DCMAKE_BUILD_TYPE=Debug`, their debug
printing functionality will be enabled and they will print status information to the serial interface.

The GNU utility `screen` is one option to communicate with the Arduino via serial over USB. It can
be used to both receive the output of any `Serial.print` statements in your Arduino code, and to
push commands over serial to the Arduino. If you don't already have it installed,
you can get it with the following command:

```
sudo apt install screen
```

You need to enable debug mode with `-DDEBUG=ON` and tell CMake what serial port
the module you want to monitor is connected to
(see [section on uploading](#uploading-the-firmware) for details on the default
ports for each module). The default baud rate is `115200` but you can change it:

```
cmake .. -DBUILD_KIA_SOUL=ON -DDEBUG=ON -DSERIAL_PORT_THROTTLE=/dev/ttyACM0 -DSERIAL_BAUD_THROTTLE=19200
```

You can use a module's `monitor` target to automatically run `screen`, or a
module's `monitor-log` target to run `screen` and output to a file called
`screenlog.0` in your current directory:

```
make kia-soul-brake-monitor
make kia-soul-brake-monitor-log
```

You can exit `screen` with `C-a \`.

To do more in-depth debugging you can use any of a number of serial monitoring applications.
Processing can be used quite effectively to provide output plots of data incoming across a serial
connection.

Be aware that using serial printing can affect the timing of the firmware. You may experience
strange behavior while printing that does not occur otherwise.


# Tests

There are two types of tests available: unit and property-based.

Building and running the tests is similar to the firmware itself, but you must instead tell
CMake to build the tests instead of the firmware with the `-DTESTS=ON` flag. We also pass
the `-DCMAKE_BUILD_TYPE=Release` flag so that CMake will disable debug symbols and enable
optimizations, good things to do when running tests to ensure nothing breaks with
optimizations.

```
cd firmware
mkdir build
cd build
cmake .. -DTESTS=ON -DCMAKE_BUILD_TYPE=Release
```

## Unit Tests

Each module has a suite of unit tests that use **Cucumber** with **Cgreen**. There are prebuilt
64-bit Linux versions in `firmware/common/testing/framework`. Boost is required for Cucumber-CPP
and has been statically linked into `libcucumber-cpp.a`. If you need to build your own versions
you can use the provided script `build_test_framework.sh` which will install the Boost dependencies
(needed for building), clone the needed repositories with specific hashes, build the Cgreen and
Cucumber-CPP libraries, and place static Boost in the Cucumber-CPP library. The built will be placed
in an `oscc_test_framework` directory in the directory that you ran the script from. You can then copy
`oscc_test_framework/cucumber-cpp` and `oscc_test_framework/cgreen` to
`firmware/common/testing/framework`.

You must have **Cucumber** installed to run the tests:

```
sudo apt install ruby-dev
sudo gem install cucumber -v 2.0.0
```

We can run all of the unit tests available:

```
make run-unit-tests
```

Each module's test can also be run individually:

```
make run-kia-soul-brake-unit-tests
make run-kia-soul-can-gateway-unit-tests
make run-kia-soul-steering-unit-tests
make run-kia-soul-throttle-unit-tests
```

Or run only the tests of a single platform:

```
make run-kia-soul-unit-tests
```

If everything works correctly you should see something like this:

```
# language: en
Feature: Receiving commands

  Commands received from a controller should be processed and acted upon.

  Scenario Outline: Enable throttle command sent from controller        # firmware/kia_soul/throttle/tests/features/receiving_commands.feature:8
    Given throttle control is disabled                                  # firmware/kia_soul/throttle/tests/features/receiving_commands.feature:9
    And the accelerator position sensors have a reading of <sensor_val> # firmware/kia_soul/throttle/tests/features/receiving_commands.feature:10
    When an enable throttle command is received                         # firmware/kia_soul/throttle/tests/features/receiving_commands.feature:12
    Then control should be enabled                                      # firmware/kia_soul/throttle/tests/features/receiving_commands.feature:14
    And the last command timestamp should be set                        # firmware/kia_soul/throttle/tests/features/receiving_commands.feature:15
    And <dac_a_val> should be written to DAC A                          # firmware/kia_soul/throttle/tests/features/receiving_commands.feature:16
    And <dac_b_val> should be written to DAC B                          # firmware/kia_soul/throttle/tests/features/receiving_commands.feature:17

    Examples:
      | sensor_val | dac_a_val | dac_b_val |
      | 0          | 0         | 0         |
      | 256        | 1024      | 1024      |
      | 512        | 2048      | 2048      |
      | 1024       | 4096      | 4096      |
```

## Property-Based Tests

The throttle, steering, and brake modules, along with the PID controller library, also contain a series of
property-based tests.

These tests use [QuickCheck for Rust](http://burntsushi.net/rustdoc/quickcheck/), so **Rust** and **Cargo**
need to be [installed](https://www.rust-lang.org/en-US/install.html) in order to run them locally.


We can run all of the property-based tests available:

```
make run-property-tests
```

Each module's test can also be run individually:

```
make run-kia-soul-brake-property-tests
make run-kia-soul-steering-property-tests
make run-kia-soul-throttle-property-tests
make run-pid-library-property-tests
```

Or run only the tests of a single platform:

```
make run-kia-soul-property-tests
```

Once the tests have completed, the output should look similar to the following:

```
running 7 tests
test bindgen_test_layout_pid_s ... ok
test check_integral_term ... ok
test check_derivative_term ... ok
test check_proportional_term ... ok
test check_reversed_inputs ... ok
test check_same_control_for_same_inputs ... ok
test check_zeroize ... ok

test result: ok. 7 passed; 0 failed; 0 ignored; 0 measured

   Doc-tests tests

running 0 tests

test result: ok. 0 passed; 0 failed; 0 ignored; 0 measured
```

## Run All Tests

Finally, you can run all available tests:

```
make run-all-tests
```


# Easier CMake Configuration

If you have a lot of `-D` commands to pass to CMake (e.g., configuring the serial
port and baud rates of all of the modules), you can instead configure with a GUI
using `cmake-gui`:

```
sudo apt install cmake-gui
```

Then use `cmake-gui` where you would normally use `cmake`:

```
cd platforms
mkdir build
cd build
cmake-gui ..
```

The GUI will open and you can change all of the options you would normally need
to pass on the command line. First, press the `Configure` button and then press
`Finish` on the dialog that opens. In the main window you'll see a list of
options that you can change that would normally be configured on the command line
with `-D` commands. When you're done, click `Configure` again and then click
the `Generate` button. You can then close `cmake-gui` and run any `make` commands
like you normally would.


# Controlling Your Vehicle

Now that all your Arduino modules are properly setup, it is time to start sending control commands.
There is an example application to do this that uses a gamepad. The example interfaces to the 
joystick gamepad via the SDL2 game controller library and sends CAN commands over the control CAN bus
via the Kvaser CANlib SDK. These CAN control commands are interpreted by the respective Arduino
modules and used to actuate the vehicle. This application has been tested with a Logitech F310 gamepad
and a wired Xbox 360 controller, but should work with any SDL2 supported game controller. Controllers 
with rumble capabilities will provide feedback when OSCC is enabled or disabled. 

## Pre-requisites:

An SDL2 supported gamepad is required, and the SDL2 library and CANlib SDK need to
be pre-installed. A CAN interface adapter, such as the [Kvaser Leaf Light](https://www.kvaser.com),
is also required.

[Xbox 360 Wired Controller](https://www.amazon.com/dp/B004QRKWLA)
[logitech-F310](http://a.co/3GoUlkN)

Install the SDL2 library with the command below.

```
sudo apt install libsdl2-dev
```

## Building Joystick Commander

Navigate to the directory for the joystick commander code.

```
cd utils/joystick_commander
```

From this directory, run the following sequence to build joystick commander:

```
mkdir build
cd build
cmake ..
make
```

Once you have initialized the CAN interface, you can use the channel number to start joystick commander and begin sending commands to the OSCC modules.

For example, with a Kvaser Leaf Light attached, using a bitrate of 500000:

```
 sudo ip link set can0 type can bitrate 500000
 sudo ip link set up can0
```


You would then run:

```
./joystick-commander 0
```

For more information on setting up a socketcan interface, check out [this guide](http://elinux.org/Bringing_CAN_interface_up).

## Controlling the Vehicle with the Joystick Gamepad

Once the joystick commander is up and running you can use it to send commands to the Arduino modules.
The controls are listed when the programs start up. Be sure the switch on the back of the controller
is switched to the 'X' position, not 'D'. The vehicle will only respond to commands if control is
enabled with the start button. The back button disables control.


# Additional Vehicles & Contributing

OSCC currently has information regarding the Kia Soul PS (2014-2016), but we want to grow! The
repository is structured to facilitate including more vehicles as more is learned about them.

Please see [CONTRIBUTING.md](CONTRIBUTING.md).


# License Information

Hardware source materials (e.g. schematics, board layouts, wiring diagrams, data sheets, physical
installation documentation, 3D models, etc.) for the OSCC (Open Source Car Control) Project are
licensed under Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0).

Firmware  and software source for the OSCC (Open Source Car Control) Project is licensed under the
MIT License (MIT) unless otherwise noted (e.g. 3rd party dependencies, etc.).

Please see [LICENSE.md](LICENSE.md) for more details.


# Contact Information

Please direct questions regarding OSCC and/or licensing to help@polysync.io.

*Distributed as-is; no warranty is given.*

Copyright (c) 2017 PolySync Technologies, Inc.  All Rights Reserved.
