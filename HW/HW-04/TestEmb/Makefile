CC=gcc
ADDSUB:Test.o func1.o func2.o
	$(CC) -o ADDSUB Test.o func1.o func2.o
Test.o:Test.c funcs.h
	$(CC) -c Test.c
func1.o:func1.c func1.h
	$(CC) -c func1.c
func2.o:func2.c func2.h
	$(CC) -c func2.c 
clean:
	rm ADDSUB *.o
