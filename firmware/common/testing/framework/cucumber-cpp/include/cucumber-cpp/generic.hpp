#include "internal/defs.hpp"
#ifdef STEP_INHERITANCE
    #error Test framework found: please include autodetect.hpp or remove the test framework includes
#else
    #include "internal/drivers/GenericDriver.hpp"
#endif
