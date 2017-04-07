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


liste_chainee ajoutListe(liste_chainee liste, int valeur) {
    element* nouvelElement = malloc(sizeof(element));
    nouvelElement->val = valeur;
    nouvelElement->next = liste;
    return nouvelElement;
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

// création de la liste chainée
  int i = 0;
  for(; i < 5; i++){
    list = ajoutListe(list, i);
  }

  afficherListe(list);

  my_sem = semget((key_t)1234, 1, 0660 | IPC_CREAT | IPC_EXCL);
  if(my_sem == -1) {
    perror("Erreur création sémaphore");
    exit(1);
  }

  semop(my_sem, &up, 1); //init

  // entrée en section critique
  semop(my_sem, &down, 1);
  printf("entree section critique !!\n");
  id = shmget((key_t)5678,sizeof(list),0660|IPC_CREAT);
  if (id<0){
		perror("Erreur création segment de mémoire partagée");
    exit(1);
	}

  compteur = shmat(id,NULL,0);
	if (compteur==NULL) {
		perror("Erreur attachement du segment de mémoire partagée");
		exit(1);
	}

  // ajout des données en mémoire partagée
  //((listeCh* ) compteur) = &list;
  memcpy (compteur, &list, sizeof(list));

  // sortie de section critique
  semop(my_sem, &up, 1);
  printf("sortie section critique !!\n");
  if(shmdt(compteur) < 0) {
		perror("Erreur détachement du segment de mémoire partagée");
		exit(1);
	}

  return 0;
}
