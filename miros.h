/****************************************************
 *  miros.h                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class miros       
 *  Original author: mustafa                     
 ****************************************************/

#if !defined(__MIROS_H__)
#define __MIROS_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "thread.h"
#include "embed_assert.h"
#include "embed_queue.h"
#include "OS_Sem.h"
#include "OS_Mux.h"

#include "TM4C129.h"                    // Device header

/**
 * Micro Operating System Kernel
 */

#define FPU_active (__get_CONTROL() & 0x4)>>2 
#define FPSCR_reg   __get_FPSCR()





extern Queue_t osReadyq[MAX_PRIO_U8+1];

/**
 * pointer to the current thread
 * 
 * extern it in the header file
 */
extern volatile OS_TCB *osCurrTCB_addr ;
/**
 * pointer to the next thread
 * 
 * extern it in the header file
 */
extern volatile OS_TCB *osNextTCB_addr ;


typedef void (*OSTHREAD_HANDLER)();
/**
 * It creates idle Thread, set priorities for hardware interrupts, set some flags
 * for OS
 */
void OS_Init(void);
/**
 * transfer control to the RTOS to run the threads
 */
void OS_Run(void);
/**
 * callback to configure and start interrupts
 * 
 * 
 * created on BSP
 */
void OS_StartUp(void);
/**
 * Create Thread which will work concurrent
 */
void OS_ThreadCreate(OS_TCB *TCB_addr, int8_t Priority, OSTHREAD_HANDLER ThreadHandler, void* Stack_addr, uint32_t Stack_size);
/**
 * When all threads on the Block State this thread will start to execute.
 */
void OS_Idle(void);
/**
 * Callback function for Idle Thread
 * 
 * Created on BSP
 */
void OS_OnIdle(void);
/**
 * When a Thread calls this function, it start to wait and its state will be
 * Blocked.
 */
void OS_Delay(uint32_t Ticks_u32);
/**
 * Most Important Function for an OS.
 * It Schedules the threads.
 */
void OS_Sched(void);
void OS_Ticks(void);


#ifdef __cplusplus
}
#endif


#endif /*!defined(__MIROS_H__)*/
