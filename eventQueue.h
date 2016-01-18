/**
Chris DeJarlais
TCSS 422
Operating Systems
eventQueue.h
demoed in office on 11-24-15
*/
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include<math.h>
#include<unistd.h>
#ifndef EVENTQUEUE_H_INCLUDED
#define EVENTQUEUE_H_INCLUDED
#define ARRAY_SIZE 1
#define START_INDEX 0
#define PERCENT 100
#define PCB_RUN 500
#define MUTEX_REQUEST 120
#define IO_REQUEST 115
#define REENQUEUE_CHANCE 75
#define MUTEX_RUN_LENGTH 20
#define IO_LENGTH 25
#define PRINTER_LENGTH 70
#define MONITOR_LENGTH 80
#define TRUE 1
#define FALSE 0
#define NA -1
#define TOTAL_PROCESSES 30
#define IO_SIZE 4
#define OFFSET 5
typedef struct Queue * fifoQueue;
typedef struct PCB * pcb;
typedef struct Node * node;
typedef struct Mutex * mutex;
fifoQueue createQueue();
pcb createPCB(int orderId);
node createNode(pcb myPCB);
mutex createMutex(int *myid);
void printFifo(fifoQueue myFifo, FILE *fp);
void Enqueue(fifoQueue myReadyQueue, int * id);
void Dequeue(fifoQueue myReadyQueue, int * id, FILE * fp);
int is_empty(fifoQueue myReadyQueue);
int peek(fifoQueue myReadyQueue);
void addNode(fifoQueue myFifo, node myNode);
void deleteList(node * head);
void runIO(fifoQueue myReadyQueue, fifoQueue myDevice, FILE *fp);
void reenqueueNode(FILE *fp, int *process_id, fifoQueue myReadyQueue, node myNode);
void endIO(fifoQueue myReadyQueue, fifoQueue myDevice, FILE *fp );
void lockMutex(node *myNode, fifoQueue myReadyQueue, mutex myMutex, FILE *fp);
void mutexRequest(node *myNode, fifoQueue myReadyQueue, mutex m1, mutex m2, mutex m3, mutex m4, FILE *fp);
void unlockMutex(int mutexId,fifoQueue myReadyQueue, mutex m1, mutex m2, mutex m3, mutex m4, FILE *fp);
void unlockHelper(fifoQueue myReadyQueue, mutex myMutex, FILE *fp);
void ioRequired(node *myNode, fifoQueue myReadyQueue, fifoQueue myPrinter,fifoQueue myKeyboard,fifoQueue myMouse,fifoQueue myMonitor, FILE *fp);
node returnFirst(fifoQueue*myFifo);
#endif // EVENTQUEUE_H_INCLUDED
