/**
Chris DeJarlais
TCSS 422
Operating Systems
eventQueue.c
demoed in office on 11-24-15
*/
#include "eventQueue.h"
enum state_type{new, ready, running, waiting, halted};
struct PCB {
    int process_id;
    int ioLength;//io event length
    int mutexOwned;
    int mutexRun;//mutex hold length
    int runLength;//process run length
    int usedQuanta;
    int printer[IO_SIZE];
    int keyboard[IO_SIZE];
    int mouse[IO_SIZE];
    int monitor[IO_SIZE];
    int mutex1Request[IO_SIZE];
    int mutex2Request[IO_SIZE];
    int mutex3Request[IO_SIZE];
    int mutex4Request[IO_SIZE];
    enum state_type state;

};
struct Mutex {
    struct Queue * myFifo;
    int mutexId;
    int ownerId;
};
struct Node {
    struct Node * next;
    struct PCB * processes;
};
struct Queue {
    struct Node * first;
    struct Node * last;
    int size;
};
/*to create a fifoQueue structure
@return the new fifoQueue*/
fifoQueue createQueue() {
    fifoQueue que = (fifoQueue)malloc(sizeof(struct Queue));
    que->first = NULL;
    que->last = NULL;
    que->size = 0;
    return que;
}
/*create a mutex structure
@param myId the id of the mutex
@return the new mutex
*/
mutex createMutex(int * myId) {
    mutex myMutex = (mutex)malloc(sizeof(struct Mutex));
    myMutex->myFifo = createQueue();
    myMutex->ownerId = NA;//initialized to no owner
    myMutex->mutexId = *myId;
    ++*myId;
    return myMutex;
}
/*create a process control block
@param orderId the process number
@return the new pcb*/
pcb createPCB(int orderId) {
    int i;
    pcb new_pcb = (pcb)malloc(sizeof(struct PCB));
    new_pcb->runLength = PCB_RUN + rand() % PCB_RUN;//run between 500 and 1000
    new_pcb->usedQuanta = FALSE;
    new_pcb->ioLength = FALSE;
    new_pcb->process_id = orderId;
    new_pcb->state = ready;
    new_pcb->mutexOwned = NA;//initialized to dummy variable representing now owner
    new_pcb->mutexRun = NA;//initialized to no owner
    for(i = START_INDEX; i < IO_SIZE; i++) {
        if(i == START_INDEX) {
            new_pcb->printer[i] = rand() % IO_REQUEST  + OFFSET;//must be a fraction of total pcb run length and greater than 0
            new_pcb->keyboard[i] = rand() % IO_REQUEST + OFFSET ;
            new_pcb->mouse[i] = rand() % IO_REQUEST  + OFFSET;
            new_pcb->monitor[i] = rand() % IO_REQUEST  + OFFSET;
            new_pcb->mutex1Request[i] = rand() % MUTEX_REQUEST + OFFSET;
            new_pcb->mutex2Request[i] = rand() % MUTEX_REQUEST + OFFSET;
            new_pcb->mutex3Request[i] = rand() % MUTEX_REQUEST + OFFSET;
            new_pcb->mutex4Request[i] = rand() % MUTEX_REQUEST + OFFSET;
        } else {//ensure random values at subsequent indexes are larger
            new_pcb->printer[i] = new_pcb->printer[i - 1] + rand() % IO_REQUEST + OFFSET;
            new_pcb->keyboard[i] = new_pcb->keyboard[i - 1] + rand() % IO_REQUEST + OFFSET;
            new_pcb->mouse[i] = new_pcb->mouse[i - 1] + rand() % IO_REQUEST + OFFSET;
            new_pcb->monitor[i] = new_pcb->monitor[i - 1] + rand() % IO_REQUEST + OFFSET;
            new_pcb->mutex1Request[i] = new_pcb->mutex1Request[i - 1] + rand() % MUTEX_REQUEST + OFFSET;
            new_pcb->mutex2Request[i] = new_pcb->mutex2Request[i - 1] + rand() % MUTEX_REQUEST + OFFSET;
            new_pcb->mutex3Request[i] = new_pcb->mutex3Request[i - 1] + rand() % MUTEX_REQUEST + OFFSET;
            new_pcb->mutex4Request[i] = new_pcb->mutex4Request[i - 1] + rand() % MUTEX_REQUEST + OFFSET;
        }
    }
    return new_pcb;
}
/*create a node
@param new_pcb the process control block pointer
@return the new node
*/
node createNode(pcb myPcb) {
    node new_node = (node)malloc(sizeof(struct Node));
    new_node->processes = myPcb;
    new_node->next = NULL;
    return new_node;
}
/*to delete the entire list
@param head the front node in the list
*/
void deleteList(node *head){
    node curr = *head;
    node next;
    while(curr != NULL){
        next = curr->next;
        free(curr->processes);
        free(curr);
        curr = next;
    }
    *head = NULL;
}
/*to add a node and pcb to the queue
@param myFifo the fifoqueue to add to
@param id the process id number
*/
void Enqueue(fifoQueue myFifo, int * id) {
    pcb new_pcb = createPCB(*id);
    node new_node = createNode(new_pcb);
    addNode(myFifo, new_node);
    ++*id;
}
/*add a node to the queue or mutex
@param myFifo the fifoQueue or Mutex
@param myNode the pcb to include
*/
void addNode(fifoQueue myFifo, node myNode) {
    if(myFifo->first == NULL) {
        myFifo->first = myNode;
        myFifo->first->next = NULL;
    } else {
        if(myFifo->last == NULL) {
            myFifo->last = myNode;
            myFifo->first->next = myFifo->last;
        } else {
            myFifo->last->next = myNode;
            myFifo->last = myFifo->last->next;
        }
    }
    myFifo->size++;
}
/*dequeue a process to run
@param myReadyQueue the queue of processes waiting for CPU access
@param process_id the id of node
@param fp the file pointer to print to file
*/
void Dequeue(fifoQueue myReadyQueue, int * process_id, FILE * fp) {
    node temp;
    int index;
    int loopBreak;
    int mutexId = 1;
    mutex m1 = createMutex(&mutexId);//create Mutex's
    mutex m2 = createMutex(&mutexId);
    mutex m3 = createMutex(&mutexId);
    mutex m4 = createMutex(&mutexId);
    fifoQueue myPrinter = createQueue();//create io queue's
    fifoQueue myKeyboard = createQueue();
    fifoQueue myMouse = createQueue();
    fifoQueue myMonitor = createQueue();
    if(myReadyQueue->first != NULL){
        temp = returnFirst(&myReadyQueue);
        temp->processes->state = running;
        printf("\nprocess number %d running with run time %d", temp->processes->process_id,temp->processes->usedQuanta);
        while(myReadyQueue->first->next != NULL) {
            ++temp->processes->usedQuanta;
            if(clock()  > PERCENT * PCB_RUN)//clock over 50,000
            	return; 
            if(temp->processes->mutexOwned > NA) {//process is holding a mutex
                --temp->processes->mutexRun;//decrement mutex time counter
                if(temp->processes->mutexRun == 0) {//mutex counter up, unlock and reset fields
                    printf("\nprocess %d done with mutex %d", temp->processes->process_id, temp->processes->mutexOwned);
                    fprintf(fp, "\nprocess %d unlocked mutex m%d", temp->processes->process_id, temp->processes->mutexOwned);
                    unlockMutex(temp->processes->mutexOwned, myReadyQueue, m1, m2, m3, m4,fp);
                    temp->processes->mutexRun = NA;
                    temp->processes->mutexOwned = NA;
                }
            } else {//process can only hold one mutex at a time
                mutexRequest(&temp, myReadyQueue, m1, m2, m3, m4,fp);
            }
            ioRequired(&temp, myReadyQueue, myPrinter,myKeyboard, myMouse, myMonitor,fp);
            runIO(myReadyQueue, myPrinter,fp);
            runIO(myReadyQueue, myKeyboard, fp);
            runIO(myReadyQueue, myMouse, fp);
            runIO(myReadyQueue, myMonitor, fp);
            if(temp->processes->usedQuanta == temp->processes->runLength) {//process is done with run time, make random decision to reenqueue
                reenqueueNode(fp, process_id, myReadyQueue, temp);
                temp = returnFirst(&myReadyQueue);// run next node in readyQueue
            }
            if(myReadyQueue->size < TOTAL_PROCESSES)//size low, add process
                Enqueue(myReadyQueue, process_id);

        }
        fprintf(fp,"\ncontents of readyQueue:\n");//print state of all queues at end of run
        printFifo(myReadyQueue, fp);
        fprintf(fp, "\ncontents of mutex1:\n");
        printFifo(m1->myFifo, fp);
        fprintf(fp,"\ncontents of mutex2:\n");
        printFifo(m2->myFifo, fp);
        fprintf(fp,"\ncontents of mutex3:\n");
        printFifo(m3->myFifo, fp);
        fprintf(fp,"\ncontents of mutex4:\n");
        printFifo(m4->myFifo, fp);
        fprintf(fp,"\ncontents of printer:\n");
        printFifo(myPrinter, fp);
        fprintf(fp,"\ncontents of mouse:\n");
        printFifo(myMouse, fp);
        fprintf(fp,"\ncontents of monitor:\n");
        printFifo(myMonitor, fp);
        fprintf(fp,"\ncontents of keyboard:\n");
        printFifo(myKeyboard, fp);
        fclose(fp);
        exit(0);

    }
}
/*request a mutex form the process
@param myNode the node to request the mutex
@param myReadyQueue the queue waiting to access the cpu
@param m1 the first mutex
@param m2 the second mutex
@param m3 the third mutex
@param m4 the fourth mutex
*/
void mutexRequest(node *myNode, fifoQueue myReadyQueue, mutex m1, mutex m2, mutex m3, mutex m4, FILE *fp) {
    int i;
    int loopBreak;
    for(i = START_INDEX; i < IO_SIZE; i++) {
        loopBreak = TRUE;
        if((*myNode)->processes->usedQuanta == (*myNode)->processes->mutex1Request[i]) {
            lockMutex(myNode, myReadyQueue, m1, fp);
        } else if((*myNode)->processes->usedQuanta == (*myNode)->processes->mutex2Request[i]) {
            lockMutex(myNode, myReadyQueue, m2, fp);
        } else if((*myNode)->processes->usedQuanta == (*myNode)->processes->mutex3Request[i]) {
            lockMutex(myNode, myReadyQueue, m3, fp);
        } else if((*myNode)->processes->usedQuanta == (*myNode)->processes->mutex4Request[i]) {
            lockMutex(myNode, myReadyQueue, m4, fp);
        } else {
            loopBreak = FALSE;
        }
        if(loopBreak) {//node sent to lock a mutex, break out of loop
            break;
        }
    }
}
/*lock the mutex if currently unlocked, else wait
@param myNode the node requesting the mutex
@param myReadyQueue the cpu queue
@param myMutex the mutex that is requested
@param fp the file to print output to
*/
void lockMutex(node *myNode, fifoQueue myReadyQueue, mutex myMutex, FILE *fp) {
    if(myMutex->ownerId == NA) {//mutex not owned, grab it
        printf("\nprocess %d owns mutex m%d at quantum %d", (*myNode)->processes->process_id, myMutex->mutexId, (*myNode)->processes->usedQuanta);
        fprintf(fp, "\nprocess  %d owns mutex m%d at quantum %d", (*myNode)->processes->process_id, myMutex->mutexId, (*myNode)->processes->usedQuanta);
        (*myNode)->processes->mutexOwned = myMutex->mutexId;
        (*myNode)->processes->mutexRun = rand() % MUTEX_RUN_LENGTH + OFFSET;//run mutex for random amount of time between 5 and 34
        myMutex->ownerId = (*myNode)->processes->process_id;
    } else {//mutex owned, put node in waiting queue
        node temp = *myNode;
        temp->processes->state = waiting;
        fprintf(fp, "\nprocess  %d blocked waiting for mutex m%d at quantum %d", temp->processes->process_id, myMutex->mutexId, temp->processes->usedQuanta);
        printf("\nprocess %d waiting on mutex m%d", temp->processes->process_id, myMutex->mutexId);
        addNode(myMutex->myFifo, temp);
        (*myNode) = returnFirst(&myReadyQueue);
        usleep(300000);
    }
}
/*unlock mutex for next process
@param mutexId the id of mutex to unlock
@param myReadyQueue the queue of processes waiting for cpu
@param m1 the first mutex
@param m2 the second mutex
@param m3 the third mutex
@param m4 the fourth mutex
@param fp the file to print output to
*/
void unlockMutex(int mutexId,fifoQueue myReadyQueue, mutex m1, mutex m2, mutex m3, mutex m4, FILE *fp) {
    switch(mutexId) {
        case 1:
            unlockHelper(myReadyQueue, m1, fp);
            break;
        case 2:
            unlockHelper(myReadyQueue, m2, fp);
            break;
        case 3:
            unlockHelper(myReadyQueue, m3, fp);
            break;
        case 4:
            unlockHelper(myReadyQueue, m4, fp);
            break;
    }
}
/*helper method to for unlockMutex. Checks if process is waiting for mutex. If it is,
transfer ownership to first process in waiting queue and put it back in readyQueue.
@param myReadyQueue the queue of processes waiting for cpu
@param myMutex the mutex to unlock
@param fp the file to print output to
*/
void unlockHelper(fifoQueue myReadyQueue, mutex myMutex, FILE *fp) {
    myMutex->ownerId = NA;
    if(myMutex->myFifo->first != NULL) {
        node temp = returnFirst(&myMutex->myFifo);
        temp->processes->mutexOwned = myMutex->mutexId;
        temp->processes->mutexRun += rand() % MUTEX_RUN_LENGTH + OFFSET;
        temp->processes->state = ready;
        myMutex->ownerId = temp->processes->process_id;//relock mutex;
        fprintf(fp, "\nprocess %d unblocked and owns mutex m%d", temp->processes->process_id, myMutex->mutexId);
        printf("\nmutex m%d unlocked and now owned by process %d",myMutex->mutexId, myMutex->ownerId);
        usleep(300000);
        addNode(myReadyQueue, temp);
    }
}
/*check if currently running process needs io by scanning all io arrays it contains.
@param myNode the node to check for io request
@param myReadyQueue the queue of processes waiting for cpu access
@param myPrinter the printer io queue
@param myKeyboard the keyboard io queue
@param myMouse the mouse io queue
@param myMonitor the monitor io queue
@param fp the file to print output to
*/
void ioRequired(node *myNode, fifoQueue myReadyQueue, fifoQueue myPrinter,fifoQueue myKeyboard,fifoQueue myMouse,fifoQueue myMonitor, FILE *fp) {
    int index;
    int loopBreak;
    node temp;
    for(index = START_INDEX; index < IO_SIZE; index++) {
        loopBreak = TRUE;
        temp = *myNode;
        temp->processes->state = waiting;//put node in waiting while checking if I/O is required
        if(temp->processes->usedQuanta == temp->processes->printer[index]) {
            printf("\nprocess %d blocked for printer io",temp->processes->process_id);//add to printerQueue
            fprintf(fp, "\nprocess %d blocked for printer io at quantum %d", temp->processes->process_id, temp->processes->usedQuanta);
            temp->processes->ioLength = rand() % PRINTER_LENGTH + OFFSET;// random run between 5 and 79
            addNode(myPrinter,temp);
        } else if(temp->processes->usedQuanta == temp->processes->keyboard[index]) {
            printf("\nprocess %d blocked for keyboard io",temp->processes->process_id);//add to keyboardQueue
            fprintf(fp, "\nprocess  %d blocked for keyboard io at quantum %d", temp->processes->process_id, temp->processes->usedQuanta);
            temp->processes->ioLength = rand() % IO_LENGTH + OFFSET;//random run length
            addNode(myKeyboard,temp);
        } else if(temp->processes->usedQuanta == temp->processes->mouse[index]) {//add to mouseQueue
            printf("\nprocess %d blocked for mouse io at quantum %d",temp->processes->process_id, temp->processes->usedQuanta);
            fprintf(fp, "\nprocess %d blocked for mouse io at quantum %d",temp->processes->process_id, temp->processes->usedQuanta );
            temp->processes->ioLength = rand() % IO_LENGTH + OFFSET;
            addNode(myMouse,temp);
        } else if(temp->processes->usedQuanta == temp->processes->monitor[index]) {//add to monitorQueue
            printf("\nprocess %d blocked for monitor io at quantum %d",temp->processes->process_id, temp->processes->usedQuanta );
            fprintf(fp, "\nprocess %d blocked for monitor io at quantum %d",temp->processes->process_id, temp->processes->usedQuanta );
            temp->processes->ioLength = rand() % MONITOR_LENGTH + OFFSET;
            addNode(myMonitor,temp);
        } else {
            loopBreak = FALSE;//node did not request any I/O
            temp->processes->state = ready;
        }
        if(loopBreak) {//node requested IO-break out of loop, run next node in readyQueue
            (*myNode) = returnFirst(&myReadyQueue);
            printf("\nprocess number %d now running", (*myNode)->processes->process_id);
            fprintf(fp, "\nprocess  %d now running", (*myNode)->processes->process_id);
            usleep(300000);
            break;
        }
    }
}
/*reenqueue node at the end of the run
@param fp the file to print to
@param process_id the id of process for adding new node
@param myReadyQueue the processes waiting for cpu
@param myNode the node done with first run
*/
void reenqueueNode(FILE *fp, int *process_id, fifoQueue myReadyQueue, node myNode) {
    if((rand() % PERCENT) < REENQUEUE_CHANCE) {//put process back in readyQueue
        myNode->processes->usedQuanta = FALSE;//reinitialize quanta to 0
        myNode->processes->state = ready;
        myNode->processes->ioLength = FALSE;//io length set to 0
        addNode(myReadyQueue, myNode);
        printf("\nprocess number %d put back in queue", myNode->processes->process_id);
    } else {
        fprintf(fp, "\nprocess  %d removed from readyQueue ", myNode->processes->process_id);
        printf("\nprocess number %d dropped from queue", myNode->processes->process_id);
        Enqueue(myReadyQueue, process_id);//add node to account for dropped node
        free(myNode->processes);
        free(myNode->next);
        free(myNode);
    }
}
/*run io on the front node in io queue
@param myReadyQueue the processes waiting for cpu
@param myDevice the IO device to run
@param fp the file to print to
*/
void runIO(fifoQueue myReadyQueue, fifoQueue myDevice, FILE *fp) {
    if(myDevice->first == NULL)
        return;
    myDevice->first->processes->ioLength--;
    if(myDevice->first->processes->ioLength == START_INDEX) {//process done with io
        endIO(myReadyQueue, myDevice, fp);
    }
}
/*return next process for IO
@param myReadyQueue the processes waiting for cpu
@param myDevice the IO device to run
@param fp the file to print to
*/
void endIO(fifoQueue myReadyQueue, fifoQueue myDevice, FILE *fp ) {
    node temp;
    temp = returnFirst(&myDevice);
    temp->processes->state = ready;
    printf("\nprocess number %d done with I/O", temp->processes->process_id);
    fprintf(fp, "\nprocess %d done with I/O", temp->processes->process_id);
    addNode(myReadyQueue, temp);
    usleep(200000);
}
/*return first node in list
@param myFifo the queue from which to remove first element
*/
node returnFirst(fifoQueue *myFifo) {
    node temp = (*myFifo)->first;
    (*myFifo)->first = (*myFifo)->first->next;
    if((*myFifo)->first == NULL)
        (*myFifo)->last = NULL;
    temp->next = NULL;
    (*myFifo)->size--;
    return temp;
}
/*prints the fifo queue
@param myFifo the fifoqueue to print
@param fp the file to output to
*/
void printFifo(fifoQueue myFifo, FILE *fp) {
    node temp = myFifo->first;
    while(temp != NULL) {
        fprintf(fp, "\n%d ", temp->processes->process_id);
        temp = temp->next;
    }
    printf("\n");
}
/*checks if empty
@param myFifo the queue to check for empty
@return whether or not empty
*/
int is_empty(fifoQueue myFifo) {
    return myFifo->first == NULL;
}
/*peeks at first element id
@param myFifo the queue to peek at
@return process id of first element
*/
int peek(fifoQueue myFifo) {
    if(myFifo->first != NULL)
        return myFifo->first->processes->process_id;
}

