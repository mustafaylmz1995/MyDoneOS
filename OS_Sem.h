/****************************************************
 *  OS_Sem.h                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class OS_Sem       
 *  Original author: mustafa                     
 ****************************************************/

#if !defined(__OS_SEM_H__)
#define __OS_SEM_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include "config_constant.h"
#include "embed_queue.h"
#include "miros.h"

typedef struct
{
	/**
	 * max value semaphore can be
	 */
	uint16_t maxValue ;
	/**
	 * Semaphore Current Value
	 */
	volatile uint16_t value ;
	/**
	 * A list that holds any Thread that waits for semaphore unlock
	 */
	Queue_t waitSet[MAX_PRIO_U8+1] ;
  
  
}  OS_Semaphore;

/**
 * Set the values to the given Semaphore
 */
void OS_Sem_Init(OS_Semaphore *sem_addr, uint16_t val, uint16_t maxVal);
/**
 * Give Semaphore and increase semaphore value by one
 */
uint8_t OS_Sem_Sig(OS_Semaphore *sem_addr);
/**
 * Takes the Semaphore and decreases semaphore value by one
 */
uint8_t OS_Sem_Wait(OS_Semaphore *sem_addr);


#ifdef __cplusplus
}
#endif


#endif /*!defined(__OS_SEM_H__)*/
