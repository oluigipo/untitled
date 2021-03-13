#pragma once

//~ DEBUF
#ifdef DEBUG
#include <stdio.h>

#define debug(code) do { code ;} while (0)
#define debug_noscope(code) code

#define debug_log debug_print
#define debug_print(...) printf(__VA_ARGS__)
#define debug_error(...) fprintf(stderr, __VA_ARGS__)

#define debug_profile(name) for (clock_t __profiler_loop = 1, __profiler_begin = clock(); __profiler_loop; __profiler_loop = 0, printf("Profiling of '" name "': %zums\n", clock() - __profiler_begin))

#define MACRO_STR(s) #s
#define MACRO_STR_2(s) MACRO_STR(s)
#define assert(cond) do { if(!(cond)) { os_assertion_failure( "FILE: " __FILE__ "\nLINE: " MACRO_STR_2(__LINE__) "\n\n" MACRO_STR(cond) ); } } while (0)

//~
#else // ifdef DEBUG

#define debug(code) 0
#define debug_noscope(code) 

#define debug_log debug_print
#define debug_print(...) 0
#define debug_error(...) 0

#define debug_profile(name) if (true)

#define assert(cond) 0

//~
#endif // else ifdef DEBUG

