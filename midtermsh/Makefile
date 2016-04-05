

all: midtermsh

clean:
	rm -f midtermsh.o midtermsh

sigusr.o: midtermsh.c
	gcc -c midtermsh.c

sigusr: midtermsh.o
	gcc midtermsh.o -o midtermsh
