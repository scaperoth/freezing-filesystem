CC = gcc
FILES = main.c
OUT_EXE = run
FLAGS = -lpthread -Wall
DEPS = main.h

build: $(FILES) $(DEPS)
	$(CC) -o $(OUT_EXE) $(FILES) $(FLAGS)

clean:
	rm -f *.o core
	rm run

rebuild: 
	clean build