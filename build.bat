@echo off
call lib.bat
cls
echo Compiling in Release Mode.
echo =====================================================
gcc src/main.c -o game.exe -O2 %LIB%

if %ERRORLEVEL% EQU 0 (
	echo Compilation Status: OK
	echo =====================================================
	game.exe
)

@echo on