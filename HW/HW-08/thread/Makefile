CC=gcc
Thread:main.o thread1.o thread2.o
	$(CC) -lpthread -o Thread main.o thread1.o thread2.o
thread1.o:main.o threads_def.h
	$(CC) -c main.c
thread1.o:thread1.c threads_def.h
	$(CC) -c thread1.c
thread2.o:thread2.c threads_def.h
	$(CC) -c thread2.c 
clean:
	rm Thread *.o
