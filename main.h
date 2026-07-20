/****************************************************
 *  main.h                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class main       
 *  Original author: musta                     
 ****************************************************/

#if !defined(__MAIN_H__)
#define __MAIN_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h> //uint32_t for type def c99 standart
#include "TM4C129.h"                    // Device header

#include "thread.h"
#include "OS_Sem.h"
#include "OS_Mux.h"
#include "miros.h"
#include "bsp.h"
/*
void main_blinky1(void);
void main_blinky2(void);
void main_blinky3(void);
void main_blinky4(void);
*/

extern OS_Semaphore Sema1 ;



#ifdef __cplusplus
}
#endif


#endif /*!defined(__MAIN_H__)*/
