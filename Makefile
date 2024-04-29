debug:
	gdb nest $(file)

clean:
	-rm *.exe

sdl:
	g++ -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2

other:
	g++ -I src/include -L src/lib -o nacho -g nacho/Core/*.cpp nacho/Core/Mapper/*.cpp -lmingw32 -lSDL2main -lSDL2

compile:
	g++ -I src/include -L src/lib -o nest -g *.cpp -lmingw32 -lSDL2main -lSDL2
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

nacho:
	g++ -I src/include -L src/lib -o nacho -g nacho/Core/*.cpp nacho/Core/Mapper/*.cpp -lmingw32 -lSDL2main -lSDL2
	./nest.exe tests/$(test).nes