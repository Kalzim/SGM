#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct my_message {
    long typeMsg;
    char msg[1000];
};

int main() {
  int id, envoi, size;
  struct my_message my_msg;
  my_msg.typeMsg = 1;

  if((id = msgget((key_t)1234, 0660 | IPC_CREAT | IPC_EXCL)) == -1) {
    perror("Erreur création file de messages");
    exit(1);
  }

  for(int i=1; i<6;i++){
  //  choixMessage(&my_msg, i);
    switch(i) {
      case 1: strcpy(my_msg.msg,"Message numéro 1\n");
      break;
      case 2: strcpy(my_msg.msg,"Message numéro 2\n");
      break;
      case 3: strcpy(my_msg.msg,"Message numéro 3\n");
      break;
      case 4: strcpy(my_msg.msg,"Message numéro 4\n");
      break;
      case 5: strcpy(my_msg.msg,"Message numéro 5\n");
      break;
    }
    size = strlen(my_msg.msg);
    envoi = msgsnd(id, &my_msg, size +1, 0);
    if(envoi == -1){
      perror("Echec envoi");
      exit(1);
    }
    else{
      printf("Message envoyé\n");
    }
  }
  return 0;
}
