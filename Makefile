CC = gcc
CFLAGS = -Wall -m64 -O3 -flto -march=native -lm `sdl2-config --cflags --libs` -lSDL2_ttf
CFLAGS2 = -g -v -da -Q -O0 -lm `sdl2-config --cflags --libs` -lSDL2_ttf
NAME = nbody
SRC = source/

programs: nbody
	rm -f *.o

nbody: main.o ${SRC}main.h bh.o ${SRC}bh.h examples.o hud.o num.o output.o sdl.o setup.o
	${CC} -fopenmp -o ${NAME} main.o bh.o examples.o hud.o num.o output.o sdl.o setup.o ${CFLAGS}

bh.o: ${SRC}bh.c
	${CC} -c ${SRC}bh.c ${CFLAGS}

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

clean:
	rm *.o $(NAME)

