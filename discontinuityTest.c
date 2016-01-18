/**
Chris DeJarlais
TCSS 422
Operating Systems
discontinuityTest.c
demoed in office on 11-24-15
*/
#include "eventQueue.h"
int main()
{
    fifoQueue myReadyQueue = createQueue();
    time_t t;
    srand((unsigned)time(&t));
    FILE *fp;
    fp = fopen("eventlog.txt", "w+");
    int i;
    int process_id = START_INDEX;
    for(i = START_INDEX; i < TOTAL_PROCESSES; i++) {//enqueue some number of processes
        Enqueue(myReadyQueue, &process_id);//random run length
    }
    Dequeue(myReadyQueue,&process_id,fp);//start dequeue procedure
    return 0;
}
