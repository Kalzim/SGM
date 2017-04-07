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

liste_chainee ajoutListe(liste_chainee liste, int valeur) {
    element* nouvelElement = malloc(sizeof(element));
    nouvelElement->val = valeur;
    nouvelElement->next = liste;
    return nouvelElement;
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

void libererMemoire(liste_chainee liste) {
  if(liste == NULL) return;
  else {
    element *tmp;
    tmp = liste->next;
    free(liste);
    libererMemoire(tmp);
  }
}

int main() {
  // semaphores
  int my_sem;
  unsigned short val[2] = {0, 1};
  struct sembuf upPlein = {0, 1, 0};
  struct sembuf downPlein = {0, -1, 0};
  struct sembuf upVide = {1, 1, 0};
  struct sembuf downVide = {1, -1, 0};

  liste_chainee list = NULL;
  int id;
  void *compteur;

  // création de la liste chainée
  for(int i = 5; i > 0; i--){
    list = ajoutListe(list, i);
  }
  printf("Liste chaînée à envoyer : ");
  afficherListe(list);

  if((my_sem = semget((key_t)1234, 2, 0660 | IPC_CREAT | IPC_EXCL)) == -1) {
    perror("Erreur création des sémaphores");
    exit(1);
  }

  /* Initialisation des semaphores */
  if(semctl(my_sem, 0, SETALL, val) == -1) {
    perror("Erreur lors de l'initialisation des semaphores ");
    exit(1);
  }

  if((id = shmget((key_t)5678,5*sizeof(liste_chainee),0660|IPC_CREAT)) == -1) {
		perror("Erreur création segment de mémoire partagée");
    exit(1);
	}

  if((compteur = shmat(id,NULL,0)) == NULL) {
		perror("Erreur attachement du segment de mémoire partagée");
		exit(1);
	}

  element *tmp = list;
  for(int i=0; i<5; i++) {
    // P semaphore vide
    if(semop(my_sem, &downVide, 1) == -1) {
      perror("Erreur lors de l'operation sur le semaphore ");
      exit(1);
    }

    int val = tmp->val;
    memcpy (compteur, &val, sizeof(int));

    // V semaphore plein
    if(semop(my_sem, &upPlein, 1) == -1) {
      perror("Erreur lors de l'operation sur le semaphore ");
      exit(1);
    }
    tmp = tmp->next;
  }

  printf("Envoi terminé !\n");

  if(shmdt(compteur) < 0) {
		perror("Erreur détachement du segment de mémoire partagée");
		exit(1);
	}

  libererMemoire(list);
  return 0;
}
