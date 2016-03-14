all: cat

clean:
	rm -f cat.o cat

cat.o: cat.c
	gcc -c -std=c89 cat.c

cat: cat.o
	gcc cat.o -o cat
