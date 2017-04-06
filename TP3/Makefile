all: prod cons emetteur recepteur

prod : prod.o
	gcc -o prod prod.o

cons : cons.o
	gcc -o cons cons.o

emetteur : emetteur.o
	gcc -o emetteur emetteur.o

recepteur : recepteur.o
	gcc -o recepteur recepteur.o

prod.o : prod.c
	gcc -c prod.c -o prod.o

cons.o : cons.c
	gcc -c cons.c -o cons.o

emetteur.o : emetteur.c
	gcc -c emetteur.c -o emetteur.o

recepteur.o : recepteur.c
	gcc -c recepteur.c -o recepteur.o

clean :
	rm *.o prod cons emetteur recepteur
