CC = gcc
CFLAGS = -Wall -march=native -O3 -lm `sdl2-config --cflags --libs` -lSDL2_ttf
NAME = nbody

programs: nbody
	rm -f *.o

nbody: main.o main.h bh.o bh.h examples.o hud.o num.o output.o sdl.o setup.o
	${CC} -fopenmp -o ${NAME} main.o bh.o examples.o hud.o num.o output.o sdl.o setup.o ${CFLAGS}

bh.o: bh.c
	${CC} -c bh.c ${CFLAGS}

examples.o: examples.c
	${CC} -c examples.c ${CFLAGS}

main.o: main.c
	${CC} -c main.c ${CFLAGS}

hud.o: hud.c
	${CC} -c hud.c ${CFLAGS}

num.o: num.c
	${CC} -fopenmp -c num.c ${CFLAGS}

output.o: output.c
	${CC} -c output.c ${CFLAGS}

sdl.o: sdl.c
	${CC} -c sdl.c ${CFLAGS}

setup.o: setup.c
	${CC} -c setup.c ${CFLAGS}

clean:
	rm *.o $(NAME)

