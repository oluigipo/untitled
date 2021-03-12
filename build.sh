LIB="-I./include -L./lib/linux -lglfw3 -lpthread -lm -ldl -lGL -lGLU -lX11 -lcglm"
clear
echo Compiling in Release Mode.
echo =====================================================
gcc src/main.c -o game -O2 -std=gnu99 -DOS_LINUX $LIB

if [ $? = 0 ]; then
	echo Compilation Status: OK
	echo =====================================================
	chmod +x ./game
	./game
fi
