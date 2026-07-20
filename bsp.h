/****************************************************
 *  bsp.h                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class bsp       
 *  Original author: mustafa                     
 ****************************************************/

#if !defined(__BSP_H__)
#define __BSP_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "TM4C129.h"                    // Device header


#include "tbio.h"
#include "miros.h"

/**
 * Start The Board Support Package and related hardware initialization
 */
void BSP_Init(void);
#define AWARE_ISR_CMSIS_PRI(priority) (__set_BASEPRI(priority << (8U -__NVIC_PRIO_BITS)))


void sendMorseCode(uint32_t bitmask);

#ifdef __cplusplus
}
#endif


#endif /*!defined(__BSP_H__)*/
