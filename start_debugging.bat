@echo off
IF "%__ALREADY_CALLED%" EQU "" (
	set __ALREADY_CALLED="yee"
	"C:/Program Files (x86)/Microsoft Visual Studio/2019/Preview/VC/Auxiliary/Build/vcvarsall.bat" x64
)
cl unity_build.c /Fd:game.pdb /Isrc /Iinclude/win /Z7 /TC /DTCC /DMSVC /DDEBUG /DOS_WINDOWS /Fegame.exe /link /DEBUG:FULL /incremental:no lib/win_msvc/openal32.lib lib/win_msvc/cglm.lib lib/win_msvc/glfw3.lib opengl32.lib gdi32.lib user32.lib kernel32.lib msvcrt.lib shell32.lib
"C:/Programs/remedybg/remedybg.exe" session.rdgb
@echo on