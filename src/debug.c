//~ DEBUF
#ifdef DEBUG

#define debug(code) do { code ;} while (0)

#define debug_print(...) printf(__VA_ARGS__)
#define debug_error(...) fprintf(stderr, __VA_ARGS__)

#define debug_profile(name) for (clock_t __profiler_loop = 1, __profiler_begin = clock(); __profiler_loop; __profiler_loop = 0, printf("Profiling of '" name "': %zums\n", clock() - __profiler_begin))

//~
#else // ifdef DEBUG

#define debug(code) do {} while (0)

#define debug_print(...) 0
#define debug_error(...) 0

#define debug_profile(name) if (true)

//~
#endif // else ifdef DEBUG

