#pragma once
#include "Log.h"
#include <intrin.h>

#ifdef SNAP_ENABLE_ASSERTION
#define SNAP_ASSERT(cond) if(cond) {} else {__debugbreak();}
#define SNAP_ASSERT_MSG(cond,msg) if(cond) {} else {SNAP_CRITICAL("Assertion Failed: File {0} on Line {1} : {2}",__FILE__,__LINE__,msg); __debugbreak();}
#else
#define SNAP_ASSERT(cond)
#define SNAP_ASSERT_MSG(cond,msg)
#endif