compile:
	g++ -g *.cpp -o nest

debug:
	gdb nest $(file)

clean:
	-rm *.exe

sdl:
	g++ -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2