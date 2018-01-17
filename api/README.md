
# Getting started with the OSCC API

There are a few options on how to get started with your own application that
uses the OSCC API.

## The library approach

To build both `libosccapi.so` and `libosccapi.a` for the Kia Soul EV,
you can run the following commands on Linux from this directory.
```
mkdir build && cd build
cmake -DKIA_SOUL_EV=ON ..
make
```
Substitute `-DKIA_SOUL_EV` for another supported vehicle if the EV isn't your
target. For example `-DKIA_SOUL=ON` will build the libraries with support
for the Kia Soul Petrol instead.

### Using `libosccapi.so`
Using the shared library for a project that uses the OSCC API is easy!
The following is an example `CMakeLists.txt` for doing just that. This example
is all you need for a project with it's own source file, `main.c`.
```
cmake_minimum_required(VERSION 2.8)
project(an_example)
set(OSCC_API_INSTALL /path_to/oscc_directory/api)
link_directories(${OSCC_API_INSTALL}/build)
set(OSCC_INCLUDES ${OSCC_API_INSTALL}/include)
set(SOURCES ${CMAKE_SOURCE_DIR}/src/main.c)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${OSCC_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC osccapi)
```

### Using `libosccapi.a`
Using the static library isn't much different than the shared one,
the CMakeLists.txt just requires the full path to the library as it doesn't
intuit it from a provided link_directory.
```
cmake_minimum_required(VERSION 2.8)
project(an_example)
set(OSCC_API_INSTALL /path_to/oscc_directory/api)
set(OSCC_INCLUDES ${OSCC_API_INSTALL}/include)
set(SOURCES ${CMAKE_SOURCE_DIR}/src/main.c)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${OSCC_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OSCC_API_INSTALL}/build/libosccapi.a)
```

## Straight from the sources
Another approach is to just include the OSCC API source code in your project.
Since the OSCC API has no dependencies beyond a Linux system, this is easy too!
Here's an example CMakeLists.txt that takes the straight from the sources
approach. Note that it makes the assumption that we're building for the Kia
Soul Petrol with the `"-DKIA_SOUL=ON"`.

```
cmake_minimum_required(VERSION 2.8)
project(an_example)
set(OSCC_API_INSTALL /path_to/oscc_directory/api)
set(OSCC_INCLUDES ${OSCC_API_INSTALL}/include ${OSCC_API_INSTALL}/src)
set(SOURCES ${CMAKE_SOURCE_DIR}/src/main.c ${OSCC_API_INSTALL}/src/oscc.c)
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DKIA_SOUL=ON")
target_include_directories(${PROJECT_NAME} PUBLIC ${OSCC_INCLUDES})
```

## Using the API

Let's walk through actually writing some code that uses the OSCC API, an example
program that listens to all the information that OSCC reports. Feel free to
experiment with this code, if it's all put into a `main.c` you can use
any of the CMakeLists.txt options above to build it.

First, we'll do some generic setup. This isn't a requriement of the OSCC API
but might make our example a little more useful.

```
#include <stdio.h> // printf
#include <signal.h> // SIGINT
#include <unistd.h> // usleep
```
Okay, now for the actual OSCC API include that allows us to initiate,
listen to each message type and then properly release OSCC. `oscc.h` is
all you need!

```
#include "oscc.h" // oscc_open, oscc_disable, oscc_close and each subscribe.
```
In order for our example to trigger the proper cleanup when we press `Ctrl+C`
to quit the program, we'll setup a signal handler to let us know.

```
// Global indication of Ctrl+C.
static int EXIT_SIGNALED = OSCC_OK;

// Help exiting gracefully.
void signal_handler(int signal_number)
{
    if (signal_number == SIGINT)
    {
        EXIT_SIGNALED = OSCC_ERROR;
    }
}
```

Now lets walk through our OSCC message subscribers. When we get a report from
something OSCC provides, let's print a message reporting what we see.

__Throttle__
```
void throttle_callback(oscc_throttle_report_s * report)
{
    printf("Hello from throttle_callback!\n");
}
```
__Steering__
```
void steering_callback(oscc_steering_report_s * report)
{
    printf("steering_callback reporting!\n");
}
```
__Brake__
```
void brake_callback(oscc_brake_report_s * report)
{
    printf("'Brake-ing' news from the brake_callback!\n");
}
```
__Fault__
```
void fault_callback(oscc_fault_report_s * report)
{
    printf("Oh! Something caused a fault and we made it to the fault_callback. We're seeing a ");

    if (report->fault_origin_id == FAULT_ORIGIN_BRAKE)
    {
        printf("brake fault!\n");
    }
    else if (report->fault_origin_id == FAULT_ORIGIN_STEERING)
    {
        printf("steering fault!\n");
    }
    else if (report->fault_origin_id == FAULT_ORIGIN_THROTTLE)
    {
        printf("throttle fault!\n");
    }
}
```
__OBD__
```
static void obd_callback(struct can_frame * frame)
{
    printf("From the obd_callback, CAN ID: %x\n", frame->can_id);
}
```
Okay, we're all setup! Let get to work! Our code base is pretty small so
we'll just stick to the `main` function
```
int main
{
```
Now let's put that 'Ctrl+C' setup we did earlier to use.
```
    struct sigaction sig;
    sig.sa_handler = signal_handler;
    sigaction(SIGINT, &sig, NULL);
```
Next we need to tell OSCC where to to send it's messages.
```
    oscc_subscribe_to_obd_messages(obd_callback);
    oscc_subscribe_to_brake_reports(brake_callback);
    oscc_subscribe_to_steering_reports(steering_callback);
    oscc_subscribe_to_throttle_reports(throttle_callback);
    oscc_subscribe_to_fault_reports(fault_callback);
```
In order to verify that initalizing OSCC succeeds and clean things up
if we fail, we need a way to capture results.
```
    oscc_result_t return_value;
```
For brevity, we'll assume that we're opening CAN channel 0 to connect to OSCC
and spin it up!
```
    int channel = 0;
    return_value = oscc_open(channel);
```
Since we captured the result of out call to oscc_open, we can verify it
succeded before moving on or signal that we need to clean things up.
```
    if(return_value != OSCC_OK)
    {
        printf("Error in oscc_open!\n");
        EXIT_SIGNALED = OSCC_ERROR;
    }
```

Now we wait, just let the callbacks to their work. To avoid loading the
CPU with this example, we'll sleep for a bit (50ms) on each iteration.
```
    while(EXIT_SIGNALED == OSCC_OK)
    {
        (void) usleep(50000);
    }
```

Once we're through, we can clean things up.
```
    oscc_disable();
    oscc_close(channel);
```

Don't forget to close up the `main` function!
```
}
```