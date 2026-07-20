#ifndef __MIROS_H__
#define __MIROS_H__

#include <stdint.h>
#include "TM4C129.h"                    // Device header



#define PRIO_APPROACH(x) (32 - __clz(x))



typedef struct {
   uint32_t waitSet;
   uint16_t volatile value; 
   uint16_t max_value; 
} OS_Semaphore;



void OSSemaphore_init(OS_Semaphore * const sem, uint16_t val, uint16_t max_val);
uint8_t OSSemaphore_wait_sleep_down(OS_Semaphore * const sem);
uint8_t OSSemaphore_signal_wakeup_up(OS_Semaphore * const sem);
  

typedef struct{
  uint32_t waitSet;
  uint8_t volatile value;  //up-down counter (always binary)
  //we can obtain the Holder's TCB with its prio
  uint8_t volatile OwnerPrio;  //lock holder thead's prio
  uint8_t ceiling;  //prio ceiling of this mutex

}OS_Mutex;


void OSMutex_init(OS_Mutex * const mux, uint8_t prio_ceil);
uint8_t OSMutex_lock_acquire_pend(OS_Mutex * const mux);
uint8_t OSMutex_unlock_release_post(OS_Mutex * const mux);


uint8_t OS_SSched_unLock(void);
uint8_t OS_SSched_Lock(uint8_t Lock_prio);



//Thread Control Block TCB
typedef struct{
	
	void *StackTop;	//stack pointer
	// ...other attributes associated with a thread
	
	uint32_t timeOut; //timeout delay down-counter
	
	uint8_t	prio;	//thread priority
	
} OSTCB; //QXThread -> OSTCB


typedef void (*OSTHREAD_HANDLER)();

void OS_idle(void);
//callback to handle the idle condition
void OS_onIdle(void);
//blocking delay
//QXThread_Delay -> OS_delay
void OS_delay(uint32_t ticks);
//process all timeout  
void OS_ticks(void);
//QF_init() -> OSINIT
void OSINIT(void);

// this func must be called with interrupts DISABLED
void OSSCHED(void);

void OSTHREAD_CREATE( OSTCB 						*ThreadControlBlock, //QXThread -> OSTCB
											uint8_t						prio,
											OSTHREAD_HANDLER	ThreadHandler,
											void 							*StackPointer,
											uint32_t 					StackSize );

// transfer control to the RTOS to run the threads
void OSRUN(void);
//callback to configure and start interrupts
void OSSTARTUP(void);                  
                      
#endif	//__MIROS_H__
