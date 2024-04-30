clean:
	-rm *.exe
	-rm *.out

build:
	g++ -O3 -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2 -lsamplerate

test:
	g++ -O3 -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2 -lsamplerate
sdl:
	g++ -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2 -lsamplerate

other:
	g++ -I src/include -L src/lib -o nacho -g nacho/Core/*.cpp nacho/Core/Mapper/*.cpp -lmingw32 -lSDL2main -lSDL2 

compile:
	g++ -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2 -lsamplerate -lsamplerate
	g++ -I src/include -L src/lib -o nacho -g nacho/Core/*.cpp nacho/Core/Mapper/*.cpp -lmingw32 -lSDL2main -lSDL2 


compare:
	./nacho.exe -insert tests/$(test).nes
	./nest.exe tests/$(test).nes
	git diff --no-index  .\nest.out .\nacho.out --output nest.diff

all:
	make compile
	make compare test=$(test)

run:
	g++ -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2
	./nest.exe tests/$(test).nes

play:
	./nest.exe games/$(game).nes

play-tas:
	./nest.exe games/$(game).nes -i tas/$(tas).tas

create-tas:
	./nest.exe games/$(game).nes -o tas/$(tas).tas