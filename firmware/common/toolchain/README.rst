=============
Arduino CMake
=============
.. image:: https://travis-ci.org/arduino-cmake/arduino-cmake.svg?branch=master
    :target: https://travis-ci.org/arduino-cmake/arduino-cmake

Arduino is a great development platform, which is easy to use. It has everything a beginner should need. The *Arduino IDE* simplifies a lot of things for the standard user, but if you are a professional programmer the IDE can feel simplistic and restrictive.

One major drawback of the *Arduino IDE* is that you cannot do anything without it, which for me is a **complete buzz kill**. Thats why queezythegreat created an alternative build system for the Arduino using CMake. With the original repository not being updated since 2014 and lacking some important features like support for the latest Arduino SDKs, we decided to fix things up for 1.6; now that some people have started using this as their preferred fork, we decided to accept our fate of being the current maintainers.

CMake is great cross-platform build system that works on practically any operating system. With it you are not constrained to a single build system. CMake lets you generate the build system that fits your needs, using the tools you like. It can generate any type of build system, from simple Makefiles, to complete projects for Eclipse, Visual Studio, XCode, etc.

The **Arduino CMake** build system integrates tightly with the *Arduino SDK*.

*Arduino SDK* version **0.19** or higher is required.

So if you like to do things from the command line (using make), or to build your firmware where you're in control, or if you would like to use an IDE such as Eclipse, KDevelop, XCode, CodeBlocks or something similar,  then **Arduino CMake** is the system for you.

Features
--------

* Integrates with *Arduino SDK*
* Supports all Arduino boards.
* Supports Arduino type libraries.
* Automatic detection of Arduino libraries.
* Generates firmware images.
* Generates built-in examples.
* Generates libraries.
* Sketch support.
* Upload support.
* Hardware Platform support.
* Programmer support (with bootloader upload).
* Supports multiple build system types (Makefiles, Eclipse, KDevelop, CodeBlocks, XCode, etc).
* Cross-platform: Windows, Linux, Mac.
* Extensible build system, thanks to CMake.


Feedback
--------

**Arduino CMake** is hosted on GitHub and is available on multiple forks (At the time of writing up to **161(!)**),
with one the more advanced and maintainable being the following:

https://github.com/arduino-cmake/arduino-cmake/

However, none of this would have been possible with the generous bunch of work the original author queezythegreat invested into this project, who definately earns most of the credit for getting things running:

https://github.com/queezythegreat/arduino-cmake

We want to stress again that he did all the initial work to even make CMake spit out Arduino firmwares. Without the effort queezythegreat put into this, we would not have been able to even think about getting an Arduino 1.6 project running. This is by no means a hostile fork and we would give him ownership of the organization as well at any time, should he wish so.

Did you find a bug or would like a specific feature, please report it at:

https://github.com/arduino-cmake/arduino-cmake/issues

If you would like to hack on this project, don't hesitate to fork it on GitHub.
We will be glad to integrate your changes if you send me a ``Pull Request``.


Requirements
------------

* Base requirements:

  - ``CMake`` - http://www.cmake.org/cmake/resources/software.html
  - ``Arduino SDK`` - http://www.arduino.cc/en/Main/Software

* Linux requirements:

  - ``gcc-avr``      - AVR GNU GCC compiler
  - ``binutils-avr`` - AVR binary tools
  - ``avr-libc``     - AVR C library
  - ``avrdude``      - Firmware uploader


Contributors
------------

I would like to thank the following people for contributing to **Arduino CMake**:

* Juan José Herrero Barbosa (`Souler`_)
* Bei Chen Liu (`Baycken`_)
* MrPointer (`MrPointer`_)
* Marc Plano-Lesay (`Kernald`_)
* James Goppert (`jgoppert`_)
* Matt Tyler (`matt-tyler`_)
* Andrew Stromme (`astromme`_)
* `johnyb`_
* `arunh`_
* Sebastian Herp (`sebastianherp`_)
* Michael Daffin (`james147`_)
* Pavel Ilin (`PIlin`_)
* Igor Mikolic-Torreira (`igormiktor`_)
* Claudio Henrique Fortes Felix (`chffelix`_)
* Alexandre Tuleu (`atuleu`_)
* `getSurreal`_
* Sebastian Zaffarano (`szaffarano`_)
* `cheshirekow`_
* Logan Engstrom (`meadowstream`_) 
* Francisco Ramírez (`franramirez688`_)
* Brendan Shillingford (`bshillingford`_)
* Mike Purvis (`mikepurvis`_) 
* Steffen Hanikel (`hanikesn`_)
* Tomasz Bogdal (`queezythegreat`_) (original author of arduino-cmake)
* Jonas (`JonasProgrammer`_)

.. _Souler: https://github.com/Souler
.. _Baycken: https://github.com/Baycken
.. _MrPointer: https://github.com/MrPointer
.. _Kernald: https://github.com/Kernald
.. _jgoppert: https://github.com/jgoppert
.. _matt-tyler: https://github.com/matt-tyler
.. _astromme: https://github.com/astromme
.. _johnyb: https://github.com/johnyb
.. _arunh: https://github.com/arunh
.. _sebastianherp: https://github.com/sebastianherp
.. _james147: https://github.com/james147
.. _PIlin: https://github.com/PIlin
.. _igormiktor: https://github.com/igormiktor
.. _chffelix: https://github.com/chffelix
.. _atuleu: https://github.com/atuleu
.. _getSurreal: https://github.com/getSurreal
.. _szaffarano: https://github.com/szaffarano
.. _cheshirekow: https://github.com/cheshirekow
.. _meadowstream: https://github.com/meadowstream
.. _franramirez688: https://github.com/franramirez688
.. _bshillingford: https://github.com/bshillingford
.. _mikepurvis: https://github.com/mikepurvis
.. _hanikesn: https://github.com/hanikesn
.. _queezythegreat: https://github.com/queezythegreat
.. _JonasProgrammer: https://github.com/JonasProgrammer

License
-------
This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.

TODO
----

* Test more complex configurations and error handling

Contents
--------

1. `Getting Started`_
2. `Using Arduino CMake`_

   1. `Creating Firmware Images`_
   2. `Creating Libraries`_
   3. `Arduino Sketches`_
   4. `Arduino Built-in Examples`_
   5. `Arduino Libraries`_
   6. `Arduino Library Examples`_
   7. `Compiler and Linker Flags`_
   8. `Programmers`_
   9. `Pure AVR Development`_
   10. `Advanced Options`_
   11. `Miscellaneous Functions`_
   12. `Bundling Arduino CMake`_

3. `Linux Environment`_

   1. `Linux Serial Naming`_
   2. `Linux Serial Terminals`_

4. `Mac OS X Environment`_

   1. `Mac Serial Naming`_
   2. `Mac Serial Terminals`_

5. `Windows Environment`_

   1. `CMake Generators`_
   2. `Windows Serial Naming`_
   3. `Windows Serial Terminals`_

6. `Eclipse Environment`_
7. `Troubleshooting`_

   1. `undefined reference to `__cxa_pure_virtual'`_
   2. `Arduino Mega 2560 image does not work`_
   3. `Library not detected automatically`_
   4. `error: attempt to use poisoned "SIG_USART0_RECV"`_

8. `Resources`_






Getting Started
---------------


The following instructions are for **\*nix** type systems, specifically this is a Linux example.

In short you can get up and running using the following commands::

    mkdir build
    cd build
    cmake ..
    make
    make upload              # to upload all firmware images             [optional]
    make blink-serial  # to get a serial terminal to wire_serial   [optional]

For a more detailed explanation, please read on...

1. Toolchain file
   
   In order to build firmware for the Arduino you have to specify a toolchain file to enable cross-compilation. There are two ways of specifying the file, either at the command line or from within the *CMakeLists.txt* configuration files. The bundled example uses the second approach like so::

        set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)

   Please note that this must be before the ``project(...)`` command.
   
   If you would like to specify it from the command line, heres how::

        cmake -DCMAKE_TOOLCHAIN_FILE=../path/to/toolchain/file.cmake PATH_TO_SOURCE_DIR

2. Creating a build directory

   The second order of business is creating a build directory. CMake has a great feature called out-of-source builds, what this means is the building is done in a completely separate directory from where the sources are. The benefit of this is you don't have any clutter in you source directory and you won't accidentally commit something that is auto-generated.

   So let's create that build directory::

        mkdir build
        cd build

3. Creating the build system

   Now let's create the build system that will create our firmware::

        cmake ..

   To specify the build system type, use the ``-G`` option, for example::

        cmake -G"Eclipse CDT4 - Unix Makefiles" ..

   If you rather use a GUI, use::

        cmake-gui ..

4. Building

   Next we will build everything::

        make

5. Uploading

   Once everything built correctly we can upload. Depending on your Arduino you will have to update the serial port used for uploading the firmware. To change the port please edit the following variable in *CMakeLists.txt*::

        set(${FIRMWARE_NAME}_PORT /path/to/device)

   Ok lets do a upload of all firmware images::

        make upload

   If you have an upload sync error then try resetting/ power cycling the board before starting the upload process.

6. Serial output

   If you have some serial output, you can launch a serial terminal from the build system. The command used for executing the serial terminal is user configurable by the following setting::

        set(${FIRMWARE_NAME}_SERIAL serial command goes here)

   In order to get access to the serial port use the following in your command::

        @SERIAL_PORT@

   That constant will get replaced with the actual serial port used (see uploading). In the case of our example configuration we can get the serial terminal by executing the following::

        make blink-serial










Using Arduino CMake
-------------------

In order to use **Arduino CMake** just include the toolchain file, everything will get set up for building. You can set the toolchain
in `CMakeList.txt` like so::

        set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/ArduinoToolchain.cmake)

Please note that this must be before the ``project(...)`` command.

You can also specify it at build configuration time::

        cmake -DCMAKE_TOOLCHAIN_FILE=../path/to/toolchain/file.cmake PATH_TO_SOURCE_DIR


Creating Firmware Images
~~~~~~~~~~~~~~~~~~~~~~~~

Once you have the **Arduino CMake** loaded you can start defining firmware images.

To create Arduino firmware in CMake you use the ``generate_arduino_firmware`` command. The full syntax of the command is::

    generate_arduino_firmware(target_name
         [BOARD board_name]
         [BOARD_CPU board_cpu]
         [SKETCH sketch_path | SRCS  src1 src2 ... srcN]
         [HDRS  hdr1 hdr2 ... hdrN]
         [LIBS  lib1 lib2 ... libN]
         [PORT  port]
         [SERIAL serial_cmd]
         [PROGRAMMER programmer_id]
         [AFLAGS flags]
         [NO_AUTOLIBS])


The options are:

+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **Name**           | **Description**                                                      | **REQUIRED**                           |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **BOARD**          | Board name *(such as uno, mega2560, ...)*                            | **REQUIRED**                           |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **BOARD_CPU**      | Board CPU *(such as atmega328, atmega168, ...)*                      | **REQUIRED** if board cpu is ambiguous |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **SKETCH**         | Sketch path (see `Arduino Sketches`_)                                | **SKETCH or SRCS are REQUIRED**        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **SRCS**           | Source files                                                         | **SKETCH or SRCS are REQUIRED**        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **HDRS**           | Headers files *(for project based build systems)*                    |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **LIBS**           | Libraries to link (see `Creating libraries`_)                        |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **PORT**           | Serial port, for upload and serial targets (see `Upload Firmware`_)  |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **SERIAL**         | Serial command for serial target (see `Serial Terminal`_)            |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **PROGRAMMER**     | Programmer ID, enables programmer burning (see `Programmers`_).      |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **ARDLIBS**        | Manual list of Arduino type libraries, common use case is when the   |                                        |
|                    | library header name does not match the librarie's directory name.    |                                        |
|                    | **ADVANCED OPTION!** Can be used in conjuction with **NO_AUTOLIBS**. |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **AFLAGS**         | avrdude flags for target                                             |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **NO_AUTOLIBS**    | Disable Arduino library detection *(default On)*                     |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **MANUAL**         | Disable Arduino Core (enables pure AVR development)                  |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+

You can specify the options in two ways, either as the command arguments or as variables. When specifying the options as variables they must be named::

    ${TARGET_NAME}_${OPTION_NAME}

Where **${TARGET_NAME}** is the name of you target and **${OPTION_NAME}** is the name of the option.

So to create a target (firmware image) called ``blink``, composed of ``blink.h`` and ``blink.cpp`` source files for the *Arduino Uno*, you write the following::

    set(blink_SRCS  blink.cpp)
    set(blink_HDRS  blink.h)
    set(blink_BOARD uno)

    generate_arduino_firmware(blink)

The previous example can be rewritten as::

    generate_arduino_firmware(blink
          SRCS  blink.cpp
          HDRS  blink.h
          BOARD uno)

Upload Firmware
_______________

To enable firmware upload functionality, you need to add the ``PORT`` option::

    set(blink_SRCS  blink.cpp)
    set(blink_HDRS  blink.h)
    set(blink_PORT /dev/ttyUSB0)
    set(blink_BOARD nano)
    set(blink_BOARD_CPU atmega328) # required because nano has atmega328 and atmega168 models

    generate_arduino_firmware(blink)

Or::

    generate_arduino_firmware(blink
          SRCS  blink.cpp
          HDRS  blink.h
          PORT  /dev/ttyUSB0
          BOARD_CPU atmega328
          BOARD nano)

Once defined there will be two targets available for uploading, ``${TARGET_NAME}-upload`` and a global ``upload`` target (which will depend on all other upload targets defined in the build):

* ``blink-upload`` - will upload just the ``blink`` firmware
* ``upload`` - upload all firmware images registered for uploading

Serial Terminal
_______________
To enable serial terminal, use the ``SERIAL`` option (``@SERIAL_PORT@`` will be replaced with the ``PORT`` option)::

    set(blink_SRCS  blink.cpp)
    set(blink_HDRS  blink.h)
    set(blink_PORT  /dev/ttyUSB0)
    set(blink_SERIAL picocom @SERIAL_PORT@ -b 9600 -l)
    set(blink_BOARD uno)

    generate_arduino_firmware(blink)

Alternatively::

    generate_arduino_firmware(blink
          SRCS  blink.cpp
          HDRS  blink.h
          PORT  /dev/ttyUSB0
          SERIAL picocom @SERIAL_PORT@ -b 9600 -l
          BOARD uno)

This will create a target named ``${TARGET_NAME}-serial`` (in this example: blink-serial).




Creating Libraries
~~~~~~~~~~~~~~~~~~

Creating libraries is very similar to defining a firmware image, except we use the ``generate_arduino_library`` command. This command creates static libraries, and are not to be confused with `Arduino Libraries`_. The full command syntax::

    generate_arduino_library(name
         [BOARD board_name]
         [BOARD_CPU board_cpu]
         [SRCS  src1 src2 ... srcN]
         [HDRS  hdr1 hdr2 ... hdrN]
         [LIBS  lib1 lib2 ... libN]
         [NO_AUTOLIBS])

The options are:

+--------------------+------------------------------------------------------+----------------------------------------+
| **Name**           | **Description**                                      | **REQUIRED**                           |
+--------------------+------------------------------------------------------+----------------------------------------+
| **BOARD**          | Board name *(such as uno, mega2560, ...)*            | **REQUIRED**                           |
+--------------------+------------------------------------------------------+----------------------------------------+
| **BOARD_CPU**      | Board CPU *(such as atmega328, atmega168, ...)*      | **REQUIRED** if board cpu is ambiguous |
+--------------------+------------------------------------------------------+----------------------------------------+
| **SRCS**           | Source files                                         | **REQUIRED**                           |
+--------------------+------------------------------------------------------+----------------------------------------+
| **HDRS**           | Headers files *(for project based build systems)*    |                                        |
+--------------------+------------------------------------------------------+----------------------------------------+
| **LIBS**           | Libraries to link *(sets up dependency tracking)*    |                                        |
+--------------------+------------------------------------------------------+----------------------------------------+
| **NO_AUTOLIBS**    | Disable Arduino library detection *(default On)*     |                                        |
+--------------------+------------------------------------------------------+----------------------------------------+
| **MANUAL**         | Disable Arduino Core (enables pure AVR development)  |                                        |
+--------------------+------------------------------------------------------+----------------------------------------+

You can specify the options in two ways, either as the command arguments or as variables. When specifying the options as variables they must be named::

    ${TARGET_NAME}_${OPTION_NAME}

Where **${TARGET_NAME}** is the name of you target and **${OPTION_NAME}** is the name of the option.

Let's define a simple library called ``blink_lib`` with two sources files for the *Arduino Uno*::

    set(blink_lib_SRCS  blink_lib.cpp)
    set(blink_lib_HDRS  blink_lib.h)
    set(blink_lib_BOARD uno)

    generate_arduino_library(blink_lib)

The other way of defining the same thing is::

    generate_arduino_library(blink_lib
        SRCS  blink_lib.cpp
        HDRS  blink_lib.h
        BOARD uno)

Once that library is defined we can use it in our other firmware images... Let's add ``blink_lib`` to the ``blink`` firmware::

    set(blink_SRCS  blink.cpp)
    set(blink_HDRS  blink.h)
    set(blink_LIBS  blink_lib)
    set(blink_BOARD uno)

    generate_arduino_firmware(blink)

CMake has automatic dependency tracking, so when you build the ``blink`` target, ``blink_lib`` will automatically get built, in the right order.


Arduino Sketches
~~~~~~~~~~~~~~~~

To build a Arduino sketch use the **SKETCH** option (see `Creating firmware images`_). For example::

    set(blink_SKETCH  ${ARDUINO_SDK_PATH}/examples/1.Basics/Blink) # Path to sketch directory
    set(blink_BOARD   uno)

    generate_arduino_firmware(blink)

This will build the **blink** example from the **Arduino SDK**.

Note: When specifying the sketch directory path, arduino-cmake is expecting to find a sketch file named after the directory (with a extension of .pde or .ino).

You can also specify the path to the main sketch file, then the parent directory of that sketch will be search for additional sketch files.

Arduino Built-in Examples
~~~~~~~~~~~~~~~~~~~~~~~~~

The Arduino SDK comes with a handful of code examples, providing an easy setup for simple operations.
Since there are many examples, they were categorized, making each example be under a certain category.
Each example consists of at least one source file, named after the example and has the *.ino* or *.pde* extension, and sits under a directory which is also named after the example.
Each category is a directory named after it, having all its examples as sub-directories, named after them.
One such example is ``Blink``, probrably the most popular one as well. It's located under the ``Basics`` category and has a source file named ``Blink.ino``.

**Arduino CMake** has the abillity to automatically generate these examples, simply by passing their name and optionally their category, as some sort of an optimization. **It supports case-insensitive names**
If you would like to generate and upload some of those examples you can use the `generate_arduino_example` command. The syntax of the command is::

    generate_arduino_example(target_name
                             [EXAMPLE example_name]
                             [BOARD board_name]
                             [BOARD_CPU board_cpu]
                             [CATEGORY category_name]
                             [PORT port]
                             [SERIAL serial command]
                             [PORGRAMMER programmer_id]
                             [AFLAGS avrdude_flags])

The options are:

+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **Name**           | **Description**                                                      | **REQUIRED**                           |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **EXAMPLE**        | Example name.                                                        | **REQUIRED**                           |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **BOARD**          | Board name *(such as uno, mega2560, ...)*                            | **REQUIRED**                           |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **BOARD_CPU**      | Board CPU *(such as atmega328, atmega168, ...)*                      | **REQUIRED** if board cpu is ambiguous |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **CATEGORY**       | Category name.                                                       |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **PORT**           | Serial port, for upload and serial targets (see `Upload Firmware`_)  |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **SERIAL**         | Serial command for serial target (see `Serial Terminal`_)            |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **PROGRAMMER**     | Programmer ID, enables programmer burning (see `Programmers`_).      |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **AFLAGS**         | avrdude flags for target                                             |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+

To generate a target for the **blink** example from the **Basics** category for the **Uno** board::

    generate_arduino_example(blink_example
                             CATEGORY Basics
                             EXAMPLE Blink
                             BOARD uno
                             PORT  /dev/ttyUSB0)

You can also rewrite the previous like so::

    set(blink_example_CATEGORY Basics)
    set(blink_example_EXAMPLE Blink)
    set(blink_example_BOARD uno)
    set(blink_example_PORT /dev/ttyUSB0)

    generate_arduino_example(blink_example)

The previous example will generate the following two target::

    blink_example
    blink_example-upload
    
**Note:** The above example will work perfectly fine even if the ``Basics`` category hadn't been passed.

Arduino Libraries
~~~~~~~~~~~~~~~~~

Libraries are one of the more powerful features which the Arduino offers to users. Instead of rewriting code, people bundle their code in libraries and share them with others.
The structure of these libraries is very simple, which makes them easy to create.

An Arduino library is **any directory which contains a header named after the directory**, simple.
Any source files contained within that directory are part of the library. Here is a example of library a called ExampleLib::

    ExampleLib/
      |-- ExampleLib.h
      |-- ExampleLib.cpp
      `-- OtherLibSource.cpp

Now because the power of Arduino lies within those user-created libraries, support for them is built right into **Arduino CMake**. The **Arduino SDK** comes with a large number of default libraries and adding new libraries is simple.

To incorporate a library into your firmware, you can do one of three things:

1. Place the library next to the default Arduino libraries (located at **${ARDUINO_SDK}/libraries**)
2. Place the library next to the firmware configuration file (same directory as the **CMakeLists.txt**)
3. Place the library in a separate folder and tell **Arduino CMake** the path to that directory.
   
   To tell CMake where to search for libraries use the `link_directories` command. The command has to be used before defining any firmware or libraries requiring those libraries.
   
   For example::
     
      link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libraries)
      link_directories(/home/username/arduino_libraries)


If a library contains nested sources, a special option must be defined to enable recursion. For example to enable recursion for the Arduino Wire library use::

    set(Wire_RECURSE True)

The option name should be **${LIBRARY_NAME}_RECURSE**, where in this case **LIBRARY_NAME** is equal to *Wire*.


Arduino Libraries are not to be confused with normal static libraries (for exmaple *system libraries* or libraries created using generate_arduino_library). The **LIBS** option only accepts static libraries, so do not list the Arduino Libraries in that option (as you will get an error).


Arduino Library Examples
~~~~~~~~~~~~~~~~~~~~~~~~

Most Arduino libraries have examples bundled with them. If you would like to generate and upload some of those examples you can use the `generate_arduino_library_example` command. The syntax of the command is::

    generate_arduino_library_example(target_name
                             [LIBRARY library_name]
                             [EXAMPLE example_name]
                             [BOARD  board_name]
                             [BOARD_CPU board_cpu]
                             [PORT port]
                             [SERIAL serial command]
                             [PORGRAMMER programmer_id]
                             [AFLAGS avrdude_flags])

The options are:

+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **Name**           | **Description**                                                      | **REQUIRED**                           |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **LIBRARY**        | Library name.                                                        | **REQUIRED**                           |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **EXAMPLE**        | Example name.                                                        | **REQUIRED**                           |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **BOARD**          | Board name *(such as uno, mega2560, ...)*                            | **REQUIRED**                           |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **BOARD_CPU**      | Board CPU *(such as atmega328, atmega168, ...)*                      | **REQUIRED** if board cpu is ambiguous |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **PORT**           | Serial port, for upload and serial targets (see `Upload Firmware`_)  |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **SERIAL**         | Serial command for serial target (see `Serial Terminal`_)            |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **PROGRAMMER**     | Programmer ID, enables programmer burning (see `Programmers`_).      |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **AFLAGS**         | avrdude flags for target                                             |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+

To generate a target for the **master_writer** example from the **Wire** library for the **Uno**::

    generate_arduino_library_example(wire_example
                             LIBRARY Wire
                             EXAMPLE master_writer
                             BOARD uno
                             PORT  /dev/ttyUSB0)

You can also rewrite the previous like so::

    set(wire_example_LIBRARY Wire)
    set(wire_example_EXAMPLE master_writer)
    set(wire_example_BOARD uno)
    set(wire_example_PORT /dev/ttyUSB0)

    generate_arduino_library_example(wire_example)

The previous example will generate the following two target::

    wire_example
    wire_example-upload

Compiler and Linker Flags
~~~~~~~~~~~~~~~~~~~~~~~~~

The default compiler and linker flags should be fine for most projects. If you required specific compiler/linker flags, use the following options to change them:

+--------------------------+----------------------+
|  **Name**                | **Description**      |
+--------------------------+----------------------+
| **ARDUINO_C_FLAGS**      | C compiler flags     |
+--------------------------+----------------------+
| **ARDUINO_CXX_FLAGS**    | C++ compiler flags   |
+--------------------------+----------------------+
| **ARDUINO_LINKER_FLAGS** | Linker flags         |
+--------------------------+----------------------+


Set these option either before the `project()` like so::

    set(ARDUINO_C_FLAGS      "-ffunction-sections -fdata-sections")
    set(ARDUINO_CXX_FLAGS    "${ARDUINO_C_FLAGS} -fno-exceptions")
    set(ARDUINO_LINKER_FLAGS "-Wl,--gc-sections")
    
    project(ArduinoExample C CXX)

or when configuring the project::

    cmake -D"ARDUINO_C_FLAGS=-ffunction-sections -fdata-sections" ../path/to/sources/


Programmers
~~~~~~~~~~~

**Arduino CMake** fully supports programmers for burning firmware and bootloader images directly onto the Arduino. 
If you have a programmer that is supported by the *Arduino SDK*, everything should work out of the box.
As of version 1.0 of the *Arduino SDK*, the following programmers are supported:

+--------------------+---------------------+
| **Programmer ID**  | **Description**     |
+--------------------+---------------------+
| **avrisp**         | AVR ISP             |
+--------------------+---------------------+
| **avrispmkii**     | AVRISP mkII         |
+--------------------+---------------------+
| **usbtinyisp**     | USBtinyISP          |
+--------------------+---------------------+
| **parallel**       | Parallel Programmer |
+--------------------+---------------------+
| **arduinoisp**     | Arduino as ISP      |
+--------------------+---------------------+

The programmers.txt file located in `${ARDUINO_SDK_PATH}/hardware/arduino/` lists all supported programmers by the *Arduino SDK*.

In order to enable programmer support, you have to use the **PROGRAMMER** option (see `Creating firmware images`_)::

    set(${TARGET_NAME}_PROGRAMMER programmer_id)

where `programmer_id` is the name of the programmer supported by the *Arduino SDK*.

Once you have enabled programmer support, two new targets are available in the build system:

* **${TARGET_NAME}-burn** - burns the firmware image via the programmer
* **${TARGET_NAME}-burn-bootloader** - burns the original **Arduino bootloader** image via the programmer

If you need to restore the original **Arduino bootloader** onto your Arduino, so that you can use the traditional way of uploading firmware images via the bootloader, use **${TARGET_NAME}-burn-bootloader** to restore it.


Pure AVR Development
~~~~~~~~~~~~~~~~~~~~

For those developers who don't want any Arduino magic, but still want to utilize the hardware platform you are in luck. This section will outline the `generate_avr_firmware()` and `generate_avr_library()` commands, which enables
you to compile sources for the given Arduino board.

No Arduino Core or Arduino libraries will get generated, this is for manual compilation of sources. These commands are for people that know what they are doing, or have done pure AVR development.
People starting out, or just familiar with Arduino should not use these commands.

The `generate_avr_firmware()` command::

    generate_avr_firmware(name
         [BOARD board_name]
         [BOARD_CPU board_cpu]
         [SRCS  src1 src2 ... srcN]
         [HDRS  hdr1 hdr2 ... hdrN]
         [LIBS  lib1 lib2 ... libN]
         [PORT  port]
         [SERIAL serial_cmd]
         [PROGRAMMER programmer_id]
         [AFLAGS flags])

This will compile the sources for the specified Arduino board type.

The options:

+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **Name**           | **Description**                                                      | **REQUIRED**                           |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **BOARD**          | Board name *(such as uno, mega2560, ...)*                            | **REQUIRED**                           |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **BOARD_CPU**      | Board CPU *(such as atmega328, atmega168, ...)*                      | **REQUIRED** if board cpu is ambiguous |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **SRCS**           | Source files                                                         | **REQUIRED**                           |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **HDRS**           | Headers files *(for project based build systems)*                    |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **LIBS**           | Libraries to link *(sets up dependency tracking)*                    |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **PORT**           | Serial port, for upload and serial targets (see `Upload Firmware`_)  |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **SERIAL**         | Serial command for serial target (see `Serial Terminal`_)            |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **PROGRAMMER**     | Programmer ID, enables programmer burning (see `Programmers`_).      |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+
| **AFLAGS**         | avrdude flags for target                                             |                                        |
+--------------------+----------------------------------------------------------------------+----------------------------------------+

You can specify the options in two ways, either as the command arguments or as variables. When specifying the options as variables they must be named::

    ${TARGET_NAME}_${OPTION_NAME}

Where **${TARGET_NAME}** is the name of you target and **${OPTION_NAME}** is the name of the option.


The `generate_avr_library()` command::

    generate_avr_library(name
         [BOARD board_name]
         [BOARD_CPU board_cpu]
         [SRCS  src1 src2 ... srcN]
         [HDRS  hdr1 hdr2 ... hdrN]
         [LIBS  lib1 lib2 ... libN])

This will compile a static library for the specified Arduino board type.

The options:

+--------------------+---------------------------------------------------+----------------------------------------+
| **Name**           | **Description**                                   | **REQUIRED**                           |
+--------------------+---------------------------------------------------+----------------------------------------+
| **BOARD**          | Board name *(such as uno, mega2560, ...)*         | **REQUIRED**                           |
+--------------------+---------------------------------------------------+----------------------------------------+
| **BOARD_CPU**      | Board CPU *(such as atmega328, atmega168, ...)*   | **REQUIRED** if board cpu is ambiguous |
+--------------------+---------------------------------------------------+----------------------------------------+
| **SRCS**           | Source files                                      | **REQUIRED**                           |
+--------------------+---------------------------------------------------+----------------------------------------+
| **HDRS**           | Headers files *(for project based build systems)* |                                        |
+--------------------+---------------------------------------------------+----------------------------------------+
| **LIBS**           | Libraries to link *(sets up dependency tracking)* |                                        |
+--------------------+---------------------------------------------------+----------------------------------------+

You can specify the options in two ways, either as the command arguments or as variables. When specifying the options as variables they must be named::

    ${TARGET_NAME}_${OPTION_NAME}

Where **${TARGET_NAME}** is the name of you target and **${OPTION_NAME}** is the name of the option.

Register Custom Hardware Platforms
~~~~~~~~~~~~~~~~
Arduino development may involve the use of additional hardware platforms that behave differently, 
such as the Sagnuino e.g.
Arduino CMake allows you to register those platforms without the need to copy their files locally, 
exactly as you would register the default Arduino platform. In fact, this is what happens behind the scenes:

1. Platform's path is determined. By default it's Arduino's path. 
   See: `Arduino Platforms PRE 1.5`_ and `Arduino Platforms 1.5`_.

2. Platform's architecture is determined. By default it's avr.

If one would like to specify a custom platform and/or architecuture, it should set the following variables:

+--------------------------------+---------------------------------------------------+
| **Name**                       | **Description**                                   |
+--------------------------------+---------------------------------------------------+
| **PLATFORM_PATH**              | Platform's path on the local file system.*        |
+--------------------------------+---------------------------------------------------+
| **PLATFORM_ARCHITECTURE**      | Platform's architecture*                          |
+--------------------------------+---------------------------------------------------+

**Note:** If variables are to be used, they MUST be set before including the Toolchain file.
  
A valid Hardware Platform is a directory containing the following::
  
      HARDWARE_PLATFORM_PATH/
          |-- bootloaders/
          |-- cores/
          |-- variants/
          |-- boards.txt
          `-- programmers.txt
  
The ``board.txt`` describes the target boards and bootloaders, While ``programmers.txt`` the programmer defintions.
  
A good example of a *Hardware Platform* is in the Arduino SDK: ``${ARDUINO_SDK_PATH}/hardware/arduino/``
.. _Arduino Platforms PRE 1.5: http://code.google.com/p/arduino/wiki/Platforms
.. _Arduino Platforms 1.5: http://code.google.com/p/arduino/wiki/Platforms1

Advanced Options
~~~~~~~~~~~~~~~~

The following options control how **Arduino CMake** is configured:

+---------------------------------+-----------------------------------------------------+
| **Name**                        | **Description**                                     |
+---------------------------------+-----------------------------------------------------+
| **ARDUINO_SDK_PATH**            | Full path to the **Arduino SDK**                    |
+---------------------------------+-----------------------------------------------------+
| **ARDUINO_AVRDUDE_PROGRAM**     | Full path to `avrdude` programmer                   |
+---------------------------------+-----------------------------------------------------+
| **ARDUINO_AVRDUDE_CONFIG_PATH** | Full path to `avrdude` configuration file           |
+---------------------------------+-----------------------------------------------------+
| **ARDUINO_DEFAULT_BOARD**       | Default Arduino Board ID, when not specified.       |
+---------------------------------+-----------------------------------------------------+
| **ARDUINO_DEFAULT_PORT**        | Default Arduino port, when not specified.           |
+---------------------------------+-----------------------------------------------------+
| **ARDUINO_DEFAULT_SERIAL**      | Default Arduino Serial command, when not specified. |
+---------------------------------+-----------------------------------------------------+
| **ARDUINO_DEFAULT_PROGRAMMER**  | Default Arduino Programmer ID, when not specified.  |
+---------------------------------+-----------------------------------------------------+

To force a specific version of **Arduino SDK**, configure the project like so::

    cmake -DARDUINO_SDK_PATH=/path/to/arduino_sdk ../path/to/sources

Note: You must create a new build system if you change **ARDUINO_SDK_PATH**.


When **Arduino CMake** is configured properly, these options are defined:

+---------------------------------+-----------------------------------------------------+
| **Name**                        | **Description**                                     |
+---------------------------------+-----------------------------------------------------+
| **ARDUINO_FOUND**               | Set to True when the **Arduino SDK** is detected    |
|                                 | and configured.                                     |
+---------------------------------+-----------------------------------------------------+
| **ARDUINO_SDK_VERSION**         | Full version of the **Arduino SDK** (ex: 1.0.0)     |
+---------------------------------+-----------------------------------------------------+
| **ARDUINO_SDK_VERSION_MAJOR**   | Major version of the **Arduino SDK** (ex: 1)        |
+---------------------------------+-----------------------------------------------------+
| **ARDUINO_SDK_VERSION_MINOR**   | Minor version of the **Arduino SDK** (ex: 0)        |
+---------------------------------+-----------------------------------------------------+
| **ARDUINO_SDK_VERSION_PATCH**   | Patch version of the **Arduino SDK** (ex: 0)        |
+---------------------------------+-----------------------------------------------------+


During compilation, you can enable the following environment variables.

+---------------------------------+-----------------------------------------------------+
| **Name**                        | **Description**                                     |
+---------------------------------+-----------------------------------------------------+
| **VERBOSE**                     | Enables verbose compilation, displays commands      |
|                                 | being executed. (Non empty value)                   |
+---------------------------------+-----------------------------------------------------+
| **VERBOSE_SIZE**                | Enables full/verbose output from avr-size.          |
|                                 | (Non empty value)                                   |
+---------------------------------+-----------------------------------------------------+

Miscellaneous Functions
~~~~~~~~~~~~~~~~~~~~~~~

This section will outlines some of the additional miscellaneous functions available to the user.

* **print_board_list()**:
  
  Print list of detected Arduino Boards.
* **print_programmer_list()**:
  
  Print list of detected Programmers.
* **print_programmer_settings(PROGRAMMER)**:
  
     *PROGRAMMER* - programmer id
  
  Print the detected Programmer settings.
* **print_board_settings(BOARD_NAME)**:
  
    *BOARD_NAME* - Board name (nano, uno, mega...)
  
  Print the detected Arduino board settings.
  
Bundling Arduino CMake
~~~~~~~~~~~~~~~~~~~~~~

Using **Arduino CMake** in your own project is simple, you just need a single directory called **cmake**. Just copy that entire directory into you project and you are set.

Copying the **cmake** directory, although simple is not the best solution. If you are using GIT for source code versioning, the best solution is using a submodule. The submodule gives you the power of updating to the latest version of **Arduino CMake** without any effort. To add a submodule do::

    git submodule add git://github.com/queezythegreat/arduino-cmake.git arduino-cmake

Then just set the CMAKE_TOOLCHAIN_FILE variable::

    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arduino-cmake/cmake/ArduinoToolchain.cmake)

For more information on GIT submodules please read: `GIT Book - Submodules`_

.. _GIT Book - Submodules: http://book.git-scm.com/5_submodules.html

Linux Environment
-----------------

Running the *Arduino SDK* on Linux is a little bit more involved, because not everything is bundled with the SDK. The AVR GCC toolchain is not distributed alongside the Arduino SDK, so it has to be installed seperately.

To get **Arduino CMake** up and running follow these steps:

1. Install the following packages using your package manager:
    
   * ``gcc-avr``      - AVR GNU GCC compiler
   * ``binutils-avr`` - AVR binary tools
   * ``avr-libc``     - AVR C library
   * ``avrdude``      - Firmware uploader
    
2. Install the *Arduino SDK*.
    
   Depending on your distribution, the *Arduino SDK* may or may not be available.
    
   If it is available please install it using your packages manager otherwise do:
    
   1. Download the `Arduino SDK`_
   2. Extract it into ``/usr/share``
    
   NOTE: Arduino version **0.19** or newer is required!

3. Install CMake:
    
   * Using the package manager or
   * Using the `CMake installer`_

   NOTE: CMake version 2.8 or newer is required!



Linux Serial Naming
~~~~~~~~~~~~~~~~~~~

On Linux the Arduino serial device is named as follows (where **X** is the device number)::

    /dev/ttyUSBX
    /dev/ttyACMX

Where ``/dev/ttyACMX`` is for the new **Uno** and **Mega** Arduino's, while ``/dev/ttyUSBX`` is for the old ones.

CMake configuration example::

    set(${FIRMWARE_NAME}_PORT /dev/ttyUSB0)


Linux Serial Terminals
~~~~~~~~~~~~~~~~~~~~~~

On Linux a wide range on serial terminal are availabe. Here is a list of a couple:

* ``minicom``
* ``picocom``
* ``gtkterm``
* ``screen``











Mac OS X Environment
--------------------

The *Arduino SDK*, as on Windows, is self contained and has everything needed for building. To get started do the following:

1. Install the  *Arduino SDK*

   1. Download `Arduino SDK`_
   2. Copy ``Arduino`` into ``Applications``
   3. Install ``FTDIUSBSerialDrviver*`` (for FTDI USB Serial)

2. Install CMake
   
   1. Download `CMake`_
   2. Install ``cmake-*.pkg``
        
      NOTE: Make sure to click on **`Install Command Line Links`**

Mac Serial Naming
~~~~~~~~~~~~~~~~~

When specifying the serial port name on Mac OS X, use the following names (where XXX is a unique ID)::

    /dev/tty.usbmodemXXX
    /dev/tty.usbserialXXX

Where ``tty.usbmodemXXX`` is for new **Uno** and **Mega** Arduino's, while ``tty.usbserialXXX`` are the older ones. 

CMake configuration example::

    set(${FIRMWARE_NAME}_PORT /dev/tty.usbmodem1d11)

Mac Serial Terminals
~~~~~~~~~~~~~~~~~~~~

On Mac the easiest way to get a Serial Terminal is to use the ``screen`` terminal emulator. To start a ``screen`` serial session::

    screen /dev/tty.usbmodemXXX

Where ``/dev/tty.usbmodemXXX`` is the terminal device. To exit press ``C-a C-\``.

CMake configuration example::

    set(${FIRMWARE_NAME}_SERIAL screen @SERIAL_PORT@)











Windows Environment
-------------------

On Windows the *Arduino SDK* is self contained and has everything needed for building. To setup the environment do the following:

1. Place the `Arduino SDK`_ either
   
   * into  **Program Files**, or
   * onto the **System Path**
    
   NOTE: Don't change the default *Arduino SDK* directory name, otherwise auto detection will no work properly!

2. Add to the **System Path**: ``${ARDUINO_SDK_PATH}/hardware/tools/avr/utils/bin``
3. Install `CMake 2.8`_
   
   NOTE: Make sure you check the option to add CMake to the **System Path**.


CMake Generators
~~~~~~~~~~~~~~~~

Once installed, you can start using CMake the usual way, just make sure to chose either a **MSYS Makefiles** or **Unix Makefiles** type generator::

    MSYS Makefiles              = Generates MSYS makefiles.
    Unix Makefiles              = Generates standard UNIX makefiles.
    CodeBlocks - Unix Makefiles = Generates CodeBlocks project files.
    Eclipse CDT4 - Unix Makefiles
                                = Generates Eclipse CDT 4.0 project files.

If you want to use a **MinGW Makefiles** type generator, you must generate the build system the following way:

1. Remove ``${ARDUINO_SDK_PATH}/hardware/tools/avr/utils/bin`` from the **System Path**
2. Generate the build system using CMake with the following option set (either through the GUI or from the command line)::

    CMAKE_MAKE_PROGRAM=${ARDIUNO_SDK_PATH}/hardware/tools/avr/utils/bin/make.exe

3. Then build the normal way

The reason for doing this is the MinGW generator cannot have the ``sh.exe`` binary on the **System Path** during generation, otherwise you get an error.

Windows Serial Naming
~~~~~~~~~~~~~~~~~~~~~

When specifying the serial port name on Windows, use the following names::

    com1 com2 ... comN

CMake configuration example::

    set(${FIRMWARE_NAME}_PORT com3)

Windows Serial Terminals
~~~~~~~~~~~~~~~~~~~~~~~~

Putty is a great multi-protocol terminal, which supports SSH, Telnet, Serial, and many more... The latest development snapshot supports command line options for launching a serial terminal, for example::

    putty -serial COM3 -sercfg 9600,8,n,1,X

CMake configuration example (assuming putty is on the **System Path**)::

    set(${FIRMWARE_NAME}_SERIAL putty -serial @SERIAL_PORT@)

Putty - http://tartarus.org/~simon/putty-snapshots/x86/putty-installer.exe










Eclipse Environment
-------------------

Eclipse is a great IDE which has a lot of functionality and is much more powerful than the *Arduino IDE*. In order to use Eclipse you will need the following:

1. Eclipse
2. Eclipse CDT extension (for C/C++ development)

On most Linux distribution you can install Eclipse + CDT using your package manager, otherwise you can download the `Eclipse IDE for C/C++ Developers`_ bundle.

Once you have Eclipse, here is how to generate a project using CMake:

1. Create a build directory that is next to your source directory, like this::
   
       build_directory/
       source_directory/

2. Run CMake with the `Eclipse CDT4 - Unix Makefiles` generator, inside the build directory::

        cd build_directory/
        cmake -G"Eclipse CDT4 - Unix Makefiles" ../source_directory

3. Open Eclipse and import the project from the build directory.

   1. **File > Import**
   2. Select `Existing Project into Workspace`, and click **Next**
   3. Select *Browse*, and select the build directoy.
   4. Select the project in the **Projects:** list
   5. Click **Finish**



.. _Eclipse IDE for C/C++ Developers: http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/heliossr2











Troubleshooting
---------------

The following section will outline some solutions to common problems that you may encounter.

undefined reference to `__cxa_pure_virtual'
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When linking you'r firmware image you may encounter this error on some systems. An easy fix is to add the following to your firmware source code::

    extern "C" void __cxa_pure_virtual(void);
    void __cxa_pure_virtual(void) { while(1); } 


The contents of the ``__cxa_pure_virtual`` function can be any error handling code; this function will be called whenever a pure virtual function is called. 

* `What is the purpose of `cxa_pure_virtual``_

.. _What is the purpose of `cxa_pure_virtual`: http://stackoverflow.com/questions/920500/what-is-the-purpose-of-cxa-pure-virtual

Arduino Mega 2560 image does not work
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you are working on Linux, and have ``avr-gcc`` >= 4.5 you might have a unpatched version gcc which has the C++ constructor bug. This bug affects the **Atmega2560** when using classes which causes the Arduino firmware to crash.

If you encounter this problem either downgrade ``avr-gcc`` to **4.3** or rebuild gcc with the following patch::

    --- gcc-4.5.1.orig/gcc/config/avr/libgcc.S  2009-05-23 17:16:07 +1000
    +++ gcc-4.5.1/gcc/config/avr/libgcc.S   2010-08-12 09:38:05 +1000
    @@ -802,7 +802,9 @@
        mov_h   r31, r29
        mov_l   r30, r28
        out     __RAMPZ__, r20
    +   push    r20
        XCALL   __tablejump_elpm__
    +   pop r20
     .L__do_global_ctors_start:
        cpi r28, lo8(__ctors_start)
        cpc r29, r17
    @@ -843,7 +845,9 @@
        mov_h   r31, r29
        mov_l   r30, r28
        out     __RAMPZ__, r20
    +   push    r20
        XCALL   __tablejump_elpm__
    +   pop r20
     .L__do_global_dtors_start:
        cpi r28, lo8(__dtors_end)
        cpc r29, r17

* `AVR GCC Bug 45263 Report`_
* `The global constructor bug in avr-gcc`_

.. _AVR GCC Bug 45263 Report: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=45263
.. _The global constructor bug in avr-gcc: http://andybrown.me.uk/ws/2010/10/24/the-major-global-constructor-bug-in-avr-gcc/



Library not detected automatically
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When a Arduino library does not get detected automatically, it usually means CMake cannot find it (obvious).

One common reason why the library is not detected, is because the directory name of the library does not match the header.
If I'm including a library header like so::

    #include "my_library.h"

Based on this include, **Arduino CMake** is expecting to find a library that has a directory name **my_libray**.
If the directory name does not match the header, it won't be consider a Arduino Library (see `Arduino Libraries`_).


When a library being used is located in a non-standard location (not in the **Arduino SDK** or next to the firmware), then that directory must be registered.
To register a non-standard directory containing Arduino libraries, use the following::

    link_directories(path_to_directory_containing_libraries)

Remember to **use this command before defining the firmware**, which requires the library from that directory.


error: attempt to use poisoned "SIG_USART0_RECV"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you get the following error::

    /usr/share/arduino/hardware/arduino/cores/arduino/HardwareSerial.cpp:91:41: error: attempt to use poisoned "SIG_USART0_RECV"
    /usr/share/arduino/hardware/arduino/cores/arduino/HardwareSerial.cpp:101:15: error: attempt to use poisoned "SIG_USART0_RECV"
    /usr/share/arduino/hardware/arduino/cores/arduino/HardwareSerial.cpp:132:15: error: attempt to use poisoned "SIG_USART1_RECV"
    /usr/share/arduino/hardware/arduino/cores/arduino/HardwareSerial.cpp:145:15: error: attempt to use poisoned "SIG_USART2_RECV"
    /usr/share/arduino/hardware/arduino/cores/arduino/HardwareSerial.cpp:158:15: error: attempt to use poisoned "SIG_USART3_RECV"

You probably recently upgraded `avr-libc` to the latest version, which has deperecated the use of these symbols. There is a `Arduino Patch`_ which
fixes these error, you can read more about this bug here: `Arduino Bug ISSUE 955`_.

.. _Arduino Bug ISSUE 955: http://code.google.com/p/arduino/issues/detail?id=955
.. _Arduino Patch: http://arduino.googlecode.com/issues/attachment?aid=9550004000&name=sig-patch.diff&token=R2RWB0LZXQi8OpPLsyAdnMATDNU%3A1351021269609

Resources
---------

Here are some resources you might find useful in getting started.

1. CMake:

   * `Offical CMake Tutorial`_
   * `CMake Tutorial`_
   * `CMake Reference`_

.. _Offical CMake Tutorial: http://www.cmake.org/cmake/help/cmake_tutorial.html
.. _CMake Tutorial: http://mathnathan.com/2010/07/11/getting-started-with-cmake/
.. _CMake Reference: http://www.cmake.org/cmake/help/cmake-2-8-docs.html

2. Arduino:
   
   * `Getting Started`_ - Introduction to Arduino
   * `Playground`_ - User contributed documentation and help
   * `Arduino Forums`_ - Official forums
   * `Arduino Reference`_ - Official reference manual

.. _Getting Started: http://www.arduino.cc/en/Guide/HomePage
.. _Playground: http://www.arduino.cc/playground/
.. _Arduino Reference: http://www.arduino.cc/en/Reference/HomePage
.. _Arduino Forums: http://www.arduino.cc/forum/








.. _CMake 2.8: http://www.cmake.org/cmake/resources/software.html
.. _CMake: http://www.cmake.org/cmake/resources/software.html
.. _CMake Installer: http://www.cmake.org/cmake/resources/software.html
.. _Arduino SDK: http://www.arduino.cc/en/Main/Software
