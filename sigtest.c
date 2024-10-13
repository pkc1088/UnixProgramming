#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <string.h> 
#include <unistd.h> 
#include <ncurses.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sem.h>

/*
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t usrthread, displaythread;
sem_t *sem;
char cname[10];
int kill_flag = 0, val, i = 0;

void sigint_handler(int signum) {     
  printf("\nSIGINT from %s\n", cname);
  pthread_cancel(usrthread);
  //pthread_cancel(displaythread);
}

void cleanup(void *arg) {
    sem_post(sem); // 세마포어 해제
    printf("Cleanup handler: sem_post called\n");
}
*/
/*
void *read_messages(void *arg) {
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
  pthread_cleanup_push(cleanup, NULL);

  while(1) {
    sem_wait(sem);
    
    printf("add : i : %d\n", i++);
    if(i == 10 || i == -10) { break;}

    sem_post(sem);
    sleep(1);
  }
  printf("read_messages() out\n");
}

void *display(void *arg) {
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
  pthread_cleanup_push(cleanup, NULL);
  
  while(1) {
    sem_wait(sem);

    printf("minus : i : %d\n", i--);
    if(i == 10 || i == -10) { break;}
    
    sem_post(sem);
    sleep(1);
  }
  printf("display() out\n");

}
 


int main(int argc, char *argv[]) {
    
  strcpy(cname, argv[1]);
  sem = sem_open(argv[1], O_CREAT, 0666, 1);
  if (sem == SEM_FAILED)
  {
      perror("sem_open");
      exit(EXIT_FAILURE);
  }
  
  signal(SIGINT, sigint_handler);
  
  pthread_create(&usrthread, NULL, read_messages, (void *) argv[1]);          
  pthread_create(&displaythread, NULL, display, (void *) argv[1]);
  
  pthread_join(usrthread, NULL);
  pthread_join(displaythread, NULL);
  
  sem_unlink(argv[1]);
  sem_close(sem);
  
  printf("end\n");
 
  return 0;
}
*/

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t usrthread, displaythread, tempthread;
sem_t *sem;
char cname[10];
int kill_flag = 0, val, i = 0;

void sigint_handler(int signum) {     
  printf("\nSIGINT from %s\n", cname);
  pthread_cancel(usrthread);
  pthread_cancel(displaythread);
}

void cleanup(void *arg) {
    sem_post(sem); // 세마포어 해제
    printf("Cleanup handler: sem_post called\n");
}

void *read_messages(void *arg) {
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  //pthread_cleanup_push(cleanup, NULL);

  while (1) {
    sem_wait(sem);
    
    printf("add : i : %d\n", i++);
    if (i == 10 || i == -10) {
      sem_post(sem); // 중복된 세마포어 해제를 피하기 위해 필요
      break;
    }

    sem_post(sem);
    sleep(1);
  }
  printf("read_messages() out\n");
  //pthread_cleanup_pop(1); // 1 to execute the cleanup handler
  return NULL;
}

void *display(void *arg) {
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  //pthread_cleanup_push(cleanup, NULL);
  
  while (1) {
    sem_wait(sem);

    printf("minus : i : %d\n", i--);
    if (i == 10 || i == -10) {
      sem_post(sem); // 중복된 세마포어 해제를 피하기 위해 필요
      break;
    }
    
    sem_post(sem);
    sleep(1);
  }
  printf("display() out\n");
  //pthread_cleanup_pop(1); // 1 to execute the cleanup handler
  return NULL;
}

void *afterSignal(void *arg) {
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
  
  while(1) {
    sem_wait(sem);

    if(i == 10 || i == -10) {
      
    }


    sem_post(sem);
    sleep(1);
  }

  return;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <semaphore_name>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  strcpy(cname, argv[1]);
  sem = sem_open(argv[1], O_CREAT, 0666, 1);
  if (sem == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  signal(SIGINT, sigint_handler);

  pthread_create(&usrthread, NULL, read_messages, NULL);          
  pthread_create(&displaythread, NULL, display, NULL);
  pthread_join(usrthread, NULL);
  pthread_join(displaythread, NULL);
  // 위 두 스레드 종료 후 tempthread 실시해서 lock 걸고 진입할 수 있는지 확인.
  pthread_create(&tempthread, NULL, afterSignal, NULL);
  

  sem_unlink(argv[1]);
  sem_close(sem);

  printf("end\n");

  return 0;
}

