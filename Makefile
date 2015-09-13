all: main.o builtins.o
	gcc main.o builtins.o -o quash -lreadline
main.o: main.c builtins.c
	gcc -c -g main.c builtins.c -lreadline
builtins.o: builtins.c
	gcc -c -g builtins.c
clean:
	rm -f *.o *.swp quash
