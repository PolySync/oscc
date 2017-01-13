# OSCC Coding standard

As this is an automotive initiative, this coding standard is based on
the MISRA C-2012 standard. The reasoning for the given standard is not
included in this as it would create much too large a document.

The MISRA standard is more exacting that the OSCC standard. It is not
implied that compliance with this standard guarantees compliance with
the MISRA standard.

## Directives

- Implemented behavior must be documented and understood
- Comply with the C99 definition of the standard
- All source files must compile without errors
- Run-time failures must be limited
	- All arithmetic operations must be checked (e.g. divide by zero)
	- Pointer arithmetic must be checked
	- Array bounds must be checked
	- Check validity of parameters either before or inside a function call
 	- Check pointer validity before dereference
	- Avoid dynamic memory allocation
		- If used, check validity of allocation

- Avoid assembly language usage.
	- If used:
		- Document all usage
		- Encapsulate and isolate usage

- No “commented out” code
	- C Style: `/* ... */`
	- C++ Style: `//`
 	- Conditional Compilation Style: `#if 0 .. #endif`

- Prefer the use of types that define the size and signed value of the variable
	- `int8_t, int16_t, int32_t, int64_t`
 	- `uint8_t, uint16_t, uint32_t, uint64_t`
	- `float32_t, float64_t, float128_t`
	- If abstract types are required, use the signed and size types to create the new type
 	- Exception: `char`, `unsigned char`, `float` and `double` are acceptable as they all have specific signs and sizes associated with them

- Prefer function calls to function-like macros
	- Macros do not perform type-checking during compilation
	- `inline` can be used, but is subject to compiler interpretation

- Prevent header files from being included more than once
	- Use the `#ifndef HEADER_FILE_H, #define HEADER_FILE_H ... #endif` mechanism

- Check validity of parameters passed to library functions
	- E.g. math calls and string calls

-   Restrict dynamic memory allocation
	- Allocate all required memory during initialization
 	- If the allocation fails, prevent the system from starting

- Sequence access to system resources appropriately
	- File access - open, read/write, close
	- Register access - atomic interaction
	- Preference is to create a system object that manages the interaction (i.e. the singleton pattern)

## Rules

### Standard Environment

- Prefer to use only the language features that are available in the C99 standard.

### Unused code

- A project should contain no unreachable or dead code
- A project should not have any unused type declarations
	- Includes enumerations, unions, structures and native types
- A project should not contain any unused macros
- A project should not contain any unused labels
- A project should not contain any unused parameters
	- Passing a parameter to another function constitutes usage

### Comments

- Comments shall not contain C or C++ style comment markers

### Identifiers

- Variables and functions in the same scope (and namespace) shall be distinguishable in the first 63 characters (by the C99 standard)

- Variables in an inner scope shall not hide a variable in an outer scope
	- E.g. do not use a local variable of the same name as a function parameter or a global variable

- Macros must be distinct from variables

- Type definitions must be unique

- Structure, union, and enumeration tags must be unique

### Types

- Prefer bit access macros to bit-field usage (see below)
	- Bit fields must use an integer or unsigned integer type
	- Single bit fields must use an unsigned integer type
	- Bit fields are not necessarily portable as they are subject to compiler implementation and processor architecture
	- Bit access Macros of the form `BITX( 1 << X )` where X is in \[ 0 .. variable width \] should be used

- String literals must be assigned to `const char*` variables
	- This includes when being used as a parameter or return value

- Avoid type conversions between boolean values and other intrinsic  types

-   Operations should between objects of the same type
	- E.g. arithmetic operation should be performed on objects of the same type

- Prefer to capture the result of an operation in an object type of the same size

	- Exceptions exist when complying with a communication protocol or  hardware register width
	- Rule applies to both narrowing and widening the result of an operation

### Declarations

- All types must be explicitly specified
	- Leave nothing in a declaration to implication
	- Objects
	- Parameters
	- Structure and union members
	- Type definitions
	- Function return types

- Functions should be in prototype form (as opposed to K & R style)
	- It is acceptable to have an empty set of parentheses instead of a void declaration as in:

	`int32_t my_function( );`

	-  Prototype form means that the parameter types are included in the function parameter list as in:

	`int32_t my_next_function( int32_t parameter1 );`

- Externally available objects or functions shall be declared only once in one file

- Prefer the use of `static` keyword to limit access to functions and objects that are not accessible externally

- Prefer object declaration to be done at the block scope where it is used
	- E.g. local variables can be declared at the function or block scope

- Arrays with external linkage should have their size explicitly stated

- Values for variables in enumerations must be unique

- Prefer the use of const pointer unless the pointer is to a value that is being modified

- The `restrict` keyword is not allowed

### Initialization

- An automatic variable (e.g. a local variable) must not be read before it has been initialized

- Enclose initializations for array, structure or union variables with values enclosed in braces:
	- `int16_t my_array[ 4 ] = { 0, 1, 2, 3 };`
	- `char` Arrays may also be initialized with a string literal
	- The stipulation is that the initializer must be internally consistent with the target array, structure or union
	- If an array is statically initialized the size must be explicitly stated as in `my_array[ 4 ]` above
	- Arrays shall not be partially initialized

- Static initialization shall not include any side effects
	- E.g. pointer dereference and increment which is allowed at run-time

### Pointers

- Pointer conversions are allowed when:
	- The conversion is between a pointer and bool
	- The conversion is from a void type and an intrinsic type
	- The conversion is between compatible types
	- The conversion is to a char or unsigned char so that the bytes of an object can be accessed directly

- Function pointers cannot be converted

- Pointer conversions between typed objects and integers are not allowed

- Problems with pointer conversions are mostly concerned with alignment problems and accessing beyond the bounds of the size of the object

- Always use the macro NULL to reference an invalid or uninitialized pointer

### Expressions

- Always use parentheses to enforce explicit precedence

- For shift operations, the amount shifted must be &gt;= 0 and &lt; sizeof( value being shifted )

- Do not use the comma operator
	- Multiple initialization on the same line
 	- For loops must iterate and act upon a single variable

- Always check for overflow on arithmetic operations on unsigned variables

### Side Effects

- Initializer lists must not have side effects
	- No modification of pointers during initialization
	- Make all statements explicit

- Check macros to prevent side-effects in calls
	- e.g. What happens when you call `DO_COPY_INDEX ( i++ )`?
```
#define DO_COPY_INDEX ( x ) ( a [ ( x ) ] = b [ ( x ) ] )
```
 

- Prefer that increment and decrement operators ( `++` and `--` ) are explicit statements instead of integral in an operation, e.g.
	- Macros
 	- Logical comparisons ( `&&` and `||` )
	- `sizeof`

- Do not use the result of an assignment operation as a variable or input into another statement

- Logical comparisons should not contain persistent side effects such as assignments

### Control Statements

- Always use spaces instead of tabs
 - Use 4 space as the standard indent

- Include a space after an opening parenthesis, brace or bracket and a space before the closing match.
	- Parentheses: `( to_b || not_to_b )`
	- Brace: `array[ 4 ] = { 0, 1, 2, 3 };`
	- Bracket: `array[ 4 ] = { 0, 1, 2, 3 };`

- Variable and function naming should use “snake case”, namely descriptive names which are all lowercase and separated by an underscore
	- `int32_t this_is_an_example_of_a_variable;`

- For loops shall have the following form:

```
    int32_t i;
    int32_t sum = 0;
    int32_t max = 25;

    for ( i = 0; i < max; i++ )
    {
        sum += i;
    }
```

- While loops shall have the following form:

```
    int32_t i = 0;
    int32_t sum = 0;
    int32_t max = 25;

    while ( i < max )
    {
        sum += i;
        i++;
    }
```

- Do loops shall have the following form:

```
    int32_t i = 0;
    int32_t sum = 0;
    int32_t max = 25;

    do
    {
      sum += i;
      i++;
    } ( while i < max );

```

- Prefer that loops exit using control logic instead of a `break`  statement

- Loop counter should not use a floating type as the iterator

- The control logic to exit a given loop must be controlled by a variable

 - This prevents statements that are always true or always false
 - Exceptions:
   - `for ( ; ; )`
   - `while ( 1 )` or `while ( true )`

- The control logic for a statement used in if, for, while and do must resolve to a boolean expression

	- Checking for the validity of a variable like an integer or pointer is not allowed

	- i.e. `while ( i )` is not acceptable where `while ( i != 0 )` is

Control Flow
------------

- Prefer to not use `goto`
	- If `goto` is required, the `goto` label must be within the same function

- Prefer to limit the number of `break` statements to one for a given loop

- Functions should have a single point of exit at the end

```
    uint32_t example_function( uint32_t* pointer_parameter )
    {
        uint32_t return_code = 0;

        if ( pointer_parameter != NULL )
        {
            // Do some work here
            return_code = 1;
        }
        else
        {
            // Do something else here
            return_code = 10;
        }
        return ( return_code );
    }
```

- Statements shall always use braces to delineate the scope of their operation

	- As with a function, `if`, `while`, `do`, `for`, always use braces to scope the set of statements or other functions that the conditional controls

-   All `if`, `else if`, trees shall be terminated with an `else` statement

### Switch Statements

- Prefer `if`, `else if`, `else` construct to `switch` statements
	- If the `switch` statement is required, it must be well-formed
	- Each `case` is self-contained and must contain a `break` statement
	- Falling through to the next `case` is only allowed when there is an empty `case`
	- All `switch` statements must contain a final `default` case
	- `Switch` statements must contain at least 2 `cases` and a `default`

```
    uint32_t input_parameter;

    switch ( input_parameter )
    {
        case 1:
        // Do some work here
        break;

        case 2:
        case 3:
        {
            uint32_t local_scope_variable;
            // Do some other work here
            break;
        }

        default:
        break;
    }
```

### Functions

- Variable argument lists are not allowed (e.g. `va_arg`, `va_start`, `va_end`)

	- printf or its equivalent is allowed for debug only

- Recursion is not allowed

- All functions require prototypes

- Prefer that return values from functions are used

### Pointers and Arrays

- Check that pointer arithmetic does not go beyond the bounds of the array to which the pointer refers.

- Avoid pointer subtraction
	- Pointer subtraction is only allowed for pointers that refer to the same array

- Avoid pointer comparison operations
	- Pointer comparison can be used to determine if a pointer is in the bounds of the array that it refers to.

- Pointers should not go beyond two levels of nesting
	- i.e. Pointers to pointers are allowed

- Pointers should not be for stack variables or other objects with automatic storage

- Check that arrays of variable size are of positive size

### Overlapping Storage

- What to do here? MISRA prefers that there are no unions allowed...

### Preprocessor Directives

- `#include` directives should be preceded only by comments or other preprocessor directives

- Macros shall not be named the same as a keyword

- Prefer that `#undef` is not used

- Macros should not contain preprocessor directives (side effects)

- Expressions from macro expansion should be surrounded by parentheses

- `#if` and `#elif` expressions should resolve as 0 or 1

- Prefer that the `#` and `##` directives not be used

### Standard Libraries

- Avoid `#define` and `#undef` on reserved identifiers or macros

- Avoid declaring a reserved identifier or macro

	- e.g. do not create a variable called “float”

- MISRA proscribes the usage of a bunch of library functions. I need to determine what is or should be available for OSCC

### Resources

- Free all allocated memory

- No file access for read/write

	- read xor write

- Do not dereference a FILE object pointer

