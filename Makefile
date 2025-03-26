all: main.c
	gcc -Isrc/include main.c -Lsrc/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -mwindows -o snake.exe

clean:
	rm -f snake.exe