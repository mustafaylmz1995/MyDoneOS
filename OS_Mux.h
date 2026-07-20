/****************************************************
 *  OS_Mux.h                                         
 *  Created on: 27-�ub-2022 17:14:06                      
 *  Implementation of the Class OS_Mux       
 *  Original author: mustafa                     
 ****************************************************/

#if !defined(__OS_MUX_H__)
#define __OS_MUX_H__


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
	 * A kind of Priority inversion case this value will be the ceil priority for who
	 * caused the problem
	 */
	uint8_t ceilingPrio ;
	/**
	 * mutex ownership
	 */
	uint32_t *owner_addr ;
	/**
	 * Mutex Current Value
	 */
	volatile uint8_t value ;
	/**
	 * Any Thread that waits for mutex unlock
	 */
	Queue_t waitSet[MAX_PRIO_U8+1] ;
}  OS_Mutex;

/**
 * Set the values to the given Mutex Struct data type
 */
void OS_Mux_Init(OS_Mutex *mux_addr, uint8_t ceilPrio);
uint8_t OS_Mux_Lock(OS_Mutex *mux_addr);
uint8_t OS_Mux_Unlock(OS_Mutex *mux_addr);


#ifdef __cplusplus
}
#endif


#endif /*!defined(__OS_MUX_H__)*/
