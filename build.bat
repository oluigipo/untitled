@echo off

set LIB=discord_game_sdk.dll dynlibs/win_x64/openal32.dll -Llib/win -lglfw3 icon.res

if exist "OpenAL32_gcc.dll" (
	rename OpenAL32.dll OpenAL32_msvc.dll
	rename OpenAL32_gcc.dll OpenAL32.dll
)

if "%1" EQU "-m32" (
	set LIB=dynlibs/win_x86/discord_game_sdk.dll dynlibs/win_x86/openal32.dll -Llib/win_x86 -DX86 -lglfw3
)

cls
echo Compiling in Release Mode.
echo =====================================================
gcc unity_build_1.c unity_build_2.c -mwindows -msse3 -o game.exe -O2 -std=c99 -Isrc -Iinclude/win -DOS_WINDOWS -DFIRST_SCENE=scene_main -Wno-discarded-qualifiers -Wno-discarded-array-qualifiers %LIB% -lgdi32 -lopengl32 -luser32 -lshell32 -static %*

if %ERRORLEVEL% EQU 0 (
	echo Compilation Status: OK
	echo =====================================================
	game.exe
)

@echo on