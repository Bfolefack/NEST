clean:
	-rm *.exe
	-rm *.out

build:
	g++ -O3 -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2

test:
	g++ -O3 -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2
	./nest.exe tests/$(test).nes

play:
	g++ -O3 -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2
	./nest.exe games/$(game).nes

play-tas:
	g++ -O3 -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2
	./nest.exe games/$(game).nes -i tas/$(tas).tas

create-tas:
	g++ -O3 -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2
	./nest.exe games/$(game).nes -o tas/$(tas).tas