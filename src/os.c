// Interface
void os_message_box(const char* restrict title, const char* restrict str);
void os_assertion_failure(const char* restrict what);

//~ Windows Stuff
#if defined(OS_WINDOWS)
#include <windows.h>
#include <stdio.h>
#define EXECUTABLE_NAME "game.exe"

void os_message_box(const char* restrict title, const char* restrict str) {
	MessageBoxA(NULL, str, title, MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
}

void os_assertion_failure(const char* restrict what) {
	char cmdline[1024];
	PROCESS_INFORMATION processInformation;
    STARTUPINFO startupInfo;
	
	ZeroMemory(&processInformation, sizeof processInformation);
	ZeroMemory(&startupInfo, sizeof startupInfo);
	
	startupInfo.cb = sizeof(startupInfo);
	
	snprintf(cmdline, sizeof cmdline, EXECUTABLE_NAME " -error \"%s\"", what);
	b32 result = CreateProcessA(NULL, cmdline, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &startupInfo, &processInformation);
	
	if (!result) {
		debug_error("Assertion Failure:\n\t%s\n", what);
		getchar();
	} else {
		WaitForSingleObject(processInformation.hProcess, INFINITE);
	}
	
	exit(1);
}

//~ Linux Stuff
#elif defined(OS_LINUX)
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define EXECUTABLE_NAME "./game"

void os_message_box(const char* restrict title, const char* restrict str) {
	char cmdline[1024];
	
	snprintf(cmdline, sizeof cmdline, "zenity --error --text=\"%s\" --title=\"%s\"", str, title);
	system(cmdline);
}

void os_assertion_failure(const char* restrict what) {
	os_message_box("Assertion Failure", what);
	exit(1);
	
	return;
	char cmdline[1024];
	snprintf(cmdline, sizeof cmdline, EXECUTABLE_NAME " -error \"%s\"", what);
	if (system(cmdline) != 0) {
		debug_error("Assertion Failure:\n\t%s\n", what);
	}
	
	exit(1);
}

#endif
