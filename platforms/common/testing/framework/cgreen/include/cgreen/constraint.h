#ifndef CONSTRAINT_HEADER
#define CONSTRAINT_HEADER

#include <cgreen/reporter.h>
#include <cgreen/cgreen_value.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

typedef enum {
    VALUE_COMPARER,
    CONTENT_COMPARER,
    STRING_COMPARER,
    DOUBLE_COMPARER,
    RETURN_VALUE,
    CONTENT_SETTER,
    CALL
} ConstraintType;

typedef struct Constraint_ Constraint;
struct Constraint_ {
    ConstraintType type;
    const char *name;
    void (*destroy)(Constraint *);
    bool (*compare)(Constraint *, CgreenValue);
    void(*execute)(Constraint *, const char *, CgreenValue, const char *, int, TestReporter *);
    char *(*failure_message)(Constraint *, const char *, intptr_t);
    const char *actual_value_message;
    const char *expected_value_message;
    CgreenValue expected_value;
    const char *expected_value_name;

    /* for PARAMETER constraints */
    const char *parameter_name;
    size_t size_of_expected_value;
};

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

Constraint *create_constraint(void);
Constraint *create_parameter_constraint_for(const char *parameter_name);

void destroy_empty_constraint(Constraint *constraint);
void destroy_static_constraint(Constraint *constraint);
void destroy_double_constraint(Constraint *constraint);
void destroy_constraint(Constraint *);
void destroy_constraints(va_list constraints);
 
bool compare_want_value(Constraint *constraint, CgreenValue actual);
bool compare_do_not_want_value(Constraint *constraint, CgreenValue actual);
void test_want(Constraint *constraint, const char *function, CgreenValue actual, const char *test_file, int test_line, TestReporter *reporter);

void test_constraint(Constraint *constraint, const char *function, intptr_t actual, const char *test_file, int test_line, TestReporter *reporter);

Constraint *create_equal_to_value_constraint(intptr_t expected_value, const char *expected_value_name);
Constraint *create_equal_to_hexvalue_constraint(intptr_t expected_value, const char *expected_value_name);
Constraint *create_not_equal_to_value_constraint(intptr_t expected_value, const char *expected_value_name);
Constraint *create_greater_than_value_constraint(intptr_t expected_value, const char *expected_value_name);
Constraint *create_less_than_value_constraint(intptr_t expected_value, const char *expected_value_name);
Constraint *create_equal_to_contents_constraint(void *pointer_to_compare, size_t size_to_compare, const char *compared_pointer_name);
Constraint *create_not_equal_to_contents_constraint(void *pointer_to_compare, size_t size_to_compare, const char *compared_pointer_name);
Constraint *create_equal_to_string_constraint(const char* expected_value, const char *expected_value_name);
Constraint *create_not_equal_to_string_constraint(const char* expected_value, const char *expected_value_name);
Constraint *create_contains_string_constraint(const char* expected_value, const char *expected_value_name);
Constraint *create_does_not_contain_string_constraint(const char* expected_value, const char *expected_value_name);
Constraint *create_begins_with_string_constraint(const char* expected_value, const char *expected_value_name);
Constraint *create_does_not_begin_with_string_constraint(const char* expected_value, const char *expected_value_name);
Constraint *create_ends_with_string_constraint(const char* expected_value, const char *expected_value_name);
Constraint *create_does_not_end_with_string_constraint(const char* expected_value, const char *expected_value_name);

Constraint *create_equal_to_double_constraint(double expected_value, const char *expected_value_name);
Constraint *create_not_equal_to_double_constraint(double expected_value, const char *expected_value_name);
Constraint *create_less_than_double_constraint(double expected_value, const char *expected_value_name);
Constraint *create_greater_than_double_constraint(double expected_value, const char *expected_value_name);
Constraint *create_return_value_constraint(intptr_t value_to_return);
Constraint *create_return_double_value_constraint(double value_to_return);
Constraint *create_set_parameter_value_constraint(const char *parameter_name, intptr_t value_to_set, size_t size_to_set);

bool no_expected_value_in(const Constraint *constraint);
bool values_are_strings_in(const Constraint *constraint);
bool is_content_comparing(const Constraint *constraint);
bool is_content_setting(const Constraint *constraint);
bool is_not_content_setting(const Constraint *constraint);
bool is_string_comparing(const Constraint *constraint);
bool is_double_comparing(const Constraint *constraint);
bool is_comparing(const Constraint *constraint);
bool is_not_comparing(const Constraint *constraint);
bool is_parameter(const Constraint *);
bool constraint_is_not_for_parameter(const Constraint *, const char *);
bool constraint_is_for_parameter(const Constraint *, const char *);
bool constraint_is_for_parameter_in(const Constraint *, const char *);
bool doubles_are_equal(double tried, double expected);
bool double_is_lesser(double actual, double expected);
bool double_is_greater(double actual, double expected);

#ifdef __cplusplus
    }
}
#endif

#endif
