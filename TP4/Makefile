all: matrice

matrice : matrice.o
	gcc -o matrice matrice.o

matrice.o : matrice.c
	gcc -c matrice.c -o matrice.o

clean :
	rm *.o matrice
