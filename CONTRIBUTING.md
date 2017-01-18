# How to contribute

There are many ways to contribute to the OSCC project; support for additional vehicles can be added, diagrams can be made to look better, car systems can be better described,
CAN frames can be better detailed, PCBs can be optimized
or code could be refactored and improved.

The directory structure of the project is created in such a way that adding support for additional vehicles should be simple and intuitive.
All vehicle specific directories should be recreated for each additional vehicle. The naming convention for is <make>_<model>_<generational identifier>
Below is a sample of how additional vehicle directories should be created.


    .
    ├── firmware
    │   ├── ...
    │   ├── brake
    │   │   ├──kia_soul_ps
    │   │   ├──<my_new_car>       # Brake firmware for vehicle you're adding
    │   ├── ...
    ├── vehicle_info
    │   ├──kia_soul_ps
    │   ├──<my_new_car>           # Vehicle specific information for vehicle you're adding
    └── ...


## Getting Started

* Using the repo's Issues section, create a new issue and:
  * Clearly describe the issue including steps to reproduce when it is a bug
  * Create a branch using `git fork` in the repository on GitHub

## Making Contributions

* Do not work directly on the `master` branch; changes to the master branch will be removed
* Create a topic branch from where you want to base your work
  * You'll likely `fork` from of the master branch
  * To quickly create a topic branch based on master:
  ```
  git checkout -b fix/master/my_contribution master
  ```

* Write a good commit message.
#### Commit Messages
* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Move resistor to..." not "Moves resistor to...")
* Limit the first line to 72 characters or less
* Reference issues and pull requests specifically
* When changing documentation only, include [ci skip] in the commit description

### Contributing Code

* Code should conform to the coding standard (see below)
* Push your changes to a topic branch in your branch of the repository
* Submit a pull request to the repository in the PolySync organization
* Update your github issue to mark that you have submitted code and are ready for it to be reviewed (Status: Ready for Merge)
  * Include a link to the pull request in the ticket
* The PolySync team will review all pull requests on a weekly basis
* __Code contributed should include unit tests__ that demonstrate the code functions as expected

### Contributing a Diagram

* The diagrams were created using the Eagle PCB Design tool (https://cadsoft.io/#)
    * All board diagrams should use this tool
* Modifications should include tests to demonstrate the the modified board works as expected

### Submitting Enhancement Requests

Enhancement suggestions are tracked as GitHub issues:

* Use a clear and descriptive title for the issue to identify the suggestion
* Provide a step-by-step description of the suggested enhancement in as many details as possible
* Provide specific examples to demonstrate the steps
    * Include snippets in the examples as Markdown code blocks ( using the \`\`\` format )
* Describe the current behavior and how this enhancement improves or changes it
* Explain why this enhancement is useful to most users
* Specify which version of OSCC this change applies to
    * The versions can be found in the Releases tab on GitHub

### Pull Request Labels
* Fill in the required template
* Follow the style guides
* Code should be complete
    * Any changes should include unit tests for the submitted
* End files with a new line
* Changes should be located in the most appropriate directory
* Please open an issue to suggest a new label.

## Directory Contents

This information is also available in the README.

| Directory | Contents |
| ---------:|:-------- |
| 3d_tools | Technical drawings and 3D files for board enclosures and other useful parts |
| assets | Diagrams and images used in the wiki |
| boards | PCB schematics and board designs for control modules that are integrated with the vehicle, typically these are the throttle, steering and brake boards, but can include other items should they be contributed. |
| control | User Space applications to provide mechanisms for the user to control the vehicle directly via something like a PC.  Applications can be command-line or GUI based and include items like a joystick controller |
| firmware | Arduino code for the throttle, steering and brake functionality.  This directory also includes code for the CAN gateway module, which uses a stock Arduino board. The firmware should remain modular and portable to a a different Arduino board |
| vehicle_info | Information on specific vehicles such as sensor outputs and wiring diagrams.  Each additional vehicle should be in its own subdirectory |

## Diagrams

Adding diagrams for new boards or other components should be located in the directory most appropriate for their function:
* A new enclosure should go in the 3d_models directory
* A new board diagram should go in the boards directory
* Any new vehicle information should reside in the vehicle_info directory

### Editing diagrams

Existing diagrams should be edited using the Autodesk Eagle as mentioned above.

## Building

To build the various components in the system, please see the README.md

## Documentation

* Documentation updates are welcome
* Documentation should be located in the *doc* directory where it is most applicable (See the Directory descriptions above)
	* If the *doc* directory does not exist, please create it as part of the submission
* Documentation pull requests function in the same way as other pull requests

### Documentation Style

* Documentation should be in markdown format

## Helpful Links

* [Autodesk Eagle](https://cadsoft.io)
* [Arduino FAQ](https://www.arduino.cc/en/Main/FAQ)
* [Throttle/Steering/CAN Gateway board](https://www.arduino.cc/en/Main/ArduinoBoardUno)
* [Braking board](https://www.arduino.cc/en/Main/arduinoBoardMega2560)

# OSCC Coding standard

As this is an automotive initiative, this coding standard is based on
the MISRA C-2012 standard. The reasoning for the given rule or directive in the
coding standard is not included in this as it would create much too large a
document.

The MISRA standard is more exacting that the OSCC standard. It is not
implied that compliance with this standard guarantees compliance with
the MISRA standard.

## 1. Directives

1. Implemented behavior must be documented and understood
1. Comply with the C99 definition of the standard
1. All source files must compile without errors
1. Run-time failures must be limited
	1. All arithmetic operations must be checked (e.g. divide by zero)
	1. Pointer arithmetic must be checked
	1. Array bounds must be checked
	1. Check validity of parameters either before or inside a function call
	1. Check pointer validity before dereference
	1. Avoid dynamic memory allocation
		1. If used, check validity of allocation

1. Avoid assembly language usage.
	- If used:
		- Document all usage
		- Encapsulate and isolate usage

1. No “commented out” code
	1. C Style: `/* ... */`
	1. C++ Style: `//`
	1. Conditional Compilation Style: `#if 0 .. #endif`

1. Prefer the use of types that define the size and signed value of the variable
	1. `int8_t, int16_t, int32_t, int64_t`
	1. `uint8_t, uint16_t, uint32_t, uint64_t`
	1. `float32_t, float64_t, float128_t`
	1. If abstract types are required, use the signed and size types to create the new type
	1. Exceptions:
		1. `char`, `unsigned char`, `float` and `double` are acceptable for embedded code as they all have specific signs and sizes associated with them
		1. User space applications have more flexibility and can use the intrinsic types: `long`, `unsigned long`, `int` and `unsigned int`

1. Prefer function calls to function-like macros
	1. Macros do not perform type-checking during compilation
	1. `inline` can be used, but is subject to compiler interpretation

1. Prevent header files from being included more than once
	1. Use the `#ifndef HEADER_FILE_H, #define HEADER_FILE_H ... #endif` mechanism

1. Check validity of parameters passed to library functions
	1. E.g. math calls and string calls

1.   Restrict dynamic memory allocation
	1. Allocate all required memory during initialization
 	1. If the allocation fails, prevent the system from starting

1. Sequence access to system resources appropriately
	1. File access - open, read/write, close
	1. Register access - atomic interaction
	1. Prefer to create a system object that manages the interaction (i.e. the singleton pattern)

## 2. Rules

### 1. Standard Environment

1. Prefer to use only the language features that are available in the C99 standard.

### 2. Unused code

1. A project should contain no unreachable or dead code
1. A project should not have any unused type declarations
	1. Includes enumerations, unions, structures and native types
1. A project should not contain any unused macros
1. A project should not contain any unused labels
1. A project should not contain any unused parameters
	1. Passing a parameter to another function constitutes usage

### 3. Comments

1. Comments shall not contain C or C++ style comment markers

### 4. Identifiers

1. Variables and functions in the same scope (and namespace) shall be distinguishable
in the first 63 characters (by the C99 standard)

1. Variables in an inner scope shall not hide a variable in an outer scope
(e.g. do not use a local variable of the same name as a function parameter or a global variable)

1. Macros must be distinct from variables

1. Type definitions must be unique

1. Structure, union, and enumeration tags must be unique

### 5. Types

1. Prefer bit access macros to bit-field usage (see below)
	1. Bit fields must use an integer or unsigned integer type
	1. Single bit fields must use an unsigned integer type
	1. Bit fields are not necessarily portable as they are subject to compiler implementation and processor architecture
	1. Bit access Macros of the form `BITX( 1 << X )` where X is in \[ 0 .. variable width \] should be used

1. String literals must be assigned to `const char*` variables
	1. This includes when being used as a parameter or return value

1. Avoid type conversions between boolean values and other intrinsic  types

1. Operations should between objects of the same type (e.g. arithmetic operation should be performed on objects of the same type)

1. Prefer to capture the result of an operation in an object type of the same size
	1. Exceptions exist when complying with a communication protocol or  hardware register width
	1. Rule applies to both narrowing and widening the result of an operation

1. Unions (overlapped storage) are only allowed when the items in the union are all the same size

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
	1. Objects
	1. Parameters
	1. Structure and union members
	1. Type definitions
	1. Function return types

1. Functions should be in prototype form (as opposed to K & R style)
	1. It is acceptable to have an empty set of parentheses instead of a void declaration as in:

	`int32_t my_function( );`

	1. Prototype form means that the parameter types are included in the function parameter list as in:

	`int32_t my_next_function( int32_t parameter1 );`

1. Externally available objects or functions shall be declared only once in one file

1. Prefer the use of `static` keyword to limit access to functions and objects that are not accessible externally

1. Prefer object declaration to be done at the block scope where it is used (e.g. local variables can be declared at the function or block scope)

1. Arrays with external linkage should have their size explicitly stated

1. Values for variables in enumerations must be unique

1. Prefer the use of const pointer unless the pointer is to a value that is being modified

1. The `restrict` keyword is not allowed

### 7. Initialization

1. An automatic variable (e.g. a local variable) must not be read before it has been initialized

1. Enclose initializations for array, structure or union variables with values enclosed in braces:
	1. `int16_t my_array[ 4 ] = { 0, 1, 2, 3 };`
	1. `char` Arrays may also be initialized with a string literal
	1. The stipulation is that the initializer must be internally consistent with the target array, structure or union
	1. If an array is statically initialized the size must be explicitly stated as in `my_array[ 4 ]` above
	1. Arrays shall not be partially initialized

1. Static initialization shall not include any side effects (e.g. pointer dereference and increment which is allowed at run-time)

### 8. Pointers

1. Pointer conversions are allowed when:
	1. The conversion is between a pointer and bool
	1. The conversion is from a void type and an intrinsic type
	1. The conversion is between compatible types
	1. The conversion is to a char or unsigned char so that the bytes of an object can be accessed directly

1. Function pointers cannot be converted

1. Pointer conversions between typed objects and integers are not allowed

1. Problems with pointer conversions are mostly concerned with alignment problems and accessing beyond the bounds of the size of the object

1. Always use the macro NULL to reference an invalid or uninitialized pointer

### 9. Expressions

1. Always use parentheses to enforce explicit precedence

1. For shift operations, the amount shifted must be &gt;= 0 and &lt; sizeof( value being shifted )

1. Do not use the comma operator
	1. Multiple initialization on the same line
	1. For loops must iterate and act upon a single variable

1. Always check for overflow on arithmetic operations on unsigned variables

### 10. Side Effects

1. Initializer lists must not have side effects
	1. No modification of pointers during initialization
	1. Make all statements explicit

1. Check macros to prevent side-effects in calls (e.g. What happens when you call `DO_COPY_INDEX ( i++ )`?)
```
#define DO_COPY_INDEX ( x ) ( a [ ( x ) ] = b [ ( x ) ] )
```


1. Prefer that increment and decrement operators ( `++` and `--` ) are explicit statements instead of integral in an operation, e.g.
	1. Macros
	1. Logical comparisons ( `&&` and `||` )
	1. `sizeof`

1. Do not use the result of an assignment operation as a variable or input into another statement

1. Logical comparisons should not contain persistent side effects such as assignments

### 11. Control Statements

1. Always use spaces instead of tabs
	1. Use 4 space as the standard indent

1. Include a space after an opening parenthesis, brace or bracket and a space before the closing match.
	1. Parentheses: `( to_b || not_to_b )`
	1. Brace: `array[ 4 ] = { 0, 1, 2, 3 };`
	1. Bracket: `array[ 4 ] = { 0, 1, 2, 3 };`

1. Variable and function naming should use “snake case”, namely descriptive names which are all lowercase and separated by an underscore
	1. `int32_t this_is_an_example_of_a_variable;`

1. For loops shall have the following form:

```c
int32_t i;
int32_t sum = 0;
int32_t max = 25;

for ( i = 0; i < max; i++ )
{
    sum += i;
}
```

1. While loops shall have the following form:

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

1. Do loops shall have the following form:

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

1. Prefer that loops exit using control logic instead of a `break`  statement

1. Loop counter should not use a floating type as the iterator

1. The control logic to exit a given loop must be controlled by a variable

    1. This prevents statements that are always true or always false
    1. Exceptions:
        1. `for ( ; ; )`
		1. `while ( 1 )` or `while ( true )`

1. The control logic for a statement used in if, for, while and do must resolve to a boolean expression

	1. Checking for the validity of a variable like an integer or pointer is not allowed (e.g. `while ( i )` is not acceptable where `while ( i != 0 )` is)

### 12. Control Flow

1. Prefer to not use `goto`
	1. If `goto` is required, the `goto` label must be within the same function

1. Prefer to limit the number of `break` statements to one for a given loop

1. Functions should have a single point of exit at the end

```c
uint32_t example_function( uint32_t* pointer_parameter )
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

1. Prefer to keep functions as small as possible
1. Duplicate code is not allowed
1. Statements shall always use braces to delineate the scope of their operation

	1. As with a function, `if`, `while`, `do`, `for`, always use braces to scope the set of statements or other functions that the conditional controls

1.   All `if`, `else if`, trees shall be terminated with an `else` statement

### 13. Switch Statements

1. Prefer `if`, `else if`, `else` construct to `switch` statements
	1. If the `switch` statement is required, it must be well-formed
	1. Each `case` is self-contained and must contain a `break` statement
	1. Falling through to the next `case` is only allowed when there is an empty `case`
	1. All `switch` statements must contain a final `default` case
	1. `switch` statements must contain at least 2 `cases` and a `default`

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

1. Recursion is not allowed

1. All functions require prototypes

1. Prefer that return values from functions are used

### 15. Pointers and Arrays

1. Check that pointer arithmetic does not go beyond the bounds of the array to which the pointer refers.

1. Avoid pointer subtraction
	1. Pointer subtraction is only allowed for pointers that refer to the same array

1. Avoid pointer comparison operations
	1. Pointer comparison can be used to determine if a pointer is in the bounds of the array that it refers to.

1. Pointers should not go beyond two levels of nesting ( i.e. Pointers to pointers are allowed )

1. Pointers should not be for stack variables or other objects with automatic storage

1. Check that arrays of variable size are of positive size

### 16. Preprocessor Directives

1. `#include` directives should be preceded only by comments or other preprocessor directives

1. Macros shall not be named the same as a keyword

1. Prefer that `#undef` is not used

1. Macros should not contain preprocessor directives (side effects)

1. Expressions from macro expansion should be surrounded by parentheses

1. `#if` and `#elif` expressions should resolve as 0 or 1

1. Prefer that the `#` and `##` directives not be used

### 17. Standard Libraries

1. Avoid `#define` and `#undef` on reserved identifiers or macros

1. Avoid declaring a reserved identifier or macro

	- e.g. do not create a variable called “float”

1. Standard library usage is acceptable

### 18. Resources

1. Free all allocated memory

1. Do not open files for read and write at the same file, file access is read-only or write only

1. Do not dereference a FILE object pointer

1. Close all system resources before shutdown

