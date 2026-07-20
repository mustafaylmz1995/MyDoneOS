/****************************************************
 *  OS_Sem.c                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class OS_Sem       
 *  Original author: mustafa                     
 ****************************************************/

#include "OS_Sem.h"

/**
 * Set the values to the given Semaphore
 */
void OS_Sem_Init(OS_Semaphore *sem_addr, uint16_t val, uint16_t maxVal)
{
  sem_addr->value           = (uint16_t)val; //current value
  sem_addr->maxValue        = (uint16_t)maxVal; //max value it can take
  
  for(int8_t a = MAX_PRIO_U8; a>=0; a--){
    create_q(&( sem_addr->waitSet[a] ) );

  }
}

/**
 * Give Semaphore and increase semaphore value by one
 */
uint8_t OS_Sem_Sig(OS_Semaphore *sem_addr)
{
 uint8_t signaled = 1;
  
	__disable_irq();
	
		++(sem_addr->value);
		
		//highest prio thread on waitSet
    int8_t _prio = MAX_PRIO_U8;
    while(_prio+1){	//look each rows


    if(sem_addr->waitSet[_prio].noe == 0){  //bu priorityli queue bos

      _prio--;
      
    }else{
      
     
      OS_TCB *osDumyTCB_addr = (OS_TCB *)peek(&(sem_addr->waitSet[_prio]) );

      osDumyTCB_addr->status = READY;
      dequeue(&(sem_addr->waitSet[_prio]) );
      enqueue(&osReadyq[_prio], (uint32_t *)osDumyTCB_addr);
      
      OS_Sched();
      __enable_irq();
			__disable_irq();
    }
    
 
  }    
  __enable_irq();

  return signaled;
 

}

/**
 * Takes the Semaphore and decreases semaphore value by one
 */
uint8_t OS_Sem_Wait(OS_Semaphore *sem_addr)
{
 
  uint8_t signaled = 1;
	 
  __disable_irq();
	

	if( sem_addr->value > 0 ){
		//semaphore is free
	
		--(sem_addr->value);
		
	}else{
		//Semaphore is locked
		
		//We add WaitSet who want to semaphore //SUSPENDED STATE
    
    osCurrTCB_addr->status = SUSPENDED;
    enqueue(&(sem_addr->waitSet[osCurrTCB_addr->thread_priority]), (uint32_t *)osCurrTCB_addr); /* add to waiting-set */
    dequeue(&osReadyq[osCurrTCB_addr->thread_priority]);  /* remove from Ready queue*/
    

		while(sem_addr->value == 0U){
						
			OS_Sched();
			__enable_irq();
			__disable_irq();
		}
		
    dequeue(&(sem_addr->waitSet[osCurrTCB_addr->thread_priority]));
		--(sem_addr->value);
		
	}
	
	__enable_irq();
			
  return signaled;
  
} 

