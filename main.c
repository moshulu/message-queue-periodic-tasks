#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#define QUEUE_NAME "/my_queue"
#define MAX_MSG_LEN 100

#define ONE_THOUSAND 1000
#define ONE_MILLION 1000000
/* offset and period are in microseconds. */
#define OFFSET 1000000
#define PERIOD 500000

sigset_t sigst;
pid_t my_pid;
mqd_t queue_id;
unsigned int msg_priority = 0;
char msgcontent[MAX_MSG_LEN];

static void wait_next_activation(void){
    int dummy;
    /* suspend calling process until a signal is pending */
    sigwait(&sigst, &dummy);
}

int start_periodic_timer(uint64_t offset, int period){
    struct itimerspec timer_spec;
    struct sigevent sigev;
    timer_t timer;
    const int signal = SIGALRM;
    int res;

    /* set timer parameters */
    timer_spec.it_value.tv_sec = offset / ONE_MILLION;
    timer_spec.it_value.tv_nsec = (offset % ONE_MILLION) * ONE_THOUSAND;
    timer_spec.it_interval.tv_sec = period / ONE_MILLION;
    timer_spec.it_interval.tv_nsec = (period % ONE_MILLION) * ONE_THOUSAND;

    sigemptyset(&sigst);  /* initialize a signal set */
    sigaddset(&sigst, signal);  /* add SIGALRM to the signal set */
    sigprocmask(SIG_BLOCK, &sigst, NULL);  /* block the signal */

    /* set the signal event at timer expiration */
    memset(&sigev, 0, sizeof(struct sigevent));
    sigev.sigev_notify = SIGEV_SIGNAL;
    sigev.sigev_signo = signal;

    /* create timer */
    res = timer_create(CLOCK_MONOTONIC, &sigev, &timer);

    if (res < 0) {
        perror("Timer Create");
        exit(-1);
    }

    /* activiate the timer */
    return timer_settime(timer, 0, &timer_spec, NULL);
}

static void task_body(void){
    static int cycles = 0;
    static uint64_t start;
    uint64_t current;
    struct timespec tv;

    if (start == 0) {
        clock_gettime(CLOCK_MONOTONIC, &tv);
        start = tv.tv_sec * ONE_THOUSAND + tv.tv_nsec / ONE_MILLION;
    }

    clock_gettime(CLOCK_MONOTONIC, &tv);
    current = tv.tv_sec * ONE_THOUSAND + tv.tv_nsec / ONE_MILLION;
    if (cycles > 0){
        sprintf(msgcontent, "%lun", (unsigned long)time(NULL));
	if(mq_send(queue_id,msgcontent,strlen(msgcontent)+1, msg_priority)==0){
	    printf("message sent | content: %s\n", msgcontent);
        }
    }

    cycles ++;
}

int main(int argc, char *argv[]){
    int res;
    unsigned int msg_priority = 0;
    my_pid = getpid();

    queue_id = mq_open(QUEUE_NAME,O_RDWR|O_CREAT|O_EXCL,S_IRWXU|S_IRWXG,NULL);

    if(queue_id == (mqd_t)-1){
        if(errno == EEXIST){
            queue_id = mq_open(QUEUE_NAME,O_RDWR);
        }
        else{
            exit(1);
        }
    }

    /* set and activate a timer */
    res = start_periodic_timer(OFFSET, PERIOD);
    if (res < 0) {
        perror("Start Periodic Timer");
        return -1;
    }

    while(1) {
        wait_next_activation(); /* wait for timer expiration */
        task_body();  /* executes the task */
    }

    return 0;
}

