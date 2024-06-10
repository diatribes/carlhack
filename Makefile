C = gcc
CFLAGS = -std=c99 -g -pedantic -Wall
LIBS = -lm -lcurses #`pkg-config --cflags --libs sdl2`

CFILES = main.c

carlhack: $(CFILES) clean
	$(CC) $(CFLAGS) -o carlhack -pthread -O3 -DNDEBUG $(CFILES) $(LIBS)

clean:
		rm -vf carlhack
