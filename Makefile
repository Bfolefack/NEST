compile:
	gcc -g *.c -o nest

debug:
	gdb nest $(file)

clean:
	-rm *.exe

sdl:
	g++ -I src/include -L src/lib -o nest -g vidya.c -lmingw32 -lSDL2main -lSDL2