#ifndef __BSP_H__
#define __BSP_H__

#include <stdint.h>
#include "miros.h"


#define SYS_CLK_HZ 16000000U

/* system clock tick [Hz] */
#define BSP_TICKS_PER_SEC 1000U //1000U means 1 milisecond

#define LED1 (1U<<1)	//PN1
#define LED2 (1U<<0)	//PN0
#define LED3 (1U<<4)	//PF4
#define LED4 (1U<<0)	//PF0

#define SW1  (1U<<0) //PJ0
#define SW2  (1U<<1) //PJ1

#define AWARE_LEVEL     0x0U
#define AWARE_ISR_CMSIS_PRI(priority)     (__set_BASEPRI(priority << (8U - __NVIC_PRIO_BITS)))

void SysTick_Handler(void);

void BSP_init(void);


void BSP_1On(void);
void BSP_1Off(void);
void BSP_1Toggle(void);

void BSP_2On(void);
void BSP_2Off(void);
void BSP_2Toggle(void);

void BSP_3On(void);
void BSP_3Off(void);
void BSP_3Toggle(void);

void BSP_4On(void);
void BSP_4Off(void);
void BSP_4Toggle(void);
void BSP_4Impulse(void);


void BSP2_sendMorseCode(uint32_t bitmask);


#endif // __BSP_H__
