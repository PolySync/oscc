# How to Contribute

- [Introduction](#introduction)
- [Getting Started](#getting-started)
- [Making Contributions](#making-contributions)
  - [Contributing Code](#contributing-code)
  - [Contributing a Diagram](#contributing-a-diagram)
  - [Submitting Enhancement Requests](#submitting-enhancement-requests)
  - [Pull Request Labels](#pull-request-labels)
  - [Acceptance Tests](#acceptance-tests)
- [Directory Contents](#directory-contents)
- [Diagrams](#diagrams)
- [Building](#building)
- [Documentation](#documentation)
- [Helpful Links](#helpful-links)
- [Release Process](#release-process)
  - [System Acceptane Testing](#system-acceptance-testing)
- [OSCC Coding standard](#oscc-coding-standard)
  - [1. Directives](#1-directives)
  - [2. Rules](#2-rules)

---

## Introduction
There are many ways to contribute to the OSCC project: support for additional vehicles can be added, diagrams can be made to look better, car systems can be better described, CAN frames can be better detailed, PCBs can be optimized, or code could be refactored and improved.

The directory structure of the project is created in such a way that adding support for additional vehicles should be simple and intuitive.

All vehicle specific directories should be recreated for each additional vehicle. The naming convention is:

```
<make>_<model>_<generational identifier>
```

Below is a sample of how additional vehicle directories should be created.

```
.
├── platforms
│   └── <my_new_car>       # Name of vehicle you're adding
│       └── firmware
│           ├── brake
│           ├── can_gateway
│           ├── steering
│           └── throttle
```

## Getting Started

- Using the repo's Issues section, create a new issue and:
  - Clearly describe the issue including steps to reproduce when it is a bug
  - Create a branch using `git fork` in the repository on GitHub

## Making Contributions

- Do not work directly on the `master` branch; changes to the master branch will be removed
- Create a topic branch from where you want to base your work
  - You'll likely `fork` from of the master branch
  - To quickly create a topic branch based on master:

  ```
  git checkout -b fix/master/my_contribution master
  ```

- Write a good commit message

#### Commit Messages

- Use the present tense ("Add feature" not "Added feature")
- Use the imperative mood ("Move resistor to..." not "Moves resistor to...")
- Limit the first line to 72 characters or less
- Reference issues and pull requests specifically
- When changing documentation only, include [ci skip] in the commit description

### Contributing Code

- Code should conform to the [coding standard](#oscc-coding-standard)
- Push your changes to a topic branch in your branch of the repository
- Ideally, your commits would also be [GPG signed](https://help.github.com/articles/signing-commits-using-gpg/)
  - `git config --global commit.gpgSign true`
  - `git config --global gpg.program gpg2`
  - `git config --global push.gpgSign if-asked`
  - `git config --global user.signingKey <Your Public Key ID>`
- Submit a pull request to the repository in the PolySync organization
- Update your github issue to mark that you have submitted code and are ready for it to be reviewed (Status: Ready for Merge)
  - Include a link to the pull request in the ticket
- The PolySync team will review all pull requests on a weekly basis
- **Code contributed should include unit tests** that demonstrate the code functions as expected
- For more details, please see the "Release Process" section below

### Contributing a Diagram

- The diagrams were created using the [Eagle PCB Design tool](https://cadsoft.io)
    - All board diagrams should use this tool
- Modifications should include tests to demonstrate that the modified board works as expected

### Submitting Enhancement Requests

Enhancement suggestions are tracked as GitHub issues:

- Use a clear and descriptive title for the issue to identify the suggestion
- Provide a step-by-step description of the suggested enhancement in as many details as possible
- Provide specific examples to demonstrate the steps
    - Include snippets in the examples as Markdown code blocks ( using the \`\`\` format )
- Describe the current behavior and how this enhancement improves or changes it
- Explain why this enhancement is useful to most users
- Specify which version of OSCC this change applies to
    - The versions can be found in the Releases tab on GitHub

### Pull Request Labels

- Fill in the required template
- Follow the style guide
- Code should be complete
    - Any changes should include unit tests for the submitted
- End files with a new line
- Changes should be located in the most appropriate directory
- Please open an issue to suggest a new label.

### Acceptance Tests
- All submitted changes will go through a round of acceptance tests.
    - Acceptance includes:
        - Unit Tests to demonstrate the code works out of context
        - System Integration tests to demonstrate that the system works as intended
    - If there are specific or special test scenarios required for the change, they should be documented for any testing
	- PolySync will perform the acceptance testing periodically in order to get changes incorporated into the source
    - If the scope of the change is small enough that the modification is not visible from the system-level, a unit test alone may be acceptable.

## Directory Contents

This information is also available in the README.

| Directory    | Contents                                 |
| ------------ | :--------------------------------------- |
| 3d_tools     | Technical drawings and 3D files for board enclosures and other useful parts |
| assets       | Diagrams and images used in the wiki     |
| boards       | PCB schematics and board designs for control modules that are integrated with the vehicle, typically these are the throttle, steering, and brake boards, but can include other items should they be contributed. |
| control      | User Space applications to provide mechanisms for the user to control the vehicle directly via something like a PC.  Applications can be command-line or GUI based and include items like a joystick controller |
| firmware     | Arduino code for the throttle, steering, and brake functionality.  This directory also includes code for the CAN gateway module, which uses a stock Arduino board. The firmware should remain modular and portable to a a different Arduino board |
| vehicle_info | Information on specific vehicles such as sensor outputs and wiring diagrams.  Each additional vehicle should be in its own subdirectory |

## Diagrams

Adding diagrams for new boards or other components should be located in the directory most appropriate for their function:

- A new enclosure should go in the 3d_models directory
- A new board diagram should go in the boards directory
- Any new vehicle information should reside in the vehicle_info directory

Existing diagrams should be edited using the [Autodesk Eagle](https://cadsoft.io), as mentioned above.

## Building

To build the various components in the system, please see the README.md

## Documentation

- Documentation updates are welcome
- Documentation should be located in the `doc` directory where it is most applicable (See the Directory descriptions above)
  - If the `doc` directory does not exist, please create it as part of the submission
- Documentation pull requests function in the same way as other pull requests
- Documentation should be in markdown format

## Helpful Links

- [Autodesk Eagle](https://cadsoft.io)
- [Arduino FAQ](https://www.arduino.cc/en/Main/FAQ)
- [Throttle/Steering/CAN Gateway board](https://www.arduino.cc/en/Main/ArduinoBoardUno)
- [Braking board](https://www.arduino.cc/en/Main/arduinoBoardMega2560)

---

# Release Process

Any changes to the OSCC modules must undergo a series of tests that conclude with a "stress test" on the vehicle itself.  What is included here is the process by which code modifications are incorporated into the main source branch (devel) and what the system tests constitute.  One of the key problems that this process is intended to solve is that any source code change must be tested on the vehicle before it can be merged with the "devel" branch.

**Merging code before it is tested is dangerous to users and must be avoided at all costs.**


## Code modification process

### Traditional Method

- The traditional method in git is to issue a Pull Request or PR against a branch to prompt a code review
  - After getting the code approved and cleaning any merge conflicts, a repository maintainer performs a merge to incorporate the branch changes in the main code stream
  - This method does not work for OSCC; it allows code changes that affect how the system operates to be incorporated into the main development branch without a demonstration that the changes result in safe vehicle operation
  - It is imperative that any code changes to OSCC go through integration and system test **before** any code is merged into the baseline

### OSCC Method

- The OSCC method uses GitHub based `status checks` that a branch must go through before it can be considered for a merge
- The steps involved are as follows:
  - When creating a branch on the source base, the branch must be a *protected branch* from the main development branch, `devel`
  - Code changes are made and then pushed back to the repository
  - At this point, there are status checks for each of the following:
    1. The code compiles without errors or warnings (automated check)
    2. The code complies with the [coding standard](#oscc-coding-standard) (automated check)
    3. The code passes a code review
		1. Two reviewer approvals are required for the code review to have passed
		2. The code review focuses on code structure and analysis of the logic as opposed to coding standard compliance
		3. If there are code changes that cannot be evaluated using the coding standard compliance checker, those changes require a code review for compliance to the standard
    4. The regression test suite completes successfully
    5. The [system acceptance tests](#system-acceptance-testing) completes successfully (system acceptance test listed below, some parts automated)
  - Once all the status checks have passed, resolve any merge conflicts and merge the changed branch with devel
    1. The merge commit will need to be signed, which means local, command-line
       merge rather than GitHub UI.
    2. The [hub](https://hub.github.com/) tool can help with this.
    3. `git checkout devel`
    4. `hub merge --no-ff https://github.com/PolySync/oscc/pull/169`
    5. `git push origin devel`

## System Acceptance Testing

The OSCC System Acceptance Test Suite is the final hurdle before a branch can be merged into the main development branch. The OSCC System Acceptance Test Suite involves running the changed code on the vehicle (nominally the Kia Soul with the OSCC modules installed.)

The following are tests that will be executed.  The expected results define compliance with the OSCC System Acceptance Test Suite.

### Definitions

1. Throttle, Gas Pedal, Accelerator - The mechanism by which the driver specifies how fast the vehicle should go, typically a pedal for a human driver
2. Brake - The mechanism by which the driver specifies how fast to stop, typically a pedal for a human driver
3. Steering - The mechanism by which the driver specifies how far to turn the wheels of the vehicle, typically this is a steering wheel for a human driver.  It should be noted here that the item being controlled is the steering wheel angle, with the direct result of changing the vehicle direction



### Testing with OSCC Disabled

For these tests, the vehicle is being controlled by a human driver.

1. *Initial Condition* - OSCC Disabled
2. Normal Acceleration - Normal Braking
3. Hard Acceleration - Hard Braking, attempt to engage the Anti-Lock Brakes (ABL)
4. Swerve
5. Turn left
6. Turn right
7. *Expected Results:* - normal driving operation

### Testing with OSCC Enabled

For these tests, the vehicle is being controlled by a remote control computer or joystick and can be automated if necessary or possible.

#### Driveability

1. Normal Acceleration - Normal Brake
	1. *Initial Conditions:* - Vehicle is in drive with brake applied
	2. Release brake
	3. Apply throttle at *?50%?*
	4. Delay 3 seconds
	5. Apply throttle at *?throttle_min%?*
	6. Apply brake at *?50%?*
	7. Wait for the vehicle to come to a complete stop
	8. Maintain brake pressure
	9. *Expected Results:* - Normal acceleration and braking with the vehicle stopped when the test is complete

2. Hard Acceleration - Hard Brake (engage ABL, if possible)
	1. *Initial Conditions:* - Vehicle is in drive with brake applied
	2. Release brake
	3. Apply throttle at *?throttle_max%?*
	4. Delay 3 seconds
	5. Apply throttle at *?throttle_min%?*
	6. Apply brake at *?brake_max%?*- this step should attempt to engage the Anti-Lock Braking system
	7. Wait for the vehicle to come to a complete stop
	8. Maintain brake pressure
	9. *Expected Results:* - Max acceleration and braking with the vehicle stopped when the test is complete

3. Swerve - left to right and right to left
	1. *Initial Conditions:* - Vehicle is in drive with the brake applied
	2. Release brake
	3. Apply throttle at *?30%?*
	4. Maintain speed during maneuvers
	5. Turn steering to *?30 degrees?* right
	6. Delay 0.5 seconds
	7. Turn steering to *?30 degrees?* left
	8. Delay 0.5 seconds
	9. Turn steering to *?0 degrees?*
	10. Delay 1 second
	11. Turn steering to *?30 degrees?* left
	12. Delay 0.5 seconds
	13. Turn steering to *?30 degrees?* right
	14. Delay 0.5 seconds
	15. Turn steering to *?0 degrees?*
	16. Delay 1 second
	17. Apply throttle at *?throttle_min%?*
	18. Apply brake at *?50%?*
	19. Wait for the vehicle to come to a complete stop
	20. Maintain brake pressure
	21. *Expected Results:* - Vehicle has swerved out of and back into the original path

4. Perform left turn
	1. *Initial Conditions:* - Vehicle is in drive with the brake applied
	2. Release brake
	3. Apply throttle at *?30%?*
	4. Maintain speed during maneuvers
	5. Turn steering to *?30 degrees?* left
	6. Delay 3 seconds
	7. Turn steering to *?0 degrees?*
	8. Delay 1 second
	9. Apply throttle at *?throttle_min%?*
	10. Apply brake at *?50%?*
	11. Wait for the vehicle to come to a complete stop
	12. Maintain brake pressure
	13. *Expected Results:* - Vehicle has performed a left turn

5. Perform right turn
	1. *Initial Conditions:* - Vehicle is in drive with the brake applied
	2. Release brake
	3. Apply throttle at *?30%?*
	4. Maintain speed during maneuvers
	5. Turn steering to *?30 degrees?* right
	6. Delay 3 seconds
	7. Turn steering to *0 degrees*
	8. Delay 1 second
	9. Apply throttle at *?throttle_min%?*
	10. Apply brake at *?50%?*
	11. Wait for the vehicle to come to a complete stop
	12. Maintain brake pressure
	13. *Expected Results:* - Vehicle has performed a right turn

6. Drive in a circle with traction control enabled
	1. *Initial Conditions:* - Vehicle is in drive with the brake applied, traction control enabled
	2. Release brake
	3. Apply throttle at *?30%?*
	4. Maintain speed during maneuvers
	5. Turn steering to *?max degrees?* right
	6. Delay 15 seconds
	7. Turn steering to *?0 degrees?*
	8. Apply throttle at *?throttle_min%?*
	9. Apply brake at *?50%?*
	10. Wait for the vehicle to come to a complete stop
	11. Maintain brake pressure
	12. *Expected Results:* - Vehicle has performed a right turn

7. Accelerate to the right and left with traction control disabled
	1. *Initial Conditions:* - Vehicle is in drive with the brake applied, traction control disabled
	2. Release brake
	3. Apply throttle at *?30%?*
	4. Maintain speed during maneuvers
	5. Turn steering to *?max degrees?* left
	6. Delay 15 seconds
	7. Turn steering to *?0 degrees?*
	8. Apply throttle at *?throttle_min%?*
	9. Apply brake at *?50%?*
	10. Wait for the vehicle to come to a complete stop
	11. Maintain brake pressure
	12. *Expected Results:* - Vehicle has performed a right turn

#### Driver Disable Detection
1. Driver lightly taps gas pedal
	1. *Initial Conditions:* - OSCC is enabled, vehicle in drive, brake applied via remote
	2. Release brake
	3. Driver applies the gas pedal at *?10%?*
	4. *Expected Results:* - OSCC disengages when driver presses gas pedal

2. Driver stomps on gas pedal hard
	1. *Initial Conditions:* - OSCC is enabled, vehicle in drive, brake applied via remote
	2. Release brake
	3. Driver applies the gas pedal at *?max-throttle?*
	4. *Expected Results:* - OSCC disengages when driver presses gas pedal

3. Driver lightly taps brake pedal
	1. *Initial Conditions:* - OSCC is enabled, vehicle in drive, brake applied via remote
	2. Release brake
	3. Driver taps the brakes as they might to disable cruise control *?5%?*
	4. *Expected Results:* - OSCC disengages when driver taps the brakes

4. Driver stomps on the brakes
	1. *Initial Conditions:* - OSCC is enabled, vehicle in drive, brake applied via remote
	2. Release brake
	3. Driver depresses the brakes in an attempt to engage the ABL *?95%?*
	4. *Expected Results:* - OSCC disengages when driver taps the brakes

5. Driver lightly turns the wheel left and right
	1. *Initial Conditions:* - OSCC is enabled, vehicle in park
	2. Driver turns the wheel slowly to the left
	3. Reenable OSCC
	4. Driver turns the wheel slowly to the right
	5. *Expected Results:* - OSCC disengages when driver turns the steering wheel

6. Driver yanks the wheel to the left and right
	1. *Initial Conditions:* - OSCC is enabled, vehicle in park
	2. Driver turns the wheel quickly to the left
	3. Reenable OSCC
	4. Driver turns the wheel quickly to the right
	5. *Expected Results:* - OSCC disengages when driver turns the steering wheel

#### Diagnostic Operation
1. Detect DAC to ADC failure case on the throttle and steering boards
	1. The points of this test is to demonstrate that diagnostic detects a failure of the DAC
	2. The method of this determination is still TBD

2. Detect PWM to ADC failure case on the PWM for the brake board
	1. The points of this test is to demonstrate that diagnostic detects a failure of the PWM
	2. The method of this determination is still TBD

3. Detect ADC failure - Steering mismatch
	1. The points of this test is to demonstrate that diagnostic detects a failure of the ADC
	2. The method of this determination is still TBD

4. Detect ADC failure - throttle mismatch
	1. The points of this test is to demonstrate that diagnostic detects a failure of the ADC
	2. The method of this determination is still TBD

5. Losing CAN communication causes disable
	1. *Initial Conditions* - OSCC is enabled, vehcle in park
	2. Disconnect CAN network from OSCC modules
	3. *Expected Results:* OSCC should disable in ~250ms

6. Delayed CAN communication causes disable
	1. *Initial Conditions* - OSCC is enabled, vehcle in park, OSCC CAN gateway loaded with test FW
	2. Connect to the CAN gateway module and send a request to "drop a packet."
	3. This causes the custom FW, to stop sending CAN update for 500ms at which point it will start again
	4. *Expected Results:* OSCC should disable in ~250ms

---

# OSCC Coding standard

As this is an automotive initiative, this coding standard is based on the MISRA C-2012 standard. The reasoning for the given rule or directive in the coding standard is not included in this as it would create too large of a document.

The MISRA standard is more exacting that the OSCC standard. It is not implied that compliance with this standard guarantees compliance with the MISRA standard.

## 1. Directives

1. Implemented behavior must be documented and understood
2. Comply with the C99 definition of the standard
3. All source files must compile without errors
4. Run-time failures must be limited
   1. All arithmetic operations must be checked (e.g. divide by zero)
   2. Pointer arithmetic must be checked
   3. Array bounds must be checked
   4. Parameter validity must be checked either before or inside a function call
   5. Pointer validity must be checked before dereference
   6. Avoid dynamic memory allocation
      1. If used, check validity of allocation

5. Avoid assembly language usage.
   1. If used, then document, encapsulate and isolate all usage

6. No “commented out” code
   1. C Style: `/- ... */`
   2. C++ Style: `//`
   3. Conditional Compilation Style: `#if 0 .. #endif`

7. Prefer the use of types that define the size and signed value of the variable
   1. `int8_t, int16_t, int32_t, int64_t`
   2. `uint8_t, uint16_t, uint32_t, uint64_t`
   3. `float32_t, float64_t, float128_t`
   4. If abstract types are required, use the signed and size types to create the new type
   5. Exceptions:
      1. `bool`, `char`, `unsigned char`, `float` and `double` are acceptable for embedded code as they all have specific signs and sizes associated with them
      2. User space applications have more flexibility and can use the intrinsic types: `long`, `unsigned long`, `int` and `unsigned int`
   6. `struct`s shall have a '_s' appended to the name and have the following form:
    ```c
	   // Example struct
	   struct torque_spoof_s
	   {
		   uint16_t low;
		   uint16_t high;
	   };
    ```
   7. type defined structures shall conform to the structure rules and have the following form:
	```c
	   #define CAN_FRAME_DLC_MAX (8)
	   // Example type defined structure
	   typedef struct
	   {
		   uint32_t id;
		   uint8_t  dlc;
		   uint32_t timestamp;
		   uint8_t  data[CAN_FRAME_DLC_MAX];
	   } can_frame_s;
	```

8. Prefer function calls to function-like macros
   1. Macros do not perform type-checking during compilation
   2. `inline` can be used, but is subject to compiler interpretation

9. Prevent header files from being included more than once
   1. Use the `#ifndef HEADER_FILE_H, #define HEADER_FILE_H ... #endif` mechanism

10. Check validity of parameters passed to library functions
	1. e.g. math calls and string calls

11. Restrict dynamic memory allocation
	1. Allocate all required memory during initialization
	2. If the allocation fails, prevent the system from starting

12. Sequence access to system resources appropriately
	1. File access - open, read/write, close
	2. Register access - atomic interaction
	3. Prefer to create a system object that manages the interaction (i.e. the singleton pattern)

## 2. Rules

- [1. Standard Environment](#1-standard-environment)
- [2. Unused code](#2-unused-code)
- [3. Comments](#3-comments)
- [4. Identifiers](#4-identifiers)
- [5. Types](#5-types)
- [6. Declarations](#6-declarations)
- [7. Initialization](#7-initialization)
- [8. Pointers](#8-pointers)
- [9. Expressions](#9-expressions)
- [10. Side Effects](#10-side-effects)
- [11. Control Statements](#11-control-statements)
- [12. Control Flow](#12-control-flow)
- [13. Switch Statements](#13-switch-statements)
- [14. Functions](#14-functions)
- [15. Pointers and Arrays](#15-pointers-and-arrays)
- [16. Preprocessor Directives](#16-preprocessor-directives)
- [17. Standard Libraries](#17-standard-libraries)
- [18. Resources](#18-resources)


### 1. Standard Environment

1. Prefer to use only the language features that are available in the C99 standard.

### 2. Unused code

1. A project should contain no unreachable or dead code
2. A project should not have any unused type declarations
   1. Includes enumerations, unions, structures, and native types
3. A project should not contain any unused macros
4. A project should not contain any unused labels
5. A project should not contain any unused parameters
   1. Passing a parameter to another function constitutes usage

### 3. Comments

1. Comments shall not mix C and C++ style comment markers
   1. C and C++ style comment markers are both allowed

### 4. Identifiers

1. Variables and functions in the same scope (and namespace) shall be distinguishable
   in the first 63 characters (by the C99 standard)

2. Variables in an inner scope shall not hide a variable in an outer scope
   (e.g. do not use a local variable of the same name as a function parameter or a global variable)

3. Macros must be distinct from variables

4. Type definitions must be unique

5. Structure, union, and enumeration tags must be unique

6. Variable, macro and function naming should use “snake case”
   1. Variables and functions should use descriptive names which are all lowercase and separated by an underscore
       - variable: `int32_t range_max;`
       - function: `int16_t send_can_frame( can_frame* transmit_frame );`
   1. Macros should use descriptive names which are all uppercase and separated by an underscore
       - macro: `#define TOGGLE_BIT( BIT, X ) ( ( X ) ^= ( 1 << ( BIT ) ) )`

7. All names (variable, macro, function or otherwise) should be human readable and avoid the use of abbreviations
	1. e.g. The macro name PEDAL_THRESHOLD should be used instead of PEDAL_THRESH


### 5. Types

1. Prefer bit access macros to bit-field usage (see below)
   1. Bit fields must use an integer or unsigned integer type
   2. Single bit fields must use an unsigned integer type
   3. Bit fields are not necessarily portable as they are subject to compiler implementation and processor architecture
   4. Bit access Macros of the form `BITX( 1 << X )` where X is in \[ 0 .. variable width \] should be used

2. String literals must be assigned to `const char*` variables
   1. This includes when being used as a parameter or return value

3. Avoid type conversions between boolean values and other intrinsic types

4. Operations should occur between objects of the same type (e.g. arithmetic operation should be performed on objects of the same type)

5. Prefer to capture the result of an operation in an object type of the same size
   1. Exceptions exist when complying with a communication protocol or hardware register width
   2. Rule applies to both narrowing and widening the result of an operation

6. Unions (overlapped storage) are only allowed when the items in the union are all the same size

  ```c
  // Example union is allowed because each element is 64 bits
  union my_array_64bits
  {
      char     char_array[8];
      int16_t  int_array[4];
      int32_t  int_array[2];
      float    float_array[2];
      double   double_array[1];
  };
  ```

### 6. Declarations

1. All types must be explicitly specified
   1. Leave nothing in a declaration to implication
   2. Objects
   3. Parameters
   4. Structure and union members
   5. Type definitions
   6. Function return types

2. Functions should be in prototype form (as opposed to K & R style)
   1. It is acceptable to have an empty set of parentheses instead of a void declaration as in:

   `int32_t my_function( );`

   1. Prototype form means that the parameter types are included in the function parameter list as in:

   `int32_t my_next_function( int32_t parameter1 );`

3. Externally available objects or functions shall be declared only once in one file

4. Prefer the use of `static` keyword to limit access to functions and objects that are not accessible externally

5. Prefer object declaration to be done at the block scope where it is used (e.g. local variables can be declared at the function or block scope)

6. Arrays with external linkage should have their size explicitly stated

7. Values for variables in enumerations must be unique

8. Prefer the use of const pointer unless the pointer is to a value that is being modified

9. The `restrict` keyword is not allowed

### 7. Initialization

1. An automatic variable (e.g. a local variable) must not be read before it has been initialized

2. Enclose initializations for array, structure, or union variables with values enclosed in braces:
   1. `int16_t my_array[ 4 ] = { 0, 1, 2, 3 };`
   2. `char` arrays may also be initialized with a string literal
   3. The stipulation is that the initializer must be internally consistent with the target array, structure, or union
   4. If an array is statically initialized the size must be explicitly stated as in `my_array[ 4 ]` above
   5. Arrays shall not be partially initialized

3. Static initialization shall not include any side effects (e.g. pointer dereference and increment allowed at run-time)

### 8. Pointers

1. Pointer conversions are allowed when:
   1. The conversion is between a pointer and bool
   2. The conversion is from a void type and an intrinsic type
   3. The conversion is between compatible types
   4. The conversion is to a char or unsigned char, so that the bytes of an object can be accessed directly

2. Function pointers cannot be converted

3. Pointer conversions between typed objects and integers are not allowed

4. Problems with pointer conversions are mostly concerned with alignment problems and accessing beyond the bounds of the size of the object

5. Always use the macro NULL to reference an invalid or uninitialized pointer

### 9. Expressions

1. Always use parentheses to enforce explicit precedence

2. For shift operations, the amount shifted must be &gt;= 0 and &lt; sizeof( value being shifted )

3. Do not use the comma operator
   1. Multiple initialization on the same line
   2. For loops must iterate and act upon a single variable

4. Always check for overflow on arithmetic operations on unsigned variables

### 10. Side Effects

1. Initializer lists must not have side effects
   1. No modification of pointers during initialization
   2. Make all statements explicit

2. Check macros to prevent side-effects in calls (e.g. What happens when you call `DO_COPY_INDEX ( i++ )`?)
  ```
  #define DO_COPY_INDEX ( x ) ( a [ ( x ) ] = b [ ( x ) ] )
  ```


3. Prefer that increment and decrement operators ( `++` and `--` ) are explicit statements instead of integral in an operation, e.g.
   1. Macros
   2. Logical comparisons ( `&&` and `||` )
   3. `sizeof`

4. Do not use the result of an assignment operation as a variable or input into another statement

5. Logical comparisons should not contain persistent side effects such as assignments

### 11. Control Statements

1. Always use spaces instead of tabs
   1. Use 4 space as the standard indent

2. Include a space after an opening parenthesis, brace, or bracket and a space before the closing match.
   1. Parentheses: `( to_b || not_to_b )`
   2. Brace: `array[ 4 ] = { 0, 1, 2, 3 };`
   3. Bracket: `array[ 4 ] = { 0, 1, 2, 3 };`

3. Variable and function naming should use “snake case,” namely descriptive names which are all lowercase and separated by an underscore
   1. `int32_t this_is_an_example_of_a_variable;`

4. `if` statements shall have the following form:

	```c
	int32_t sum;
	const static int32_t max = 25;

	if ( sum > max )
	{
	    sum = max;
	}
	```

5. `for` loops shall have the following form:

	```c
	int32_t i;
	int32_t sum = 0;
	int32_t max = 25;

	for ( i = 0; i < max; i++ )
	{
	    sum += i;
	}
	```

6. `while` loops shall have the following form:

	```c
	int32_t i = 0;
	int32_t sum = 0;
	int32_t max = 25;

	while ( i < max )
	{
	    sum += i;
	    i++;
	}
	```

7. `do` loops shall have the following form:

	```c
	int32_t i = 0;
	int32_t sum = 0;
	int32_t max = 25;

	do
	{
	    sum += i;
	    i++;
	} ( while i < max );

	```

8. Prefer that loops exit using control logic instead of a `break`  statement

9. Loop counter should not use a floating type as the iterator

10. The control logic to exit a given loop must be controlled by a variable

    1. This prevents statements that are always true or always false
    2. Exceptions:
        1. `for ( ; ; )`
        2. `while ( 1 )` or `while ( true )`

11. The control logic for a statement used in if, for, while and do must resolve to a boolean expression

    1. Checking for the validity of a variable like an integer or pointer is not allowed (e.g. `while ( i )` is not acceptable where `while ( i != 0 )` is)

### 12. Control Flow

1. Prefer to not use `goto`
   1. If `goto` is required, the `goto` label must be within the same function

2. Prefer to limit the number of `break` statements to one for a given loop

3. Functions should have a single point of exit at the end

	```c
	uint32_t example_function( uint32_t- pointer_parameter )
	{
	    uint32_t return_code = 0;

	    if ( pointer_parameter != NULL )
	    {
	        ...
	        return_code = 1;
	    }
	    else
	    {
	        ...
	        return_code = 10;
	    }
	    return ( return_code );
	}
	```

4. Prefer to keep functions as small as possible
5. Duplicate code is not allowed
6. Statements shall always use braces to delineate the scope of their operation

   1. As with a function, `if`, `while`, `do`, `for`, always use braces to scope the set of statements or other functions that the conditional controls

7. All `if`, `else if`, trees shall be terminated with an `else` statement

### 13. Switch Statements

1. Prefer `if`, `else if`, `else` construct to `switch` statements
   1. If the `switch` statement is required, it must be well-formed
   2. Each `case` is self-contained and must contain a `break` statement
   3. Falling through to the next `case` is only allowed when there is an empty `case`
   4. All `switch` statements must contain a final `default` case
   5. `switch` statements must contain at least 2 `cases` and a `default`

	```c
	uint32_t input_parameter;

	switch ( input_parameter )
	{
	    case 1:
	    ...
	    break;

	    case 2:
	    case 3:
	    {
	        uint32_t local_scope_variable;
	        ...
	        break;
	    }

	    default:
	    break;
	}
	```

### 14. Functions

1. Variable argument lists are not allowed (e.g. `va_arg`, `va_start`, `va_end`)

   1. printf or its equivalent is allowed for debug only

2. Recursion is not allowed

3. All functions require prototypes

4. Prefer that return values from functions are used

### 15. Pointers and Arrays

1. Check that pointer arithmetic does not go beyond the bounds of the array to which the pointer refers

2. Avoid pointer subtraction

   1. Pointer subtraction is only allowed for pointers that refer to the same array

3. Avoid pointer comparison operations

   1. Pointer comparison can be used to determine if a pointer is in the bounds of the array that it refers to

4. Pointers should not go beyond two levels of nesting ( i.e. Pointers to pointers are allowed )

5. Pointers should not be for stack variables or other objects with automatic storage

6. Check that arrays of variable size are of positive size

### 16. Preprocessor Directives

1. `#include` directives should be preceded only by comments or other preprocessor directives

2. Macros shall not be named the same as a keyword

3. Prefer that `#undef` is not used

4. Macros should not contain preprocessor directives (side effects)

5. Expressions from macro expansion should be surrounded by parentheses

6. `#if` and `#elif` expressions should resolve as 0 or 1

7. Prefer that the `#` and `##` directives not be used

### 17. Standard Libraries

1. Avoid `#define` and `#undef` on reserved identifiers or macros

2. Avoid declaring a reserved identifier or macro

   1. e.g. do not create a variable called “float”

3. Standard library usage is acceptable

### 18. Resources

1. Free all allocated memory

2. Do not open files for read and write at the same time, file access is read-only or write-only

3. Do not dereference a FILE object pointer

4. Close all system resources before shutdown
