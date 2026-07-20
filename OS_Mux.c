/****************************************************
 *  OS_Mux.c                                         
 *  Created on: 27-�ub-2022 17:14:06                      
 *  Implementation of the Class OS_Mux       
 *  Original author: mustafa                     
 ****************************************************/

#include "OS_Mux.h"

/**
 * Set the values to the given Mutex Struct data type
 */
void OS_Mux_Init(OS_Mutex *mux_addr, uint8_t ceilPrio)
{
  mux_addr->value           = 1U; //up-down counter (always binary)
  mux_addr->ceilingPrio     = ceilPrio;
  mux_addr->owner_addr      = NULL; //lock holder thread's prio
  
  for(int8_t a = MAX_PRIO_U8; a>=0; a--){
    create_q(&( mux_addr->waitSet[a] ) );
  }
}

uint8_t OS_Mux_Lock(OS_Mutex *mux_addr)
{
  
  uint8_t locked = 1;  //assume that the mutex will be locked
  OS_TCB *osDumyTCB_addr = (OS_TCB *)(mux_addr->owner_addr);
  
  
  __disable_irq();
    
  if(mux_addr->value == 0U){ //A key free to use

    

    if( osDumyTCB_addr->thread_priority == osCurrTCB_addr->thread_priority){ //A thread which has same priority with owner of mutex  called
    //.. will be re
      while(mux_addr->value == 0U){	
        OS_Sched();
        __enable_irq();
        __disable_irq();
      }

    //..

    }else if(osDumyTCB_addr->thread_priority < osCurrTCB_addr->thread_priority){ //Lower Prio thread has the mutex
      //Prio Ceiling


      //thread'in priority degerini gunceliyoruz

      //queuemuzdaki yerini duzeltiyoruz
      osDumyTCB_addr->status = READY;
      dequeue(&(osReadyq[osDumyTCB_addr->thread_priority]));
      enqueue(&osReadyq[mux_addr->ceilingPrio], (uint32_t *)osDumyTCB_addr);

      //su anki threadimizi wait queueya alıp suspend edelim
      osCurrTCB_addr->status = SUSPENDED;
      enqueue(&(mux_addr->waitSet[osCurrTCB_addr->thread_priority]),(uint32_t *)osCurrTCB_addr); /* add to waiting-set */
      dequeue(&osReadyq[osCurrTCB_addr->thread_priority]);  /* remove from Ready queue*/
      
      while(mux_addr->value == 0U){	
        OS_Sched();
        __enable_irq();
        __disable_irq();
      }
      

        
    }else{ //wait for mutex to be released by its owner
      

      //su anki threadimizi wait queueya alıp suspend edelim
      osCurrTCB_addr->status = SUSPENDED;
      enqueue(&(mux_addr->waitSet[osCurrTCB_addr->thread_priority]), (uint32_t *)osCurrTCB_addr); /* add to waiting-set */
      dequeue(&osReadyq[osCurrTCB_addr->thread_priority]);  /* remove from Ready queue*/

      while(mux_addr->value == 0U){	
        OS_Sched();
        __enable_irq();
        __disable_irq();
      }
      
    }
    
  }

  mux_addr->value = 0U;  //Current Thread has the key
  mux_addr->owner_addr = (uint32_t *)osCurrTCB_addr;  //which task has the key
  
  __enable_irq();
      
  return locked;
    
}

uint8_t OS_Mux_Unlock(OS_Mutex *mux_addr)
{
  uint8_t signaled = 1;

  __disable_irq();
  if(mux_addr->value == 0U){ // if key is not free
    
    mux_addr->value         = 1U;
    mux_addr->owner_addr    = NULL;
  
  	//t�m prio bakacagiz
    int8_t _prio = MAX_PRIO_U8;
    while(_prio+1){	//look each rows

      if(mux_addr->waitSet[_prio].noe == 0){  //bu priorityli queue bos
        //..
        _prio--;//bir sonraki prio gec
      }else{
        
        OS_TCB *osDumyTCB_addr = (OS_TCB *)peek( &(mux_addr->waitSet[_prio]) );
        
        //insert the thread is ready
        osDumyTCB_addr->status = READY;
        dequeue( &(mux_addr->waitSet[_prio]) ); //remove from waitSet
        enqueue( &osReadyq[_prio], (uint32_t *)osDumyTCB_addr);

        //remove ceiled prioritied thread

        osDumyTCB_addr = (OS_TCB *)peek( &osReadyq[mux_addr->ceilingPrio] );
        if(osDumyTCB_addr != NULL){

          osDumyTCB_addr->status = READY;
          dequeue(&osReadyq[mux_addr->ceilingPrio]);  /* remove from Ready queue with ceiled Prio*/
          enqueue(&osReadyq[osDumyTCB_addr->thread_priority], (uint32_t *)osDumyTCB_addr);  //insert it to the its own priority place for Ready Queue
        
        }


        OS_Sched();
        __enable_irq();
        __disable_irq();


        break;

      }


    }

    
  }else{
    
    signaled = 0;
  }
   
  __enable_irq();

  return signaled;


} 

