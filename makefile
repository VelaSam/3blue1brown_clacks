compile:
	cc -o out bouncy.c `sdl2-config --cflags --libs`

run: 
	./out
