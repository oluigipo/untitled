@echo off
cls
echo Compiling in Release Mode.
echo =====================================================
gcc unity_build.c -Isrc -o game.exe -O2 -std=c99 -DOS_WINDOWS -lglfw3 -Llib/win -Iinclude/win -lopenal32.dll -lgdi32 -lopengl32 -lcglm -luser32 -lkernel32 -lmsvcrt -lshell32 -lsndfile -static %*

if %ERRORLEVEL% EQU 0 (
	echo Compilation Status: OK
	echo =====================================================
	game.exe
)

@echo on