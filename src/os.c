#ifdef _WIN32
#include <windows.h>

void os_sleep(uint ms) {
	Sleep(ms);
}

#elif defined(__linux__)
#include <unistd.h>

void os_sleep(uint ms) {
	usleep(ms * 1000);
}

#endif