#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <setjmp.h>
#include <errno.h>
#include <err.h>
/*
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void prepare(void) {
    printf("preparing locks\n");
    pthread_mutex_lock(&lock1);
    pthread_mutex_lock(&lock2);
}
void parent(void) {
    printf("parent unlocking\n");
    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);
}
void child(void) {
    printf("child unlocking\n");
    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);
}
void *thread_func(void *arg) {printf("thread started\n"); }

int main() {
    int err; pid_t pid; pthread_t tid;

    pthread_atfork(prepare, parent, child);
    pthread_create(&tid, NULL, thread_func, 0);
    sleep(1);
    printf("parent is about to fork....\n");
    if((pid = fork()) < 0) return -1;
    if(pid == 0) {printf("child returned from fork\n");}
    else printf("parent returned from fork\n");

    return 0;
}

*/

/*
#include <setjmp.h>
static __jmp_buf buffer;
static void f2(void) {longjmp(buffer, 1);}
static void f1(int i, int j, int k) {
    printf("f1 (count, val, sum) : (%d, %d, %d)\n", i, j, k);
    f2();
}
int main(void) {
    int count;
    register int val;
    volatile int sum;

    count = 1; val = 2; sum = 3;
    if(setjmp(buffer) != 0) {
        printf("after longjmp : (count, val, sum) : (%d, %d, %d)\n", count, val, sum);
        exit(0);
    }
    count = 100; val = 200; sum = 300;
    f1(count, val, sum);
}
*/

/*
static void sig_quit(int);
int main(void) {
    sigset_t newmask, oldmask, pendmask;
    signal(SIGQUIT, sig_quit);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    sleep(5);
    if(sigismember(&pendmask, SIGQUIT)) printf("\nsigquit pending\n");

    sigprocmask(SIG_SETMASK, &oldmask, NULL);
    printf("sigquit unblocked\n");
    sleep(5);
    exit(0);
}
static void sig_quit(int signo) {
    printf("caught sigquit\n");
    signal(SIGQUIT, SIG_DFL);
}
*/

/*
static void sig_usr1(int), sig_alrm(int);
static sigjmp_buf buffer;
static volatile sig_atomic_t canjump;

int main(void) {
    signal(SIGUSR1, sig_usr1);
    signal(SIGALRM, sig_alrm);

    pr_mask("starting main: ");

    if(__sigsetjmp(buffer, 1)) {pr_mask("ending main: "); exit(0);}
    
    canjump = 1;
    for(;;) pause();
}

static void sig_usr1(int signo)  {
    time_t starttime;
    if(canjump == 0) return;
    pr_mask("starting sig_usr1: ");
    alarm(3);
    starttime = time(NULL);
    for(;;) if(time(NULL) > starttime + 5) break;
    pr_mask("finishing sig_usr1: ");
    canjump = 0;
    siglongjmp(buffer, 1);
}

static void sig_alrm(int signo) {pr_mask("in sig_alrm: ");}

//starting main: 
//starting sig_usr1:  SIGUSR1
//in sig_alrm:  SIGUSR1 SIGALRM
//finishing sig_usr1:  SIGUSR1
//ending main:  
*/

// pr_mask 함수 정의
void pr_mask(const char *msg) {
    sigset_t sigset;
    int errno_save;

    errno_save = errno; // 현재 errno 저장
    if (sigprocmask(0, NULL, &sigset) < 0) {
        perror("sigprocmask error");
    } else {
        printf("%s", msg);
        if (sigismember(&sigset, SIGINT)) printf(" SIGINT");
        if (sigismember(&sigset, SIGQUIT)) printf(" SIGQUIT");
        if (sigismember(&sigset, SIGUSR1)) printf(" SIGUSR1");
        if (sigismember(&sigset, SIGALRM)) printf(" SIGALRM");
        // 추가로 필요한 시그널을 여기에 추가
        printf("\n");
    }
    errno = errno_save; // 저장된 errno 복원
}

/*
static void sig_int(int);
int main(void) {
    sigset_t newmask, oldmask, waitmask;
    pr_mask("program start : ");

    signal(SIGINT, sig_int);
    
    sigemptyset(&waitmask);
    sigaddset(&waitmask, SIGUSR1);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    pr_mask("in cirtical region : ");

    if(sigsuspend(&waitmask) != -1)  ;
        //err_sys("sigsuspend error");

    //sigsuspend(&newmask);
    pr_mask("after return from suspend : ");

    sigprocmask(SIG_SETMASK, &oldmask, NULL);
    pr_mask("program exit : ");

    exit(0);
}

static void sig_int(int signo) {pr_mask("\nin sig_int : ");}

*/

/*
volatile sig_atomic_t quitflag;
static void sig_int(int signo) {
    if(signo == SIGINT) printf("\n..Interrupt\n");
    else if(signo == SIGQUIT) quitflag = 1;
}

int main(void) {
    sigset_t newmask, oldmask, zeromask;
    signal(SIGINT, sig_int);
    signal(SIGQUIT, sig_int);
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);

    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    
    while(quitflag == 0) sigsuspend(&newmask);

    //quitflag = 0;
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
    printf("program finished\n");
    exit(0);
}
*/


#include <sys/types.h>
static volatile sig_atomic_t sigflag;
static sigset_t newmask, oldmask, zeromask;

static void sig_usr(int signo) {sigflag = 1;}   // 시그널이 발생하면 flag를 1로 셋팅

void TELL_WAIT(void) {
    signal(SIGUSR1, sig_usr);
    signal(SIGUSR2, sig_usr);
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);

    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
}
void WAIT_PARENT(void) {
    while(sigflag == 0) sigsuspend(&zeromask);
    sigflag = 0;
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}
void WAIT_CHILD(void) {
    while(sigflag == 0) sigsuspend(&zeromask);
    sigflag = 0;
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}
void TELL_PARENT(pid_t pid) {kill(pid, SIGUSR2);}
void TELL_CHILD(pid_t pid) {kill(pid, SIGUSR1);}  // 자식 pid에 sigusr1을 발생시킴

static void charatatime(char *str) {
    char *ptr;
    int c;
    setbuf(stdout, NULL);
    for(ptr = str; c = *ptr++;) putc(c, stdout);
}

int main(void) {
    
    pid_t pid; 
    char *str = malloc(10 * sizeof(char));
    
    TELL_WAIT();
    if((pid = fork()) < 0) {perror("fork failed\n");}
    else if (pid == 0) {     // child
        for(int i = 0; i < 10; i++) {
            WAIT_PARENT();   //parent goes first
            charatatime("output from child : "); sprintf(str, "%d", i); charatatime(str); charatatime("\n");
            TELL_PARENT((getppid()));   // 자식은 부모의 pid를 넘김
        }
    } else {                // parent
        for(int i = 0; i < 10; i++) {
            //WAIT_CHILD();  
            charatatime("output from parent : "); sprintf(str, "%d", i); charatatime(str); charatatime("\n");
            TELL_CHILD(pid);    // 부모는 자식의 pid를 넘김
            WAIT_CHILD();
        }
    }

    free(str);
    exit(0);
}
