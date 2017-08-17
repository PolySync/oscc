#ifndef CONSTRAINT_SYNTAX_HELPERS_HEADER
#define CONSTRAINT_SYNTAX_HELPERS_HEADER


#include <cgreen/constraint.h>
#ifdef __cplusplus
#include <cgreen/cpp_constraint.h>
#endif
#include <stdint.h>

/* we normally want to favor delegating functions (for namespacing,
 * and to avoid confusing symbol/preprocessor conflicts), but for the
 * intptr_t catch-all type, we need an explicit cast lest we get
 * warnings-as-errors in newer compilers.  also, we need the textual
 * representation of the expected value and this is the only
 * reasonable way to do it.
 */
#define is_equal_to(value) create_equal_to_value_constraint((intptr_t)value, #value)
#define is_equal_to_hex(value) create_equal_to_hexvalue_constraint((intptr_t)value, #value)
#define is_not_equal_to(value) create_not_equal_to_value_constraint((intptr_t)value, #value)

#define is_greater_than(value) create_greater_than_value_constraint((intptr_t)value, #value)
#define is_less_than(value) create_less_than_value_constraint((intptr_t)value, #value)

#define is_equal_to_contents_of(pointer, size_of_contents) create_equal_to_contents_constraint((void *)pointer, size_of_contents, #pointer)
#define is_not_equal_to_contents_of(pointer, size_of_contents) create_not_equal_to_contents_constraint((void *)pointer, size_of_contents, #pointer)

#define is_equal_to_string(value) create_equal_to_string_constraint(value, #value)
#define is_not_equal_to_string(value) create_not_equal_to_string_constraint(value, #value)
#define contains_string(value) create_contains_string_constraint(value, #value)
#define does_not_contain_string(value) create_does_not_contain_string_constraint(value, #value)
#define begins_with_string(value) create_begins_with_string_constraint(value, #value)
#define does_not_begin_with_string(value) create_does_not_begin_with_string_constraint(value, #value)
#define ends_with_string(value) create_ends_with_string_constraint(value, #value)
#define does_not_end_with_string(value) create_does_not_end_with_string_constraint(value, #value)

#define is_equal_to_double(value) create_equal_to_double_constraint(value, #value)
#define is_not_equal_to_double(value) create_not_equal_to_double_constraint(value, #value)

#define is_less_than_double(value) create_less_than_double_constraint(value, #value)
#define is_greater_than_double(value) create_greater_than_double_constraint(value, #value)


#define will_return(value) create_return_value_constraint((intptr_t)value)
#define will_return_double(value) create_return_double_value_constraint(value)
#define will_set_contents_of_parameter(parameter_name, value, size) create_set_parameter_value_constraint(#parameter_name, (intptr_t)value, (size_t)size)


#ifdef __cplusplus
extern "C" {
#endif

/* these constraints don't take arguments, and we don't want to force
 * users to put "()" on the end of every usage.  we also want to avoid
 * macros when practical, for the namespacing and confusing symbol
 * collision issues, so we use singleton instances.
 */
extern Constraint static_non_null_constraint;
extern Constraint *is_non_null;
#define is_not_null (is_non_null)

extern Constraint static_null_constraint;
extern Constraint *is_null;

extern Constraint static_false_constraint;
extern Constraint *is_false;

extern Constraint static_true_constraint;
extern Constraint *is_true;

#ifdef __cplusplus
}
#endif

#endif
