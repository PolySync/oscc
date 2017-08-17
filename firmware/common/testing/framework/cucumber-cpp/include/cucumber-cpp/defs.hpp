#ifdef __GNUC__
    #warning "Use of defs.hpp is deprecated, please use either autodetect.hpp or generic.hpp"
#else
    #pragma message( "Use of defs.hpp is deprecated, please use either autodetect.hpp or generic.hpp" )
#endif
#include "internal/defs.hpp"
#ifndef STEP_INHERITANCE
    #include "internal/drivers/GenericDriver.hpp"
#endif
