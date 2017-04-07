#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#define Min 0
#define Max 100

int countPages = 0;
char *addr;

int randInterval(int min, int max) {
    return rand()%(max-min) +min;
}

void initMatrice(int lignes, int colonnes) {
  char c = 0;
  int fd;
  if((fd = open("file.txt", O_RDWR | O_CREAT, 0666)) < 0) {
    perror("Erreur d'ouverture du fichier en écriture - initMatrice");
  }

  for(int i=0; i< lignes; i++) {
    for(int j=0; j<colonnes; j++) {
      c = (char) randInterval(Min, Max);
      if (write(fd, &c, sizeof(c)) < 0) {
        perror("Erreur en écriture dans le fichier");
      }
    }
  }
  close(fd);
}

int racine(double X) {
  double A,B,M,XN;
  if(X==0.0) {
     return 0;
  } else {
     M=1.0;
     XN=X;
     while(XN>=2.0) {
        XN=0.25*XN;
        M=2.0*M;
     }
     while(XN<0.5) {
        XN=4.0*XN;
        M=0.5*M;
     }
     A=XN;
     B=1.0-XN;
     do {
        A=A*(1.0+0.5*B);
        B=0.25*(3.0+B)*B*B;
     } while(B>=1.0E-15);
     return (int)(A*M);
  }
}

void protect(int sig, siginfo_t *si, void *unused){
  if(mprotect(si->si_addr, 6*getpagesize(), PROT_READ) < 0){
    perror("Erreur mprotect");
    exit(1);
  }
  countPages++;
}

int main() {
  int max = getpagesize();
  printf("taille page : %d\n", max);
  int mult = 6;
  int col = mult*(racine((double) max));
  printf("nombre de lignes et colonnes : %d\n", 8*col);
  int tailleM = col*col;
  printf("taille de M = %d multiple de page = %d\n", tailleM, mult);

  struct sigaction act, oact;
  sigemptyset(&act.sa_mask);
  act.sa_sigaction = protect;
  act.sa_flags = SA_SIGINFO;
  oact.sa_flags = SA_SIGINFO;
  int fd, val = 0;
  int caractereActuel= 0;
  int count = 0;
  int fsize;

  if((fd = open("file.txt", O_RDONLY)) == -1) {
    printf("génération de la matrice...\n");
    initMatrice(col, col);
    if((fd = open("file.txt", O_RDONLY)) == -1) {
      perror("Erreur d'ouverture du fichier en écriture - Main");
      exit(1);
    }
  }

  if((sigaction(SIGSEGV, &act, &oact)) == -1){
    perror("Erreur lors de la mise en place de la routine");
    exit(1);
  }

  addr = (char*)mmap(0, fsize, PROT_NONE, MAP_SHARED, fd, 0);
  if(fd < 0) {
    perror("Erreur d'ouverture du fichier en écriture - Main");
    exit(1);
  }
  for(int i=0 ; i<10; i++) {
    val = randInterval(0, tailleM);
    int res = (int) addr[val];
    printf("%d\n", res);
    printf("%d à l'indice dans le fichier %d\n", res, val );// on l'affiche
  }
  printf("Nb pages %d et pourcentage accédées %d \n", countPages, (countPages/mult)*100);
  munmap(addr, tailleM);

  close(fd);
  return 0;
}
