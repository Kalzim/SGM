#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*
typedef struct {
    int val1;
    int val2;
}listeCh;*/

typedef struct element element;

struct element {
    int val;
    struct element *next;
};
typedef element* liste_chainee;

// semaphores
struct sembuf sbuf;
struct sembuf up = {0, 1, 0};
struct sembuf down = {0, -1, 0};

void libererMemoire(liste_chainee liste) {
  printf("entree liberation\n");
  if(liste == NULL) return;
  else {
    /* Sinon, on efface le premier élément et on retourne le reste de la
    liste effacée */
    element *tmp;
    tmp = liste->next;
    free(liste);
    libererMemoire(tmp);
  }
}

void afficherListe(liste_chainee liste) {
    element *tmp = liste;
    while(tmp != NULL) {
        printf("%d \n", tmp->val);
        tmp = tmp->next;
    }
}

int main() {
  liste_chainee list = NULL;
  int my_sem, id;
  void *compteur;

  my_sem = semget((key_t)1234, 1, 0660);
  if(my_sem == -1) {
    perror("Erreur création sémaphore");
    exit(1);
  }
  printf("semaphore recupere\n");
  // entrée en section critique
  semop(my_sem, &down, 1);
  printf("entree section critique !!\n");
  id = shmget((key_t)5678,sizeof(liste_chainee),0660);
  if (id<0){
		perror("Erreur création segment de mémoire partagée");
    exit(1);
	}
  printf("shmget OK\n");

  compteur = shmat(id,NULL,0);
	if (compteur==NULL) {
		perror("Erreur attachement du segment de mémoire partagée");
		exit(1);
	}
  printf("attachement OK\n");
  memcpy(&list, compteur, sizeof(compteur));
  // sortie de section critique
  semop(my_sem, &up, 1);
  printf("sortie section critique !!\n");


  afficherListe(list);
  printf("fin d'affichage\n");
  libererMemoire(list);

  if(shmdt(compteur) < 0) {
		perror("Erreur détachement du segment de mémoire partagée");
		exit(1);
	}

  if(shmctl(id, 0, 0)== -1){
    perror("Erreur destruction du segment de mémoire partagée");
    exit(1);
  }

  if(semctl(my_sem, 1, IPC_RMID)){
    perror("Erreur destruction semaphores");
    exit(1);
  }

  return 0;
}
