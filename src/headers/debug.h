#pragma once
#undef assert

//~ DEBUG
#ifdef DEBUG
#include <stdio.h>
#include "os.h"

#define debug if (true)
#define debug_noscope(code) code

#define debug_log debug_print
#define debug_print(...) printf(__VA_ARGS__)
#define debug_error(...) fprintf(stderr, __VA_ARGS__)

#define debug_profile(name) for (clock_t __profiler_loop = 1, __profiler_begin = clock(); __profiler_loop; __profiler_loop = 0, printf("Profiling of '" name "': %zums\n", clock() - __profiler_begin))

#define MACRO_STR(s) #s
#define MACRO_STR_2(s) MACRO_STR(s)
#define assert(cond) do { if(!(cond)) { os_assertion_failure( "FILE: " __FILE__ "\nLINE: " MACRO_STR_2(__LINE__) "\n\n" #cond ); } } while (0)

//~
#else // ifdef DEBUG

#define debug if (false)
#define debug_noscope(code) 

#define debug_log debug_print
#define debug_print(...)
#define debug_error(...)

#define debug_profile(name) if (true)

#define assert(cond)

//~
#endif // else ifdef DEBUG
