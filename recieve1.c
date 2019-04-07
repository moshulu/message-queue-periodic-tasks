#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define QUEUE_NAME      "/my_queue"
#define MAX_MSG_LEN    10000
int main(int argc, char * argv[]) {
    mqd_t queue_id;
    char msgcontent[MAX_MSG_LEN];
    int msg_size;
    unsigned int priority;

    /* open the queue created by the sender */
    queue_id = mq_open(QUEUE_NAME, O_RDWR);
    if (queue_id == (mqd_t)-1) {
        perror("In mq_open()");
        exit(1);
    }
    while(1){
        msg_size=mq_receive(queue_id,msgcontent,MAX_MSG_LEN,&priority);
        //output message info
        printf("Recieved message. | content: %s | size: %d | priority: %d\n", msgcontent, msg_size, priority);
    }

    //close the queue
     mq_close(queue_id);

     return 0;
}
