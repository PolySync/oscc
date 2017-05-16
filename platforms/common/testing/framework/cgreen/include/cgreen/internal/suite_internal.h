#ifndef SUITE_INTERNAL_HEADER
#define SUITE_INTERNAL_HEADER

#include "cgreen/unit.h"


enum {test_function, test_suite};

typedef struct TestSuite_ TestSuite;

typedef struct {
    int type;
    const char *name;
    union {
        CgreenTest *test;
        TestSuite *suite;
    } Runnable;
} UnitTest;

struct TestSuite_ {
	const char *name;
	const char* filename;
	int line;
	UnitTest *tests;
	void (*setup)(void);
	void (*teardown)(void);
	int size;
};

#ifdef __cplusplus
namespace cgreen {
    extern "C" {
#endif

void do_nothing(void);

TestSuite *create_named_test_suite_(const char *name, const char *filename, int line);
void add_test_(TestSuite *suite, const char *name, CgreenTest *test);
void add_tests_(TestSuite *suite, const char *names, ...);
void add_suite_(TestSuite *owner, const char *name, TestSuite *suite);

#ifdef __cplusplus
    }
}
#endif

#endif

