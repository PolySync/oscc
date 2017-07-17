#ifndef CGREEN_CPP_CONSTRAINT
#define CGREEN_CPP_CONSTRAINT


#include <cgreen/constraint.h>
#include <string>

namespace cgreen {

template<typename T>
class CppConstraint : public Constraint {
 public:
    T expected_real_value;
    bool (*compare)(CppConstraint *, T);
};

Constraint *create_equal_to_string_constraint(const std::string& expected_value, const char *expected_value_name);
Constraint *create_equal_to_string_constraint(const std::string* expected_value, const char *expected_value_name);
Constraint *create_not_equal_to_string_constraint(const std::string& expected_value, const char *expected_value_name);
Constraint *create_not_equal_to_string_constraint(const std::string* expected_value, const char *expected_value_name);
Constraint *create_contains_string_constraint(const std::string& expected_value, const char *expected_value_name);
Constraint *create_contains_string_constraint(const std::string* expected_value, const char *expected_value_name);
Constraint *create_does_not_contain_string_constraint(const std::string& expected_value, const char *expected_value_name);
Constraint *create_does_not_contain_string_constraint(const std::string* expected_value, const char *expected_value_name);
Constraint *create_begins_with_string_constraint(const std::string& expected_value, const char *expected_value_name);
Constraint *create_begins_with_string_constraint(const std::string* expected_value, const char *expected_value_name);


template<typename T>
bool compare_want_value(CppConstraint<T> *constraint, T actual) {
    return constraint->expected_real_value == actual;
}

template<typename T>
bool compare_do_not_want_value(CppConstraint<T> *constraint, T actual) {
    return !compare_want_value(constraint, actual);
}

template<typename T>
void test_want_value(CppConstraint<T> *constraint, const char *function, T actual, const char *test_file, int test_line, TestReporter *reporter) {
    (void)constraint;
    (void)function;
    (void)actual;
    (void)test_file;
    (void)test_line;
    (void)reporter;
}

#include <stdlib.h>
// TODO: add create_equal_to_constraint_<T> where operator<< output is used for expected_value name
template<typename T>
CppConstraint<T> *create_equal_to_value_constraint(CgreenValue cgreen_value, T expected_value, const char *expected_value_name) {
    CppConstraint<T> *constraint;// = create_cpp_constraint<T>();
    constraint = new CppConstraint<T>();
    constraint->type = VALUE_COMPARER;

    (void)cgreen_value;         /* Avoid warnings for UNUSED, which it is for now */
    
    constraint->Constraint::compare = &compare_want_value;
    constraint->execute = &test_want;
    constraint->name = "equal";
    constraint->expected_value = expected_value;
    constraint->expected_value_name = expected_value_name;
    constraint->size_of_expected_value = sizeof(intptr_t);
    constraint->expected_real_value = expected_value;
    return constraint;
}


}

#endif
