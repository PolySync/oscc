#ifndef UNIT_IMPLEMENTATION_HEADER
#define UNIT_IMPLEMENTATION_HEADER

#ifdef __cplusplus
#include <cstddef>
#endif

#include "stringify_token.h"


typedef struct {
    const char* name;
    const char* filename;
    void (*setup)(void);
    void (*teardown)(void);
} CgreenContext;

typedef struct {
    int skip;                 /* Should test be skipped? */
    CgreenContext* context;
    const char* name;
    void(*run)(void);
    const char* filename;
    int line;
} CgreenTest;

#define CGREEN_SPEC_PREFIX "CgreenSpec"
#define CGREEN_SEPARATOR "__"
#define spec_name(contextName, testName) CgreenSpec__##contextName##__##testName##__

//This gives better error messages at the cost of duplication
#define ENSURE_VA_NUM_ARGS(...) ENSURE_VA_NUM_ARGS_IMPL_((__VA_ARGS__, _CALLED_WITH_TOO_MANY_ARGUMENTS,  WithContextAndSpecificationName,  WithSpecificationName))
#define ENSURE_VA_NUM_ARGS_IMPL_(tuple) ENSURE_VA_NUM_ARGS_IMPL tuple

#define ENSURE_VA_NUM_ARGS_IMPL(_1, _2, _3, _4, N, ...) N

#define ENSURE_macro_dispatcher(func, ...)   ENSURE_macro_dispatcher_(func, ENSURE_VA_NUM_ARGS(__VA_ARGS__))

// these levels of indirecton are a work-around for variadic macro deficiencies in Visual C++ 2012 and prior
#define ENSURE_macro_dispatcher_(func, nargs)           ENSURE_macro_dispatcher__(func, nargs)
#define ENSURE_macro_dispatcher__(func, nargs)           ENSURE_macro_dispatcher___(func, nargs)
#define ENSURE_macro_dispatcher___(func, nargs)          func ## nargs

#define Ensure_NARG(...) ENSURE_macro_dispatcher(Ensure, __VA_ARGS__)

#define EnsureWithContextAndSpecificationName(skip, contextName, specName, ...) \
    static void contextName##__##specName (void);\
    CgreenTest spec_name(contextName, specName) = { skip, &contextFor##contextName, STRINGIFY_TOKEN(specName), &contextName##__##specName, __FILE__, __LINE__ }; \
    static void contextName##__##specName (void)

extern CgreenContext defaultContext;

#define EnsureWithSpecificationName(skip, specName, ...)   \
    static void specName (void);\
    CgreenTest spec_name(default, specName) = { skip, &defaultContext, STRINGIFY_TOKEN(specName), &specName, __FILE__, __LINE__ }; \
    static void specName (void)

#define DescribeImplementation(subject) \
        static void setup(void);                \
        static void teardown(void);                                     \
        static CgreenContext contextFor##subject = { STRINGIFY_TOKEN(subject), __FILE__, &setup, &teardown }; \
        extern void(*BeforeEach_For_##subject)(void);                   \
        extern void(*AfterEach_For_##subject)(void);                    \
        static void setup(void) {                                       \
            if (BeforeEach_For_##subject != NULL) BeforeEach_For_##subject(); \
        }                                                               \
        static void teardown(void) {                                    \
            if (AfterEach_For_##subject != NULL) AfterEach_For_##subject(); \
        }

#define BeforeEachImplementation(subject) \
        void BeforeEach_For_##subject##_Function(void);                 \
        void(*BeforeEach_For_##subject)(void) = &BeforeEach_For_##subject##_Function; \
        void BeforeEach_For_##subject##_Function(void)

#define AfterEachImplementation(subject) \
        static void AfterEach_For_##subject##_Function(void);           \
        void(*AfterEach_For_##subject)(void) = &AfterEach_For_##subject##_Function; \
        static void AfterEach_For_##subject##_Function(void)

#endif
