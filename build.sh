LIB="-I./include/linux -I./src/ -L./lib/linux -lopenal -lglfw3 -lpthread -lm -ldl -lGL -lGLU -lX11 -lcglm"
clear
echo Compiling in Release Mode.
echo =====================================================
gcc unity_build_1.c unity_build_2.c -o game -O2 -std=gnu99 -DOS_LINUX $LIB

if [ $? = 0 ]; then
	echo Compilation Status: OK
	echo =====================================================
	chmod +x ./game
	./game
fi
