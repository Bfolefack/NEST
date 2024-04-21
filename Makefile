compile:
	gcc -g *.c -o nest

debug:
	gdb nest $(file)

clean:
	-rm *.exe