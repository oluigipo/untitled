// Interface
internal inline void os_sleep_ms(u64 ms);
internal inline void os_sleep_ns(u64 ms);
internal inline u64 os_time_ms(void);
internal inline u64 os_time_ns(void);

//~ Windows Stuff
#if defined(OS_WINDOWS)
#include <windows.h>
#include <time.h>

internal inline void os_sleep_ms(u64 ms) {
	Sleep(ms);
}

internal inline void os_sleep_ns(u64 ns) {
	os_sleep_ms(ns / 1000000); // well then
}

internal inline u64 os_time_ms(void) {
	return clock() / (CLOCKS_PER_SEC / 1000);
}

internal inline u64 os_time_ns(void) {
	LARGE_INTEGER result;
	
	if (QueryPerformanceCounter(&result))
		return result.QuadPart;
	else // fallback
		return GetTickCount() * 1000000;
}

//~ Linux Stuff
#elif defined(OS_LINUX)
#include <time.h>
#include <unistd.h>

internal inline void os_sleep_ms(u64 ms) {
	usleep(ms * 1000);
}

internal inline void os_sleep_ns(u64 ns) {
	nanosleep(&(struct timespec) { .tv_nsec = ns }, &(struct timespec) { 0 });
}

internal inline u64 os_time_ms(void) {
	return clock() / (CLOCKS_PER_SEC / 1000);
}

internal inline u64 os_time_ns(void) {
	struct timespec ts;
	
	if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
        return ((u64)ts.tv_sec * 1000000000ull + (u64)ts.tv_nsec);
    else // fallback
        return os_time_ms() * 1000000ull;
}

//~ Error message
#else
#error This should be compiled only on Windows & Linux. If you want to support your platform, please implement the functions found in 'src/os.c'
#endif
