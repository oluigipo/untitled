#include "headers/os.h"
#include "headers/debug.h"

//~ Windows Stuff
#if defined(OS_WINDOWS)
#include <windows.h>
#include <stdio.h>
#define EXECUTABLE_NAME "game.exe"

void os_message_box(const char* restrict title, const char* restrict str) {
	MessageBoxA(NULL, str, title, MB_ICONERROR | MB_OK | MB_DEFBUTTON1);
}

void os_assertion_failure(const char* restrict what) {
	os_message_box("Assertion Failure", what);
	
	// NOTE(luigi): This code will make this game a bomber. Opening itself over and over again until the end of time.
#if 0
	char cmdline[1024];
	PROCESS_INFORMATION processInformation;
    STARTUPINFO startupInfo;
	
	ZeroMemory(&processInformation, sizeof processInformation);
	ZeroMemory(&startupInfo, sizeof startupInfo);
	
	startupInfo.cb = sizeof(startupInfo);
	
	snprintf(cmdline, sizeof cmdline, EXECUTABLE_NAME " -error \"%s\"", what);
	
	b32 result = CreateProcessA(NULL, cmdline, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP, NULL, NULL, &startupInfo, &processInformation);
	
	if (!result) {
		debug_error("Assertion Failure:\n%s\n", what);
		getchar();
	} else {
		WaitForSingleObject(processInformation.hProcess, INFINITE);
	}
#endif
	
	exit(1);
}

void os_switch_software_rendering(void) {
	if (game.renderer == 0) {
		system("ren ./_opengl32.dll opengl32.dll");
	} else {
		system("ren ./opengl32.dll _opengl32.dll");
	}
	
	game.renderer ^= 1;
}

//~ Linux Stuff
#elif defined(OS_LINUX)
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define EXECUTABLE_NAME "./game"

void os_message_box(const char* restrict title, const char* restrict str) {
	debug_print("============================\nMessage: %s\n%s\n============================\n", title, str);
}

void os_assertion_failure(const char* restrict what) {
	os_message_box("Assertion Failure", what);
	
#if 0
	pid_t pid;
	int status;
	
	pid = fork();
	if (pid < 0) {
		debug_error("Assertion Failure:\n%s\n", what);
		getchar();
	} else if (pid == 0) {
		execl(EXECUTABLE_NAME, "-error", what, NULL);
	}
	
	waitpid(pid, &status, 0);
#endif
	
	exit(1);
}

void os_switch_software_rendering(void) {
	// TODO(luigi): support for linux
	game.renderer = 0;
}

#endif
