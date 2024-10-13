#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define true 1

int balance = 0;

// 뮤텍스와 컨디션 변수
pthread_mutex_t lock;
pthread_cond_t newDeposit;

// 함수 프로토타입
void* DepositThread(void* arg);
void* WithdrawThread(void* arg);
void withdraw(int amount);
void deposit(int amount);

int main() {
    // 뮤텍스와 조건변수 초기화
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&newDeposit, NULL);

    // 스레드 생성
    pthread_t depositThread, withdrawThread;
    pthread_create(&depositThread, NULL, DepositThread, NULL);
    pthread_create(&withdrawThread, NULL, WithdrawThread, NULL);

    pthread_join(depositThread, NULL);
    pthread_join(withdrawThread, NULL);

    // 뮤텍스와 조건변수 제거
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&newDeposit);

    return 0;
}

void* DepositThread(void* arg) {
    while (true) {
        deposit((rand() % 10) + 1);
        sleep(1);
    }
    return NULL;
}

void* WithdrawThread(void* arg) {
    while (true) {
        withdraw((rand() % 10) + 1);
    }
    return NULL;
}

void withdraw(int amount) {     // 출금
    pthread_mutex_lock(&lock);
    while (balance < amount) {
        printf("\t\t\tWait for a deposit\n");
        pthread_cond_wait(&newDeposit, &lock);
    }
    balance -= amount;
    printf("\t\t\tWithdraw %d\t\t%d\n", amount, balance);
    pthread_mutex_unlock(&lock);
}

void deposit(int amount) {      // 입금
    pthread_mutex_lock(&lock);
    balance += amount;
    printf("Deposit %d\t\t\t\t\t%d\n", amount, balance);
    pthread_cond_signal(&newDeposit);
    pthread_mutex_unlock(&lock);
}
