#include "miros.h"
#include "embed_assert.h"

EA_DEFINE_THIS_FILE


uint32_t const ArraySize_idleThread =  40U;  
uint32_t Stack_idleThread[ArraySize_idleThread];
uint32_t *StackPointer_idleThread = (uint32_t *)&Stack_idleThread[0];

uint32_t const ArraySize_tickThread =  40U;  
uint32_t Stack_tickThread[ArraySize_tickThread];
uint32_t *StackPointer_tickThread = (uint32_t *)&Stack_tickThread[0];



//QXThread -> OSTCB
OSTCB	* volatile OS_CURRENT;	//pointer to the current thread
OSTCB	* volatile OS_NEXT;			//pointer to the next thread to run 
//QXThread -> OSTCB
OSTCB	*OSTHREADS[32+1];	//array of threads started

uint32_t	OS_readySet; 		//'1' when thread is 'Ready', '0' if 'Not Ready'
uint32_t	OS_delayedSet; 	//'1' when thread is 'Delayed', '0' if 'Not Delayed'


//OS Idle definition
OSTCB	idleThread;	//QXThread -> OSTCB
void OS_idle(void){
	while(1){
		OS_onIdle(); //Callback function
	}
}

//QXThread_Delay -> OS_delay
void OS_delay(uint32_t ticks){
	__disable_irq();
	
	//never call OS_delay from the idleThread
	EA_REQUIRE(OS_CURRENT != OSTHREADS[0]);
	
	OS_CURRENT->timeOut = ticks;
	
	uint32_t bit = (1U<<(OS_CURRENT->prio - 1U));
	OS_readySet &=~ bit;
	OS_delayedSet |= bit;

	OSSCHED();
	__enable_irq();
	
	
}


OSTCB	tickThread;	//QXThread -> OSTCB
void OS_ticks(void){

	uint32_t workingSet = OS_delayedSet;
	while(workingSet != 0U){
		OSTCB *t = OSTHREADS[PRIO_APPROACH(workingSet)]; //QXThread -> OSTCB
		EA_ASSERT( (t!=(OSTCB*)0) && (t->timeOut != 0U) ); //QXThread -> OSTCB
	
		uint32_t bit = (1U << ((t->prio) - 1U));
		
		--(t->timeOut);
		if(t->timeOut == 0U){
			OS_readySet		|= bit; //insert the thread is ready
			OS_delayedSet &=~ bit; //remove the delayed set as not delayed
		}

		workingSet &=~ bit; //remove the delayed set as not delayed
	}
	
}


//QF_init() -> OSINIT
void OSINIT(void){
  
  
  
  OS_readySet   = 0x00000000;
  OS_delayedSet = 0x00000000;
  
	//set the PENDSV interrupt priority to the lowest level
	*(uint32_t volatile *)0xE000ED20	|= (0xFF<<16);
	
	OSTHREAD_CREATE(	&idleThread,
										0U, //idle thread priority 
										&OS_idle,
										StackPointer_idleThread,
										sizeof(Stack_idleThread)
	);
  

	
	
}


void OSSCHED(void){
	// OS_NEXT = ...
	if(OS_readySet == 0U){ // is it on idle 
		OS_NEXT = OSTHREADS[0]; //the idle thread 
	}else{
 		OS_NEXT = OSTHREADS[PRIO_APPROACH(OS_readySet)];
		EA_ASSERT(OS_NEXT != (OSTCB *)0);	//QXThread -> OSTCB
	}
	
	//trigger pendsv exception
	if(OS_NEXT != OS_CURRENT){
		*(uint32_t volatile *)0xE000ED04	=	(1U<<28);
	}
  BSP_4Impulse();
	
}

void OSTHREAD_CREATE( OSTCB 						*ThreadControlBlock, //QXThread -> OSTCB
											uint8_t						prio,
											OSTHREAD_HANDLER	ThreadHandler,
											void 							*StackPointer,
											uint32_t 					StackSize )
											
{
	
	//must be in range and the prio level must be unused
	EA_REQUIRE( (prio < EA_DIM(OSTHREADS))
						&& (OSTHREADS[prio] == (OSTCB *)0) //QXThread -> OSTCB
	);
	

  uint32_t        *p_stk_limit;


	//Round down the Stack Top to the 8-byte boundary 
	uint32_t *StackTop 		= (uint32_t *)( (((uint32_t)StackPointer + StackSize)>>3U)<<3U   );
	//Round down the Stack Bottom to the 8-byte boundary 
	uint32_t *StackBottom	= (uint32_t *)( ((((uint32_t)StackPointer -	1U)>>3U)+1U	)<<3U 	);
	
  p_stk_limit = StackTop;
	
		//fabricated Cortex-M ISR stack frame for blinky1
	*(--StackTop) = (1U<<24); 								// xPSR //THUMB instruction state
	*(--StackTop) = (uint32_t)ThreadHandler;	// PC		//address of thread return
	*(--StackTop) = 0x0000000EU;							// LR		//blinky1 doesnt return not important
	*(--StackTop) = 0x0000000CU;							// R12
	*(--StackTop) = 0x00000003U;							// R3
	*(--StackTop) = 0x00000002U;							// R2
	*(--StackTop) = (uint32_t)p_stk_limit;		// R1 R1 can be p_stk_limit
	*(--StackTop) = 0x00000000U;							// R0 R0 can be p_arg
	//additionally, fake registers R4-R11
	*(--StackTop) = 0x11111111u;							// R11
	*(--StackTop) = 0x10101010u;							// R10
	*(--StackTop) = 0x09090909u;							// R9
	*(--StackTop) = 0x08080808u;							// R8
	*(--StackTop) = 0x07070707u;							// R7
	*(--StackTop) = 0x06060606u;							// R6
	*(--StackTop) = 0x05050505u;							// R5
	*(--StackTop) = 0x04040404u;							// R4
	
	//Save The top of the stack in the thread's attribute
	ThreadControlBlock->StackTop = StackTop;
	
	
	for(uint32_t *sp = StackTop-1U; sp>=StackBottom; --sp){
		*sp = 0xDEADBEEF;
	}
	

	//register the thread with the OS
	OSTHREADS[prio] = ThreadControlBlock;
	ThreadControlBlock->prio = prio;
	
	//make the thread ready to run 
	if(prio > 0U){
		OS_readySet |= (1U<<(prio - 1U));
	}
  

}


void OSRUN(void){
	//Callback to configure and start interrupts
	OSSTARTUP();
	
	__disable_irq();
	OSSCHED();
	__enable_irq();
	
	//the following code should never execute
	EA_ERROR();
	
	
}



__asm void PendSV_Handler(void){	
	
    IMPORT 				OS_CURRENT  //extern variable
    IMPORT 				OS_NEXT	    //extern variable
//  __disable_irq();
    CPSID         i	
//  if(OS_CURRENT != (OSTCB *)0){			//QXThread -> OSTCB
    LDR           r1,=OS_CURRENT    //;OS_CURRENT= @0x00000670
		LDR           r1,[r1,#0x00]
		CBZ           r1,PendSV_Restore	//;@0x0000065E
//    	push registers r4-r11 on the stack
//    	OS_CURRENT->StackTop = fakeStackTop;
				PUSH          {r4-r11}              //;r4-r11 arasindaki degerleri stack'e push ediyoruz	
				LDR           r1,=OS_CURRENT  	    //; @0x00000670
				LDR           r1,[r1,#0x00]
				STR           sp,[r1,#0x00]			//;(STR           r0,[r1,#0x00]) compiler r0 kullandi biz gercek degeri atayacagiz
//  } "	
 
PendSV_Restore
//  fakeStackTop = OS_NEXT->StackTop; 
		LDR           r1,=OS_NEXT              //; OS_NEXT=	@0x00000674
		LDR           r1,[r1,#0x00]
		LDR           sp,[r1,#0x00]

//  OS_CURRENT = OS_NEXT;
		LDR           r1,=OS_NEXT  				//; OS_NEXT=	@0x00000674
		LDR           r1,[r1,#0x00]
		LDR           r2,=OS_CURRENT            //; OS_CURRENT= @0x00000670
		STR           r1,[r2,#0x00]
//  pop registers r4-r11
    POP           {r4-r11}                  //;r4-r11 arasindaki degerleri stack'e push ediyoruz	
//	__enable_irq();
		CPSIE         i
		BX            lr
	
}



/* OS SEMAPHORE */
void OSSemaphore_init(OS_Semaphore* sem, uint16_t val, uint16_t max_val){
    
  EA_REQUIRE_ID(100, max_val > 0U);

  sem->value     = (uint16_t)val; //current value
  sem->max_value = (uint16_t)max_val; //max value it can take
  sem->waitSet   = 0U;  //waiting 
   
 }
  
uint8_t OSSemaphore_wait_sleep_down(OS_Semaphore* sem){
	
  uint8_t signaled = 1;

  
  EA_REQUIRE_ID(200, (sem->max_value > 0U) /* sema must be initialized */
   && (OS_CURRENT != (OSTCB *)0)); /* curr must be extended */
	 
  __disable_irq();
	

	if( sem->value > 0 ){
		//Semaphore is free
	
		--(sem->value);
		
	}else{
		//Semaphore is locked
		
		//We add WaitSet who want to semaphore 
		uint32_t bit = (1U << ((OS_CURRENT->prio) - 1U));
		sem->waitSet |= bit; /* add to waiting-set */
		OS_readySet  &= ~bit; /* remove from ready-set */


		while(sem->value == 0U){
						
			OSSCHED();
			__enable_irq();
			__disable_irq();
		}
		
		sem->waitSet &=~ bit; //remove the waitSet
		--(sem->value);
		
	}
	
	__enable_irq();
			
  return signaled;

}


 
uint8_t OSSemaphore_signal_wakeup_up(OS_Semaphore* sem){

  uint8_t signaled = 1;

	 
  EA_REQUIRE_ID(200, (sem->max_value > 0U) /* sema must be initialized */
   && (OS_CURRENT != (OSTCB *)0)); /* curr must be extended */
  
	__disable_irq();
	
		++(sem->value);
		
		//highest prio thread on waitSet
		uint8_t prioforWaitSet = PRIO_APPROACH(sem->waitSet);
		if(prioforWaitSet != 0U){	//There should be an Set value for wait
			
			uint32_t bit = (1U << (( (OSTHREADS[prioforWaitSet])->prio) - 1U));
			OS_readySet		|= bit; //insert the thread is ready
      
      OSSCHED();
      __enable_irq();
			__disable_irq();
		}
	
	
	__enable_irq();
	
  return signaled;
   
  
}
/* OS SEMAPHORE */

/* OS MUTEX */
void OSMutex_init(OS_Mutex * const mux, uint8_t prio_ceil){

  EA_REQUIRE_ID(100, ((prio_ceil < 32U) && (prio_ceil != 0U)));
  
  mux->waitSet    = 0U;
  mux->value      = 1U;  //up-down counter (always binary)
  
  //we can obtain the Holder's TCB with its prio
  mux->OwnerPrio  = 0U;  //lock holder thread's prio
  mux->ceiling    = prio_ceil;

 
}


uint8_t OSMutex_lock_acquire_pend(OS_Mutex * const mux){

  uint8_t locked = 1;  //assume that the mutex will be locked
  OSTCB *curr = OS_CURRENT;
  
  EA_REQUIRE_ID(200, (curr != (OSTCB *)0) /* current thread must be extended */
      && ((mux->ceiling == 0U)
      || (curr->prio < mux->ceiling)) /* below ceiling */
      && (mux->ceiling <= 32U) /* in range */     
  ); 
  
  __disable_irq();
    
    if(mux->value == 1U){ //A key free to use

      mux->value = 0U;  //Current Thread has the key
      mux->OwnerPrio = curr->prio;  //which task has the key

      
    }else if(mux->OwnerPrio == curr->prio){ //Same thread calls again
      
    
    }else if(mux->OwnerPrio < curr->prio){ //Lower Prio thread has the mutex
      //Prio Ceiling
      OSTHREADS[mux->ceiling] = OSTHREADS[mux->OwnerPrio];
      
      
      uint32_t bit = (1U << ((mux->ceiling) - 1U));
			OS_readySet		|= bit; //insert the owner thread is ready with ceiling prio

      OS_delayedSet |= ((OS_delayedSet&(1U<<(mux->OwnerPrio-1U)))>>(mux->OwnerPrio -1U))<<(mux->ceiling-1U);
      
      bit = (1U << ((curr->prio) - 1U));
      mux->waitSet  |= bit; //set current thread as wait list
      
      while(mux->value == 0U){	
        OSSCHED();
        __enable_irq();
        __disable_irq();
      }
      
            

      
      mux->value = 0U;  //Current Thread has the key
      mux->OwnerPrio = curr->prio;  //which task has the key
      
        
    }else{ //wait for mutex to be released by its owner
      
      /* remove this curr prio from the ready set (will block)
      * and insert to the waiting set on this semaphore */
      uint32_t bit = (1U << ((curr->prio) - 1U));
      mux->waitSet |= bit; /* add to waiting-set */
      OS_readySet  &= ~bit; /* remove from ready-set */

      while(mux->value == 0U){	
        OSSCHED();
        __enable_irq();
        __disable_irq();
      }
      
      mux->value = 0U;  //Current Thread has the key
      mux->OwnerPrio = curr->prio;  //which task has the key
   
    }
    
    __enable_irq();
        
    return locked;
  
}


uint8_t OSMutex_unlock_release_post(OS_Mutex * const mux){
  uint8_t signaled = 1;

  OSTCB *curr = OS_CURRENT;
  OSTCB	*muxowner;
   
  EA_REQUIRE_ID(200, (curr != (OSTCB *)0)); /* curr must be extended */

  __disable_irq();
  if(mux->value == 0U){ // if key is not free
    
    mux->value      = 1U;
    mux->OwnerPrio  = 0U;
    
    uint8_t prioforWaitSet = PRIO_APPROACH(mux->waitSet);
    if(prioforWaitSet != 0U){	//There should be an Set value for wait
      
      muxowner = OSTHREADS[prioforWaitSet];
      uint32_t bit = (1U << ((muxowner->prio) - 1U));
      OS_readySet		|= bit; //insert the thread is ready
      mux->waitSet &=~ bit; //remove the waitSet
    
      
      OSTHREADS[mux->ceiling] = 0x00;
      bit = (1U << ((mux->ceiling) - 1U));
			OS_readySet		&=~ bit; //insert the thread is ready
      OS_delayedSet &=~ bit;
      
      
      OSSCHED();
      __enable_irq();
      __disable_irq();

      
    }

    
  }else{
    
    signaled = 0;
  }
   
  __enable_irq();

  return signaled;


}
/* OS MUTEX */



/* OS Selective Scheduler Locking*/
// max 32 
// return Locking State
uint32_t OS_lockPrio = 0;
uint8_t OS_Lockstat = 0;

uint8_t OS_SSched_Lock(uint8_t Lock_prio){ 
  
  
  __disable_irq();
  
  if( OS_Lockstat == 0U ){
    
    OS_lockPrio = OS_readySet;
    uint32_t temp = 0xFFFFFFFF;
    //temp = temp << (Lock_prio-1);
    temp = temp << (Lock_prio);
    
    OS_readySet = temp & OS_readySet;

    OS_Lockstat = 1;
    
  }else{

    OS_Lockstat = 0;

  }    
  

  __enable_irq();
  
  return OS_Lockstat;
  
}


uint8_t OS_SSched_unLock(void){
  
  uint8_t ret;
  
  __disable_irq();
  
  if(OS_Lockstat == 0){
    ret = 0;
    
    
  }else{
    
    OS_readySet = OS_lockPrio;
    OS_lockPrio = 0;
    OS_Lockstat = 0;
    ret = 1;
  }
  
  
  __enable_irq();
  return ret;
  
}

/* OS Selective Scheduler Locking*/


