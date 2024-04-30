clean:
	-rm *.exe
	-rm *.out

build:
	g++ -O3 -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2 -lsamplerate

play:
	./nest.exe games/$(game).nes

play-tas:
	./nest.exe games/$(game).nes -i tas/$(tas).tas

create-tas:
	./nest.exe games/$(game).nes -o tas/$(tas).tas