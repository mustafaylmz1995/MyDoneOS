/****************************************************
 *  tbio.h                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class tbio       
 *  Original author: mustafa                     
 ****************************************************/

#if !defined(__TBIO_H__)
#define __TBIO_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "TM4C129.h"                    // Device header

#include "main.h"
#include "OS_Sem.h"

/**
 * TM4C1294 Basic Input Output
 */

/**
 * PN1
 */
#define LED1 (1U<<1)
/**
 * PN0
 */
#define LED2 (1U<<0)
/**
 * PF4
 */
#define LED3 (1U<<4)
/**
 * PF0
 */
#define LED4 (1U<<0)
/**
 * PJ0
 */
#define SW1 (1U<<0)
/**
 * PJ1
 */
#define SW2 (1U<<1)


void LED1_On(void);
void LED2_On(void);
void LED3_On(void);
void LED4_On(void);
void LED1_Off(void);
void LED2_Off(void);
void LED3_Off(void);
void LED4_Off(void);
void LED1_Toggle(void);
void LED2_Toggle(void);
void LED3_Toggle(void);
void LED4_Toggle(void);
void LED1_Impulse(void);
void LED2_Impulse(void);
void LED3_Impulse(void);
void LED4_Impulse(void);




#ifdef __cplusplus
}
#endif


#endif /*!defined(__TBIO_H__)*/
