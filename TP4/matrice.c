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

int randInterval(int min, int max){
    return rand()%(max-min) +min;
}

void initMatrice(int lignes, int colonnes) {
  char rL [] = "\n";
  int val = 0;
  char c = 0;
  int fd = open("file.txt", O_RDWR | O_CREAT, 0666);
  if(fd < 0) {
    perror("Erreur d'ouverture du fichier en écriture - initMatrice");
  }
  char test [] ="ok\0";
  write(fd, &test, sizeof(test));

  for(int i=0; i< lignes; i++) {
    for(int j=0; j<colonnes; j++) {
      c = (char) randInterval(Min, Max);
      //printf("valeur générée : %d\n", (int) c);
      if (write(fd, &c, sizeof(c)) <0) {
        perror("Erreur en écriture dans le fichier");
      }
    }
  //  write(fd, &rL, sizeof(rL));
  }
  close(fd);
}

  void protect(int num, siginfo_t *sip, void* p){
      int nbPages = 0.25*getpagesize();
      if(mprotect(sip->si_addr, nbPages, PROT_READ) < 0){
        perror("Erreur mprotect");
        exit(0);
      }
      countPages++;
  }



int main() {
  struct sigaction act;
  struct sigaction oact;
  act.sa_sigaction = protect;
  act.sa_flags = SA_SIGINFO;
  int nbPages = 0.25*getpagesize();
  int maxRdm = nbPages*nbPages;
  int val = 0;
  int caractereActuel= 0;
  int count = 0;
  int fsize;
  printf("taille d'une page : %d\n", nbPages);
  int nbLignes = nbPages, nbColonnes = 5;
  initMatrice(nbPages, nbPages);

//  FILE *fd = fopen("file.txt", "r+");
//  fsize = lseek(fd, 0, SEEK_END);
  int fd = open("file.txt", O_RDONLY);
  if(sigaction(SIGSEGV, &act, &oact)){

  }
  printf("avant mmap\n");
  char *addr = (char*)mmap(0, fsize, PROT_NONE, MAP_SHARED, fd, 0);
  if(fd < 0) {
    perror("Erreur d'ouverture du fichier en écriture - Main");
  }
  for(int i=0 ; i<10; i++) {
    printf("entree for\n");
    val = randInterval(0, maxRdm);
  //  fseek(fd,val,0); //on déplace le curseur dans le fichier
  //  caractereActuel = fgetc(fd); // on lit un caractere
    printf("%d à l'indice dans le fichier %d\n",(int) addr[val],val );// on l'affiche
  }
  printf("sortie for\n");
  printf("Nb pages %d et pourcentage accédées %d \n", countPages, (countPages/nbPages)*100);
  munmap(addr, nbPages);

  close(fd);
  return 0;
}
