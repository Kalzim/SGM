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

  for(int i=1; i<4;i++){
    switch(i) {
      case 1: strcpy(my_msg.msg,"C'est bien");
      break;
      case 2: strcpy(my_msg.msg,"pratique ces");
      break;
      case 3: strcpy(my_msg.msg,"files de messages.");
      break;
    }
    size = strlen(my_msg.msg);
    printf("Message à envoyer n°%d : %s\n", i, my_msg.msg);
    envoi = msgsnd(id, &my_msg, size+1, 0);
    if(envoi == -1){
      perror("Echec envoi");
      exit(1);
    }
  }
  return 0;
}
