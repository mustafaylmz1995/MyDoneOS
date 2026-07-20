/****************************************************
 *  thread.h                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class thread       
 *  Original author: musta                     
 ****************************************************/

#if !defined(__THREAD_H__)
#define __THREAD_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>	//malloc


#include "config_constant.h"

/*
  Thread States

*/
typedef enum {
  NEW,
  RUNNING,
  BLOCKED,
  SUSPENDED,
  READY,
  TERMINATED
}threadState;


/**
 * Thread Control Block
 */
typedef struct
{
		/**
	 * stack pointer
	 */
	void *stack_ptr ;
  void *stack_top_addr;  
  void *stack_bot_addr;	
  
  /**
	 * thread priority
	 */
	uint8_t thread_priority ;
	/**
	 * timeout delay down counter
	 */
	uint32_t delay_timeout ;

  /*
  state of the TCB 
  */
  threadState status;
  
  
}  OS_TCB;



#ifdef __cplusplus
}
#endif


#endif /*!defined(__THREAD_H__)*/
