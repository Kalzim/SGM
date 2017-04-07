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
  int id, envoi, size;
  printf("b\n" );
  struct my_message my_msg;
  printf("c\n" );
  my_msg.typeMsg = 1;
  printf("d\n" );

  printf("Avant creation file\n" );
  id = msgget((key_t)1234, 0660 | IPC_CREAT | IPC_EXCL);
  if(id == -1) {
    perror("Erreur création file msg");
    exit(1);
  }

  strcpy(my_msg.msg,"Je suis trop fort en SGM !!!\n");
  size = strlen(my_msg.msg);

  for(int i=0; i<5;i++){
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
