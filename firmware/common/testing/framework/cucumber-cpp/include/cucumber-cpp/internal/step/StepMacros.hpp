#ifndef CUKE_STEPMACROS_HPP_
#define CUKE_STEPMACROS_HPP_

#include "../RegistrationMacros.hpp"

// ************************************************************************** //
// **************                     STEP                     ************** //
// ************************************************************************** //

#define CUKE_STEP_(step_matcher)                        \
CUKE_STEP_WITH_NAME_(CUKE_GEN_OBJECT_NAME_, step_matcher) \
/**/

#define CUKE_STEP_WITH_NAME_(step_name, step_matcher) \
CUKE_OBJECT_(                                         \
    step_name,                                        \
    STEP_INHERITANCE(step_name),                      \
    CUKE_STEP_REGISTRATION_(step_name, step_matcher)  \
)                                                     \
/**/

#define CUKE_STEP_REGISTRATION_(step_name, step_matcher) \
::cucumber::internal::registerStep< step_name >(         \
    step_matcher, __FILE__, __LINE__                     \
)                                                        \
/**/

// ************************************************************************** //
// **************               GIVEN/WHEN/THEN                ************** //
// ************************************************************************** //

#define GIVEN CUKE_STEP_
#define WHEN CUKE_STEP_
#define THEN CUKE_STEP_

// ************************************************************************** //
// **************                 REGEX_PARAM                  ************** //
// ************************************************************************** //

#define REGEX_PARAM(type, name) const type name(getInvokeArg<type>())
#define TABLE_PARAM(name) const ::cucumber::internal::Table & name = \
    getArgs()->getTableArg()

#endif /* CUKE_STEPMACROS_HPP_ */
