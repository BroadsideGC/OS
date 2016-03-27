all: sigusr

clean:
	rm -f sigusr.o sigusr

sigusr.o: sigusr.c
	gcc -c sigusr.c

sigusr: sigusr.o
	gcc sigusr.o -o sigusr
