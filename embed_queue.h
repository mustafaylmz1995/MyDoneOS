/****************************************************
 *  embed_queue.h                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class embed_queue       
 *  Original author: mustafa                     
 ****************************************************/

#if !defined(__EMBED_QUEUE_H__)
#define __EMBED_QUEUE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "thread.h"



typedef struct{
  
  void* q[MAX_SAME_PRIO_THREAD_U8+1];
  int8_t rear;
  int8_t front;
  uint8_t  noe; //number of element
  uint8_t  max;
  
}Queue_t;


void create_q(Queue_t *Queue);
void enqueue(Queue_t *Queue, uint32_t* valAddr);
void dequeue(Queue_t *Queue);
void* peek(Queue_t *Queue);


#ifdef __cplusplus
}
#endif


#endif /*!defined(__EMBED_QUEUE_H__)*/
