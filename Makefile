CC = gcc
CFLAGS = -Wall -O2 -lm `sdl2-config --cflags --libs` -lSDL2_ttf
NAME = nbody

programs: nbody
	rm -f *.o

nbody: main.o main.h hud.o num.o output.o sdl.o setup.o
	${CC} -o ${NAME} main.o hud.o num.o output.o sdl.o setup.o ${CFLAGS}

main.o: main.c
	${CC} -c main.c ${CFLAGS}

hud.o: hud.c
	${CC} -c hud.c ${CFLAGS}

num.o: num.c
	${CC} -c num.c ${CFLAGS}

output.o: output.c
	${CC} -c output.c ${CFLAGS}

sdl.o: sdl.c
	${CC} -c sdl.c ${CFLAGS}

setup.o: setup.c
	${CC} -c setup.c ${CFLAGS}

clean:
	rm *.o $(NAME)

