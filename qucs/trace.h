// Functions for traces and testing.
// essentials only
// from gnucap/io_trace.h, revisit later.

#include <iostream>

#undef unreachable
#define unreachable() ( \
    std::cerr << "@@#\n@@@unreachable:" \
              << __FILE__ << ":" << __LINE__ << ":" << __func__ << "\n"); \

#undef incomplete
#define incomplete() ( \
    std::cerr << "@@#\n@@@incomplete:" \
              << __FILE__ << ":" << __LINE__ << ":" << __func__ << "\n" )

#undef untested
#ifdef TRACE_UNTESTED
#define untested() ( std::cerr <<  "@@#\n@@@:"<< __FILE__ << ":"<< __LINE__ \
          <<":" << __func__ << "\n" )
#else
#define untested()
#endif

#undef itested
#ifdef TRACE_ITESTED
#define itested() ( std::cerr << "@@#\n@@@:" \
     << __FILE__ << ":" << __LINE__ << ":" << __func__ << "\n" )
#else
#define itested()
#endif
