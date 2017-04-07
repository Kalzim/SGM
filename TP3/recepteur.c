#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct my_message{
   long typeMsg;
   char msg[1000];
};

int main() {
  int id, reception;
  struct my_message my_msg;
  my_msg.typeMsg = 1;

  if((id = msgget((key_t)1234, 0660)) == -1) {
    perror("Erreur création file msg");
    exit(1);
  }

  for(int i=1; i<6;i++){
    if((reception = msgrcv(id, &my_msg, 1000, 0, 0)) == -1){
      perror("Echec reception");
      exit(1);
    }
    else{
      printf("Message reçu\n");
    }
    printf("Message n° %d : %s\n",i, my_msg.msg);
  }


  if(msgctl(id, IPC_RMID, 0) == -1){
    perror("Erreur destruction semaphores");
    exit(1);
  }


  return 0;
}
