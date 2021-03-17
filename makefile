CC=gcc

Project1Shell: Project1Shell.o
	gcc -o Project1Shell Project1Shell.o

Project1Shell.o: Project1Shell.c
	gcc -c Project1Shell.c

clean: rm Project1Shell Project1Shell.o
