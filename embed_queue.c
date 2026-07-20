/****************************************************
 *  embed_queue.c                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class embed_queue       
 *  Original author: mustafa                     
 ****************************************************/

#include "embed_queue.h"


/**
 * Call to create queue
 */
void create_q(Queue_t *Queue)
{
	Queue->noe        = 0;
	Queue->rear       = -1;
	Queue->front	    = -1;
  Queue->max        = MAX_SAME_PRIO_THREAD_U8+1;

  
} 

//Enqueue
void enqueue(Queue_t *Queue, uint32_t* valAddr) {
	if (Queue->rear == (Queue->max - 1)) {
    if(Queue->front == 0){
      //printf("Overflow!\n");
      return;
    }else{
      Queue->rear = 0;
    }
	}else if ((Queue->front == -1) || (Queue->rear == -1)) {
		Queue->front = 0;
		Queue->rear = 0;
	}	else {
		Queue->rear += 1;
		
	}
//	queue[rear] = num;
  Queue->q[Queue->rear] = valAddr;
  Queue->noe++;
}


//Dequeue
void dequeue(Queue_t *Queue) {

	if ((Queue->front == -1) || (Queue->rear == -1) ) {
		//printf("Underflow!\n");
		return;
	}
	else {
    if (Queue->front == Queue->rear) {
			Queue->q[Queue->front] = NULL;
			Queue->front = -1;
			Queue->rear = -1;
		}
		else if (Queue->front == (Queue->max - 1)) {
			Queue->q[Queue->front] = NULL;
			Queue->front = 0;
		}
		else {
			Queue->q[Queue->front] = NULL;
			Queue->front+=1;
		}

	}
  Queue->noe--;


}



//Peek
void* peek(Queue_t *Queue) {

	if ((Queue->front == -1) || (Queue->rear == -1)) {
		//printf("Queue is empty!\n");
    return ((void*)0);
	}
	else {
		return Queue->q[Queue->front];
	}

	
}


