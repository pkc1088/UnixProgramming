#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

int tc = 1;
int h = 0;
sem_t bear, pot;

void *beefun(void *arg) {
    while(1) {
        sem_wait(&pot);
        h++;
        printf("Produced item: %d\n", h);
        if(h == 5) sem_post(&bear); 
        else sem_post(&pot);
    }
    return NULL;
}

void *bearfun(void *arg) {
    while(1) {
        sem_wait(&bear); 
        h = 0;
        printf("%d turn - Bear Consumed item: %d\n", tc, h);
        
        //for(int i = 0; i < 5; i++) 
        sem_post(&pot);   // 생산자에게 신호 보냄
        
        if(tc++ == 3) exit(0);
    }
    return NULL;
}

int main() {
    
    pthread_t producer_thread, consumer_thread;

    // 세마포어 초기화
    sem_init(&pot, 0, 1);
    sem_init(&bear, 0, 0);
    //sem_init(&pot, 0, 1);

    // 생산자, 소비자 스레드 생성
    pthread_create(&consumer_thread, NULL, bearfun, NULL);
    for(int i = 0; i < 5; i++)
        pthread_create(&producer_thread, NULL, beefun, NULL);
    
    // 생산자, 소비자 스레드 종료 대기
    for(int i = 0; i < 5; i++)
        pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // 세마포어 해제
    sem_destroy(&pot);
    sem_destroy(&bear);
    //sem_destroy(&pot);
    
    return 0;
}
