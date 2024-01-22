INCLUDES = -I ./include
FLAGS = -g #debugging
OBJECTS = ./build/chip8ram.o  ./build/chip8stack.o ./build/chip8keyboard.o ./build/chip8.o ./build/chip8screen.o
all: ${OBJECTS}
	gcc ${FLAGS} ${INCLUDES} ./src/main.c ${OBJECTS} -L ./lib -lmingw32 -lSDL2main -lSDL2 -o ./bin/main

./build/chip8ram.o:src/chip8ram.c
	gcc ${FLAGS} ${INCLUDES}  ./src/chip8ram.c -c -o ./build/chip8ram.o
./build/chip8stack.o:src/chip8stack.c
	gcc ${FLAGS} ${INCLUDES}  ./src/chip8stack.c -c -o ./build/chip8stack.o
./build/chip8keyboard.o:src/chip8keyboard.c
	gcc ${FLAGS} ${INCLUDES}  ./src/chip8keyboard.c -c -o ./build/chip8keyboard.o
./build/chip8.o:src/chip8.c
	gcc ${FLAGS} ${INCLUDES}  ./src/chip8.c -c -o ./build/chip8.o
./build/chip8screen.o:src/chip8screen.c
	gcc ${FLAGS} ${INCLUDES}  ./src/chip8screen.c -c -o ./build/chip8screen.o

clean:
	del build\*