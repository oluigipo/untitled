LIB="-I./include -L./lib -lglfw3 -lcglm"
clear
echo Compiling in Release Mode.
echo =====================================================
gcc src/main.c -o game -O2 -std=c99 -DOS_LINUX $LIB

if [ $? = 1 ]; then
	echo Compilation Status: OK
	echo =====================================================
	chmod +x ./game
	./game
fi
