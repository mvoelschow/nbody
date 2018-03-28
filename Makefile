CC = gcc
CFLAGS = -m64 -O3 -march=native -lm `sdl2-config --cflags --libs` -lSDL2_ttf -lOpenCL
CFLAGS2 = -g -Wall -O0 -lm `sdl2-config --cflags --libs` -lSDL2_ttf -lOpenCL
NAME = nbody
SRC = source/

programs: nbody
	rm -f *.o

nbody: main.o ${SRC}main.h examples.o hud.o num.o output.o sdl.o setup.o vsop87.o
	${CC} -lOpenCL -fopenmp -o ${NAME} main.o examples.o hud.o num.o output.o sdl.o setup.o vsop87.o ${CFLAGS}

examples.o: ${SRC}examples.c
	${CC} -c ${SRC}examples.c ${CFLAGS}

main.o: ${SRC}main.c
	${CC} -c ${SRC}main.c ${CFLAGS}

hud.o: ${SRC}hud.c
	${CC} -c ${SRC}hud.c ${CFLAGS}

num.o: ${SRC}num.c
	${CC} -fopenmp -c ${SRC}num.c ${CFLAGS}

output.o: ${SRC}output.c
	${CC} -c ${SRC}output.c ${CFLAGS}

sdl.o: ${SRC}sdl.c
	${CC} -c ${SRC}sdl.c ${CFLAGS}

setup.o: ${SRC}setup.c
	${CC} -c ${SRC}setup.c ${CFLAGS}

vsop87.o: ${SRC}vsop87.c
	${CC} -c ${SRC}vsop87.c ${CFLAGS}

clean:
	rm *.o $(NAME)

