#include <stdio.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>

typedef struct element element;

struct element {
    int val;
    struct element *next;
};
typedef element* liste_chainee;

void libererMemoire(liste_chainee liste) {
  if(liste == NULL) return;
  else {
    element *tmp;
    tmp = liste->next;
    free(liste);
    libererMemoire(tmp);
  }
}

void afficherListe(liste_chainee liste) {
    element *tmp = liste;
    printf("{");
    while(tmp != NULL) {
      if(tmp->next == NULL) printf("%d", tmp->val);
      else printf("%d, ", tmp->val);
      tmp = tmp->next;
    }
    printf("}\n");
}

liste_chainee ajoutListe(liste_chainee liste, int valeur) {
  int loop = 1;
  element* nouvelElement = malloc(sizeof(element));
  nouvelElement->val = valeur;
  element* tmp = liste;
  if(tmp == NULL) {
    return nouvelElement;
  }
  else {
    while(loop) {
      if(tmp->next == NULL) {
        tmp->next = nouvelElement;
        loop = 0;
      }
      tmp = tmp->next;
    }
  }
  return liste;
}

int main() {
  // semaphores
  int my_sem;
  struct sembuf upPlein = {0, 1, 0};
  struct sembuf downPlein = {0, -1, 0};
  struct sembuf upVide = {1, 1, 0};
  struct sembuf downVide = {1, -1, 0};

  liste_chainee list = NULL;
  int id, res = 0;
  void *compteur;

  if((my_sem = semget((key_t)1234, 0, 0)) == -1) {
    perror("Erreur création sémaphore");
    exit(1);
  }

  if ((id = shmget((key_t)5678,5*sizeof(liste_chainee),0660)) == -1){
		perror("Erreur création segment de mémoire partagée");
    exit(1);
	}

	if ((compteur = shmat(id,NULL,0)) == NULL) {
		perror("Erreur attachement du segment de mémoire partagée");
		exit(1);
	}

  printf("Début de récupération de la liste chaînée\n");
  // récupération de la liste chainée
  for(int i=0; i<5; i++) {
    // P semaphore plein
    if(semop(my_sem, &downPlein, 1) == -1) {
      perror("Erreur lors de l'operation sur le semaphore ");
      exit(1);
    }

    memcpy(&res, compteur, sizeof(int));
    list = ajoutListe(list, res);

    // V semaphore vide
    if(semop(my_sem, &upVide, 1) == -1) {
      perror("Erreur lors de l'operation sur le semaphore ");
      exit(1);
    }
  }
  printf("Liste chaînée reçue : ");
  afficherListe(list);
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
