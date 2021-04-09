@echo off
cls
echo Compiling in Release Mode.
echo =====================================================
gcc unity_build_1.c unity_build_2.c -mwindows -msse3 -o game.exe -O2 -std=c99 -Isrc -Iinclude/win -Llib/win -DOS_WINDOWS -DFIRST_SCENE=scene_main -Wall -Wformat=0 -Wno-missing-braces discord_game_sdk.dll -lglfw3 -lopenal32.dll -lgdi32 -lcglm -luser32 -lkernel32 -lmsvcrt -lshell32 -static %*

if %ERRORLEVEL% EQU 0 (
	echo Compilation Status: OK
	echo =====================================================
	game.exe
)

@echo on