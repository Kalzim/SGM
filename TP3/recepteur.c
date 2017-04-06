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
/*typedef struct message message;

struct message{

 long typeMsg;
 int msg;

};
typedef message* my_message;*/


int main() {
  printf("a\n" );
  int id, reception;
  printf("b\n" );
  struct my_message my_msg;
  printf("c\n" );
  my_msg.typeMsg = 1;
  printf("d\n" );


  id = msgget((key_t)1234, 0660);
  if(id == -1) {
    perror("Erreur création file msg");
    exit(1);
  }
  printf("file de msg recupere\n");

  for(int i=0; i<5;i++){
    reception = msgrcv(id, &my_msg, 1000, 0, 0);
    if(reception == -1){
      perror("Echec reception");
      exit(1);
    }
    else{
      printf("Message reçu\n");
    }

    printf("Message n° %d : %s\n",i, my_msg.msg);
  }


  if(msgctl(id, IPC_RMID, 0)== -1){
    perror("Erreur destruction semaphores");
    exit(1);
  }


  return 0;
}
