// include this header when using the __func__ macro
// this is where platforms that don't have C99 __func__ will be handled
#ifndef __func__
#  ifdef _MSC_VER
#    define __func__ __FUNCTION__
#  endif
#endif
