<img src="https://github.com/PolySync/OSCC/blob/master/assets/oscc_logo_title.png">


The Open Source Car Control Project is a hardware and software project detailing the conversion of a late model vehicle into an autonomous driving research and development vehicle.

See the [Wiki](https://github.com/PolySync/OSCC/wiki) for full documentation, details and other information.


Repository Contents
-------------------
* **/3d_models** - Technical drawings and 3D files for board enclosures and other useful parts.
* **/assets** - Diagrams and images used in the wiki.
* **/boards** - PCB schematics and board designs for control modules.
* **/control** - Utilities for controlling a vehicle.
* **/firmware** - Arduino code for the various modules.
* **/vehicle_info** - Information on specific vehicles such as sensor outputs and wiring diagrams.

Building Arduino Code
------------

OSCC used Arduino makefiles to avoid some of the limitations of the Arduino IDE. Using this method you can build, upload, and monitor the code.
Check out [Arduino-Makefile](https://github.com/sudar/Arduino-Makefile) to get started.

Once the prerequisites are installed, connect the Arduino you want to program to your machine via USB.


Navigate to the directory for the firmware you want to build.

`cd firmware/steering/kia_soul_ps/`

And then build, upload, and monitor the serial output.

`make && make upload && make monitor`

The GNU utility `screen` is used to monitor the serial output from the Arduino. In order to exit screen use `C-a \`.


Additional Vehicles & Contributing
------------

OSCC currently has information regarding the Kia Soul PS (2014-2016), but we want to grow! The repository is structured to facilitate including more vehicles as more is learned about them.


Please see [CONTRIBUTING.md](CONTRIBUTING.md).



License Information
-------------------

Hardware source materials (e.g. schematics, board layouts, wiring diagrams, data sheets, physical installation documentation, 3D models, etc.) for the OSCC (Open Source Car Control) Project are licensed under Creative Commons Attribution-ShareAlike 4.0 International (CC BY-SA 4.0).

Software source for the OSCC (Open Source Car Control) Project is licensed under GNU General Public License (GPLv3) unless otherwise noted (e.g. 3rd party dependencies, etc.).

Please see [LICENSE.md](LICENSE.md) for more details.


Contact Information
-------------------

Please direct questions regarding OSCC and/or licensing to help@polysync.io

*Distributed as-is; no warranty is given.*

Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved.
