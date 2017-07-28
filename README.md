<img src="https://raw.githubusercontent.com/wiki/PolySync/OSCC/images/oscc_logo_title.png">


The Open Source Car Control project is a hardware and software project that faciliates conversion of a
late model vehicle into an autonomous driving R&D machine.

OSCC enables developers to intercept messages from the car's on-board OBD-II CAN network, forwarding reports on the states of various vehicle components, like steering angle or wheel speeds, into your application. After you've used this data in your algorithm, you can then use our API to send spoofed commands back into the car's ECUs. OSCC provides a modular and stable way of using software to interface with a vehicle's communications network and electrical system.

Although we currently support late model Kia Souls, the API and firmware have been designed to make it easy to add new vehicle details as the specific details of other systems are determined. Additionally, the seperation between API and firmware means it is easier to modify and test parts of your program without having to update the flashed modules.

Our [Wiki](https://github.com/PolySync/OSCC/wiki) is in the process of being updated to reflect the new changes, but contains a bunch of valuable information to help you get started in understanding the details of the system.


## Repository Contents

* **api** - Software API, so your program can seamlessly talk to our modules.
* **firmware** - Arduino libraries and firmware for the OSCC modules.
* **hardware** - PCB schematics and board designs for control modules.


## Boards

The sensor interface and actuator control board schematics and design files are located in the
`boards` directory. If you don't have the time or fabrication resources, the boards can be
purchased as a kit from the [OSCC website](http://oscc.io).

Thanks to [Trey German](https://www.polymorphiclabs.com) and [Macrofab](https://macrofab.com/) for
help designing and manufacturing the custom boards.

## Versions

It's important that the correct version of the firmware is used with the
correct versions of the module boards. As the boards are updated with additional
pins and other features, the firmware is modified accordingly to use them.
Mismatched versions will cause problems.

*Your hardware version is printed on the front of the OSCC shield.*

Consult the following table for version compatibility.

| Actuator Board | Firmware  |
| -------------- | --------  |
| >= v1.1.0      | >= v0.7   |

| Sensor Interface Board | Firmware  |
| ---------------------- | --------- |
| >= v1.1.0              | >= v0.7   |



# Building and Uploading Firmware

The OSCC Project is built around a number of individual firmware modules that interoperate to allow communication with your vehicle.
These modules are built from Arduinos and Arduino shields designed specifically for interfacing with various vehicle components.
Once these modules have been installed in the vehicle and flashed with the firmware, the API can be used to
recieve reports from the car and send spoofed commands.

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

To generate Makefiles, tell CMake which platform to build firmware for. For example, if you want to build
firmware for the Kia Soul:

```
cmake .. -DKIA_SOUL=ON
```

By default, your firmware will have debug symbols which is good for debugging but increases
the size of the firmware significantly. To compile without debug symbols and optimizatons
enabled, use the following instead:

```
cmake .. -DKIA_SOUL=ON -DCMAKE_BUILD_TYPE=Release
```

This will generate the necessary files for building.

Now you can build all of the firmware with `make`:

```
make
```

If you'd like to build only a specific module, you can provide a target name to
`make` for whichever module you'd like to build:

```
make brake
make gateway
make steering
make throttle
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
make throttle-upload
```

However, if you want to flash all modules, you need to change the ports in
CMake for each module to match what they are on your machine. The easiest way
is to connect each module in alphabetical order (brake, CAN gateway, steering,
throttle) so that they are assigned `/dev/ttyACM0` through `/dev/ttyACM3` in
a known order. You can then change the ports during the `cmake ..` step:

```
cmake .. -DKIA_SOUL=ON -DSERIAL_PORT_BRAKE=/dev/ttyACM0 -DSERIAL_PORT_CAN_GATEWAY=/dev/ttyACM1 -DSERIAL_PORT_STEERING=/dev/ttyACM2 -DSERIAL_PORT_THROTTLE=/dev/ttyACM3
```

Then you can flash all with one command:

```
make all-upload
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
make brake-monitor
make brake-monitor-log
```

You can exit `screen` with `C-a \`.

To do more in-depth debugging you can use any of a number of serial monitoring applications.
Processing can be used quite effectively to provide output plots of data incoming across a serial
connection.

Be aware that using serial printing can affect the timing of the firmware. You may experience
strange behavior while printing that does not occur otherwise.


## Tests

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

### Unit Tests

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
make run-brake-unit-tests
make run-can-gateway-unit-tests
make run-steering-unit-tests
make run-throttle-unit-tests
```

Or run only the tests of a single platform:

```
make run-unit-tests
```

If everything works correctly you should see something like this:

```
# language: en
Feature: Receiving commands

  Commands received from a controller should be processed and acted upon.

  Scenario Outline: Enable throttle command sent from controller        # firmware/throttle/tests/features/receiving_commands.feature:8
    Given throttle control is disabled                                  # firmware/throttle/tests/features/receiving_commands.feature:9
    And the accelerator position sensors have a reading of <sensor_val> # firmware/throttle/tests/features/receiving_commands.feature:10
    When an enable throttle command is received                         # firmware/throttle/tests/features/receiving_commands.feature:12
    Then control should be enabled                                      # firmware/throttle/tests/features/receiving_commands.feature:14
    And the last command timestamp should be set                        # firmware/throttle/tests/features/receiving_commands.feature:15
    And <dac_a_val> should be written to DAC A                          # firmware/throttle/tests/features/receiving_commands.feature:16
    And <dac_b_val> should be written to DAC B                          # firmware/throttle/tests/features/receiving_commands.feature:17

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
make run-brake-property-tests
make run-steering-property-tests
make run-throttle-property-tests
make run-pid-library-property-tests
```

Or run only the tests of a single platform:

```
make run-property-tests
```

Once the tests have completed, the output should look similar to the following:

```
running 7 tests
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

### Run All Tests

Finally, you can run all available tests:

```
make run-all-tests
```


## Easier CMake Configuration

If you have a lot of `-D` commands to pass to CMake (e.g., configuring the serial
port and baud rates of all of the modules), you can instead configure with a GUI
using `cmake-gui`:

```
sudo apt install cmake-gui
```

Then use `cmake-gui` where you would normally use `cmake`:

```
cd firmware
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

# Controlling Your Vehicle - an Example Application

Now that all your Arduino modules are properly setup, it is time to start sending control commands.
We've created an example application, joystick commander, that uses the OSCC API to interface with the firmware, allowing you to send commands using a game controller and receive reports from the on-board OBD-II CAN. These commands are converted into CAN messages, which the OSCC API sends to the respective Arduino modules and are used to actuate the vehicle.

[OSCC Joystick Commander](https://github.com/PolySync/oscc-joystick-commander)

# OSCC API

**Use provided CAN channel to open and close communications to CAN bus connected to the OSCC modules.**

```c
oscc_error_t oscc_open( uint channel )
oscc_error_t oscc_close( uint )
```

These methods are the start and end points of using the OSCC API in your application. ```oscc_open``` will open a socket connection
on the specified CAN channel, enabling it to quickly recieve reports from and send commands to the firmware modules.
When you are ready to terminate your application, ```oscc_close``` can terminate the connection.

**Send enable or disable commands to all OSCC modules.**

```c
oscc_error_t oscc_enable( void )
oscc_error_t oscc_disable( void )
```

After you have initialized your CAN connection to the firmware modules, these methods can be used to enable or disable the system. This
allows your application to choose when to enable sending commands to the firmware. Although you can only send commands when the system is
enabled, you can recieve reports at any time.

**Publish message with requested normalized value to the corresponding module.**

```c
oscc_error_t publish_brake_position( double normalized_position )
oscc_error_t publish_steering_torque( double normalized_torque )
oscc_error_t publish_throttle_position( double normalized_position )
```

These commands will forward a double value, *[0.0, 1.0]*, to the specified firmware module. The API will construct the appropriate values
to send spoof commands into the vehicle ECU's to achieve the desired state. The API also contains safety checks to ensure no invalid values
can be written onto the hardware.

**Register callback function to be called when OBD message received from vehicle.**

```c
oscc_error_t subscribe_to_brake_reports( void(*callback)(oscc_brake_report_s *report)  )
oscc_error_t subscribe_to_steering_reports( void(*callback)(oscc_steering_report_s *report) )
oscc_error_t subscribe_to_throttle_reports( void(*callback)(oscc_throttle_report_s *report) )
oscc_error_t subscribe_to_fault_reports( void(*callback)(oscc_fault_report_s *report) )
oscc_error_t subscribe_to_obd_messages( void(*callback)(struct can_frame *frame) )
```

In order to recieve reports from the modules, your application will need to register a callback handler with the OSCC API.
When the appropriate report for your callback function is recieved from the API's socket connection, it will then forward the
report to your software.

In addition to OSCC specific reports, it will also forward any non-OSCC reports to any callback function registered with
```subscribe_to_obd_messages```. This can be used to view CAN frames recieved from the vehicle's OBD-II CAN channel. If you know
the corresponding CAN frame's id, you can parse reports sent from the car.

# Additional Vehicles & Contributing

OSCC currently has information regarding the Kia Soul PS (2014-2016), but we want to grow! The
repository is structured to facilitate including more vehicles as more is learned about them.

In order to include information related to a new vehicle's specification, follow the format defined in ```api/include/vehicles/kia_soul.h``` and
add a CMake option to choose your new header when compiling the API.

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
