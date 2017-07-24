# Building joystick-commander

The joystick commander uses cmake to build.

It defaults to a socketcan based interface for the build, but has the option to support the Kvaser linuxcan module also.

At this point, all versions of the joystick_commander require a USB to CAN connection to interact with the OSCC modules.  The Kvaster Leaf Light v2 is an example of the type of converter necessary to facilitate this communication.

## Building against socketcan

**The socketcan libraries must be installed for this build sequence.**

From the control/joystick_commander directory run the following sequence to build the joystick_commander using the socketcan libraries:

```
mkdir build
cd build
cmake .. ( optionally: cmake -DOSCC_INTF=socketcan .. )
make
```

This will build the joystick_commander in the `build` directory.

Once the build is complete, if the socketcan library has been configured correctly (`sudo ip link set can0 type can bitrate 500000`) the joystick_commander is available for use.

## Building against linuxcan

**The Kvaser linuxcan libraries must be installed for this build sequence.**

From the control/joystick_commander directory run the following sequence to build the joystick_commander using the Kvaser linuxcan libaries:

```
mkdir build
cd build
cmake -DOSCC_INTF=linuxcan ..
make
```

This will build the joystick_commander in the `build` directory.

Once the build is complete the joystick_commander is available for use.