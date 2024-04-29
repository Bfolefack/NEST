clean:
	-rm *.exe
	-rm *.out

build:
	g++ -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2

play:
	g++ -O3 -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2
	./nest.exe games/$(game).nes

test:
	g++ -O3 -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2
	./nest.exe tests/$(test).nes