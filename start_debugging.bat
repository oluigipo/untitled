@echo off
cls
IF "%__ALREADY_CALLED%" EQU "" (
	set __ALREADY_CALLED="yee"
	"C:/Program Files (x86)/Microsoft Visual Studio/2019/Preview/VC/Auxiliary/Build/vcvarsall.bat" x64
)
cl unity_build_1.c unity_build_2.c /Fd:game.pdb /Isrc /Iinclude/win /Z7 /TC /DTCC /DMSVC /DDEBUG /DOS_WINDOWS /Fegame.exe /link /NODEFAULTLIB:library /DEBUG:FULL /incremental:no lib/win_msvc/openal32.lib ole32.lib oleaut32.lib lib/win_msvc/cglm.lib lib/win_msvc/glfw3.lib lib/win_msvc/discord_game_sdk.dll.lib opengl32.lib gdi32.lib user32.lib kernel32.lib msvcrt.lib shell32.lib
rename OpenAL32.dll OpenAL32_gcc.dll
rename OpenAL32_msvc.dll OpenAL32.dll
"C:/Programs/remedybg/remedybg.exe" session.rdgb
rename OpenAL32.dll OpenAL32_msvc.dll
rename OpenAL32_gcc.dll OpenAL32.dll
@echo on