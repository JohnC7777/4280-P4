$(CC) = gcc
final:
	$(CC) main.c statSem.c scanner.c parser.c -o main

clean:
	rm -f *.o main
