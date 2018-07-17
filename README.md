<img src="https://raw.githubusercontent.com/wiki/PolySync/OSCC/images/oscc_logo_title.png">


Open Source Car Control (OSCC) is an assemblage of software and hardware designs that enable computer control of modern cars in order to facilitate the development of autonomous vehicle technology. It is a modular and stable way of using software to interface with a vehicle’s communications network and control systems.

OSCC enables developers to send control commands to the vehicle, read control messages from the vehicle’s OBD-II CAN network, and forward reports for current vehicle control state. Such as steering angle & wheel speeds. Control commands are issued to the vehicle component ECUs via the steering wheel torque sensor, throttle position sensor, and brake position sensor. (Because the gas-powered Kia Soul isn’t brake by-wire capable, an auxiliary actuator is added to enable braking.) This low-level interface means that OSCC offers full-range control of the vehicle without altering the factory safety-case, spoofing CAN messages, or hacking ADAS features.

Although OSCC currently supports only the 2014 or later Kia Soul (petrol and EV), the API and firmware have been designed to make it easy to add new vehicle support. Additionally, the separation between API and firmware means it is easier to modify and test parts of your program without having to update the flashed OSCC modules.

Our [Wiki](https://github.com/PolySync/OSCC/wiki) is in the process of being updated to reflect the new changes, but contains a bunch of valuable information to help you get started in understanding the details of the system.

**DriveKit:** If you're looking for a turn-key solution, we offer a commercially-supported vehicle control interface called DriveKit. DriveKit is a complete system for by-wire control of Kia Soul EV and Kia Niro Hybrid vehicles. It includes an integrated control module, automotive-grade wiring harnesses, e-stop switch, and additional vehicle control features –– plus it installs in under an hour. **Visit https://polysync.io/drivekit/ for more information.**

# Versions

New versions of the API and the firmware are released periodically as new features are added and bugs are
fixed. **It is of vital importance that you update whenever there is a new version so that you can be certain
you are not using a version with known safety issues.**

There are four versions to be aware of:

* **Sensor Interface Board (throttle and steering):** the version is printed on the front of the shield

* **Vehicle Control Module (EV brakes):** the version is printed on the front of the shield

* **Actuator Control Board (petrol brakes):** the version is printed on the front of the shield

* **API and Firmware:** a single version is shared by both and listed in the [Releases section](https://github.com/PolySync/oscc/releases) of the repository.

The following table can be used to ensure that you use the appropriate firmware version with your boards:

|                        | Board Version | Firmware Version |
| ---------------------- | ------------- | ---------------- |
| Sensor Interface       | >= 1.0.0      | >= 1.0.0         |
| Vehicle Control        | 0.1.0         | >= 1.1.0         |
| Actuator Control       | >= 1.2.0      | >= 1.0.0         |
|                        | < 1.2.0 __*__ | >= 1.0.1         |

__*__ *Later versions of the Actuator Control Board utilize new pins to perform additional safety checks on startup. To use the new firmware on older boards, please see additional instructions in the [build](#brake-startup-test) section.*

# Repository Contents

* **api** - Software API, so your program can seamlessly talk to our modules.
* **firmware** - Arduino libraries and firmware for the OSCC modules.
* **hardware** - PCB schematics and board designs for control modules.

# Boards

The sensor interface and actuator control board schematics and design files are located in the
`hardware/boards` directory.

Thanks to [Trey German](https://www.polymorphiclabs.com) for
help designing the boards.

# Building and Uploading Firmware

The OSCC Project is built around a number of individual firmware modules that inter-operate to allow communication with your vehicle.
These modules are built from Arduinos and Arduino shields designed specifically for interfacing with various vehicle components.
Once these modules have been installed in the vehicle and flashed with the firmware, the API can be used to
receive reports from the car and send spoofed commands.

## Pre-requisites

You must have Arduino Core (version 1.8.5 or greater) and CMake (version 2.8 or greater) installed on
your machine.

```
sudo apt install build-essential cmake
```

A manual Arduino install is required since the debian package is older than 1.8.5
```
sudo apt-get purge arduino arduino-core
wget http://arduino.cc/download.php?f=/arduino-1.8.5-linux64.tar.xz -O arduino-1.8.5.tar.xz
tar -xf arduino-1.8.5.tar.xz
cd arduino-1.8.5
sudo mkdir -p /usr/share/arduino
sudo cp -R * /usr/share/arduino
```

OSCC uses CMake to avoid some of the limitations of the Arduino IDE. Using this method you can build
and upload the firmware from the command-line.

Check out [Arduino CMake](https://github.com/arduino-cmake/arduino-cmake) for more information.

## Building the Firmware

Navigate to the `firmware` directory and create a build directory inside of it:

```
cd firmware
mkdir build
cd build
```

To generate Makefiles, tell `cmake` which vehicle to build for by supplying the
appropriate build flag:

| Vehicle         | Flag                   |
| --------------- | ---------------------- |
| Kia Soul Petrol | -DVEHICLE=kia_soul    |
| Kia Soul EV     | -DVEHICLE=kia_soul_ev |
| Kia Niro        | -DVEHICLE=kia_niro    |


For example, if you want to build firmware for the petrol Kia Soul:

```
cmake .. -DVEHICLE=kia_soul
```

**Operator Overrides: While all OSCC modules have operator override detection enabled by default, attempting to grab the steering wheel while the system is active could result in serious injury. The preferred method of operator override for steering is to utilize the brake pedal or E-stop button. To disable operator override for the steering module, pass an additional flag to the CMake build step.**

```
cmake .. -DVEHICLE=kia_soul -DSTEERING_OVERRIDE=OFF
```

If steering operator overrides remain enabled, the sensitivity can be adjusted by changing the value of the `TORQUE_DIFFERENCE_OVERRIDE_THRESHOLD` in the corresponding vehicle's header file.

* Lowering this value will make the steering module more sensitive to operator override, but will result in false positives around high-torque areas, such as the mechanical limits of the steering rack or when quickly and rapidly changing direction.
* Increasing this value will result in fewer false positives, but will make it more difficult to manually override the wheel.

By default, your firmware will have debug symbols which is good for debugging but increases
the size of the firmware significantly. To compile without debug symbols and optimizations
enabled, use the following instead:

```
cmake .. -DVEHICLE=kia_soul -DCMAKE_BUILD_TYPE=Release
```

<a name="brake-startup-test"></a>
**NOTE:**
> For older (< 1.2.0) versions of the actuator control board, you need to set an additional flag using `cmake .. -DKIA_SOUL=ON -DBRAKE_STARTUP_TEST=OFF` to disable startup tests that are not compatible with the previous board design.

This will generate the necessary files for building.

Now you can build all of the firmware with `make`:

```
make
```

If you'd like to build only a specific module, you can provide a target name to
`make` for whichever module you'd like to build:

```
make brake
make can-gateway
make steering
make throttle
```

## Uploading the Firmware

Once the firmware is successfully built, you can upload it. When you connect to
an Arduino with a USB cable, your machine assigns a serial device to it with the
path `/dev/ttyACM#` where `#` is a digit starting at 0 and increasing by one with
each additional Arduino connected.

You can upload firmware to a single module or to all modules. By default, `cmake`
is configured to expect each module to be `/dev/ttyACM0`, so if you connect a
single module to your machine, you can flash it without changing anything:

```
make throttle-upload
```

However, if you want to flash all modules, you need to change the ports in
`cmake` for each module to match what they are on your machine. The easiest way
is to connect each module in alphabetical order (brake, CAN gateway, steering,
throttle) so that they are assigned `/dev/ttyACM0` through `/dev/ttyACM3` in
a known order. You can then change the ports during the `cmake ..` step:

```
cmake .. -DVEHICLE=kia_soul -DSERIAL_PORT_BRAKE=/dev/ttyACM0 -DSERIAL_PORT_CAN_GATEWAY=/dev/ttyACM1 -DSERIAL_PORT_STEERING=/dev/ttyACM2 -DSERIAL_PORT_THROTTLE=/dev/ttyACM3
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

You need to enable debug mode with `-DDEBUG=ON` and tell `cmake` what serial port
the module you want to monitor is connected to
(see [section on uploading](#uploading-the-firmware) for details on the default
ports for each module). The default baud rate is `115200` but you can change it:

```
cmake .. -DVEHICLE=kia_soul -DDEBUG=ON -DSERIAL_PORT_THROTTLE=/dev/ttyACM0 -DSERIAL_BAUD_THROTTLE=19200
```

You can use a module's `monitor` target to automatically run `screen`, or a
module's `monitor-log` target to run `screen` and output to a file called
`screenlog.0` in the module's build directory:

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

# Controlling Your Vehicle - an Example Application

Now that all your Arduino modules are properly setup, it is time to start sending control commands.

We've created an example application, joystick commander, that uses the OSCC API to interface with the firmware, allowing you to send commands using a game controller and receive reports from the on-board OBD-II CAN. These commands are converted into CAN messages, which the OSCC API sends to the respective Arduino modules and are used to actuate the vehicle.

[OSCC Joystick Commander](https://github.com/PolySync/oscc-joystick-commander)

We've also created a ROS node, that uses the OSCC API to interface with the firmware from ROS messages, allowing you to send commands and receive reports in ROS.

[ROSCCO](https://github.com/PolySync/roscco)

# OSCC API

**Open and close CAN channel to OSCC Control CAN.**

```c
oscc_result_t oscc_open( uint channel );
oscc_result_t oscc_close( uint channel );
```

These methods are the start and end points of using the OSCC API in your application. ```oscc_open``` will open a socket connection
on the specified CAN channel, enabling it to quickly receive reports from and send commands to the firmware modules.
When you are ready to terminate your application, ```oscc_close``` can terminate the connection.

**NOTE:**
> The OSCC API catches the SIGIO signal which is asserted when a CAN frame is received on the CAN socket, and sets up a SIGIO signal handler
> to forward valid CAN messages to your application. If you have a section of code that is susceptible to conflicts with interrupts,
> you should temporarily block the SIGIO signal for the duration of that section and unblock it afterward.

**Enable and disable all OSCC modules.**

```c
oscc_result_t oscc_enable( void );
oscc_result_t oscc_disable( void );
```

After you have initialized your CAN connection to the firmware modules, these methods can be used to enable or disable the system. This
allows your application to choose when to enable sending commands to the firmware. Although you can only send commands when the system is
enabled, you can receive reports at any time.

**Publish control command to the corresponding module.**

```c
oscc_result_t publish_brake_position( double normalized_position );
oscc_result_t publish_steering_torque( double normalized_torque );
oscc_result_t publish_throttle_position( double normalized_position );
```

These commands will forward a double value to the specified firmware module. The double values are [0.0, 1.0] for brake and throttle,
and [-1.0, 1.0] for steering where -1.0 is counterclockwise and 1.0 is clockwise. The API will construct the appropriate values to send
as spoofed voltages to the vehicle to achieve the desired state. The API also contains safety checks to ensure no voltages outside of
the vehicle's expected range are sent.

**NOTE:**
> Each of these functions must be called at least once every 200ms to prevent the modules from detecting a loss of communication to the
> controlling computer (at which point they would disable themselves).

**Register callback function to handle OSCC report and OBD messages.**

```c
oscc_result_t subscribe_to_brake_reports( void(*callback)(oscc_brake_report_s *report) );
oscc_result_t subscribe_to_steering_reports( void(*callback)(oscc_steering_report_s *report) );
oscc_result_t subscribe_to_throttle_reports( void(*callback)(oscc_throttle_report_s *report) );
oscc_result_t subscribe_to_fault_reports( void(*callback)(oscc_fault_report_s *report) );
oscc_result_t subscribe_to_obd_messages( void(*callback)(struct can_frame *frame) );
```

In order to receive reports from the modules, your application will need to register a callback handler with the OSCC API.
When the appropriate report for your callback function is received from the API's socket connection, it will then forward the
report to your software.

Each module's reports are described in their respective wiki sections:

* [Brake (EV)](https://github.com/PolySync/oscc/wiki/Firmware-Brake-%28EV%29#brake-report)
* [Brake (Petrol)](https://github.com/PolySync/oscc/wiki/Firmware-Brake-%28Petrol%29#brake-report)
* [Steering](https://github.com/PolySync/oscc/wiki/Firmware-Steering#steering-report)
* [Throttle](https://github.com/PolySync/oscc/wiki/Firmware-Throttle#throttle-report)

In addition to OSCC specific reports, it will also forward any non-OSCC reports to any callback function registered with
```subscribe_to_obd_messages```. This can be used to view CAN frames received from the vehicle's OBD-II CAN channel. If you know
the corresponding CAN frame's id, you can parse reports sent from the car.

# Tests

There are two types of tests available: unit and property-based.

## Test Dependencies

The unit tests and property-based tests each have their own set of dependencies
required to run the tests.

For the unit tests you must have **Cucumber 2.0.0** and its dependency **Boost** installed:

```
sudo apt install ruby-dev libboost-dev
sudo gem install cucumber -v 2.0.0
```

For the property-based tests you must have **Rust**, its build manager **Cargo**, and **libclang**:

```
sudo apt install rustc cargo clang libclang-dev
```

## Running Tests

Building and running the tests is similar to the firmware itself, but you must instead tell
`cmake` to build the tests instead of the firmware with the `-DTESTS=ON` flag. We also pass
the `-DCMAKE_BUILD_TYPE=Release` flag so that `cmake` will disable debug symbols and enable
optimizations, good things to do when running tests to ensure nothing breaks with
optimizations. Lastly, you must tell the tests which vehicle header to use for
the tests (e.g., `-DVEHICLE=kia_soul`).

```
cd firmware
mkdir build
cd build
cmake .. -DTESTS=ON -DCMAKE_BUILD_TYPE=Release -DVEHICLE=kia_soul
```

### Unit Tests

Each module has a suite of unit tests that use **Cucumber** with **Cgreen**. There are pre-built
64-bit Linux versions in `firmware/common/testing/framework`.

Boost is required for Cucumber-CPP and has been statically linked into `libcucumber-cpp.a`.
If you need to build your own versions you can use the provided script `build_test_framework.sh`
which will install the Boost dependencies (needed for building), clone the needed
repositories with specific hashes, build the Cgreen and Cucumber-CPP libraries,
and place static Boost in the Cucumber-CPP library.

The built libraries will be placed in an `oscc_test_framework` directory in the
directory that you ran the script from. You can then copy `oscc_test_framework/cucumber-cpp`
and `oscc_test_framework/cgreen` to `firmware/common/testing/framework`.

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

### Property-Based Tests

The throttle, steering, and brake modules, along with the PID controller library, also contain a series of
property-based tests.

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

### All Tests

Finally, as a convenience you can run all available tests:

```
make run-all-tests
```

# Additional Vehicles & Contributing

OSCC currently has information regarding the Kia Soul PS (2014-2018), but we want to grow! The
repository is structured to facilitate including more vehicles as more is learned about them.

In order to include information related to a new vehicle's specification, follow the format defined in ```api/include/vehicles/kia_soul.h``` and
add a `cmake` option to choose your new header when compiling the API.

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
