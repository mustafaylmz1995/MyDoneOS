/****************************************************
 *  bsp.c                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class bsp       
 *  Original author: mustafa                     
 ****************************************************/

#include "bsp.h"

/**
 * System Clock Frequency
 */
#define SYS_CLK_HZ 16000000U
/**
 * Clock Ticks Per Second
 * 1000U means 1 milisecond
 */
#define BSP_TICKS_PER_SEC 1000U
/**
 * Kernel Priority Aware Level
 */
#define AWARE_LEVEL 0x0

//OS_Semaphore MorseSema;
OS_Mutex  MorseMutex;


/**
 * When an Embedded Assert occurs this function resets the system. (callbakc func.
 * definition on embed_assert.h)
 */
void EA_OnAssert(char const *module_str, int loc_s32)
{
	/* TBD: damage control */
	(void)module_str; /* avoid the "unused parameter" compiler warning */
	(void)loc_s32;    /* avoid the "unused parameter" compiler warning */
	NVIC_SystemReset();
}

/**
 * Callback function for Idle Thread
 * 
 */

void OS_OnIdle(void)
{
  LED2_Toggle();
	//wait for interrupt low power sleep mode
	//__WFI();
}

/**
 * callback to configure and start interrupts
 */
void OS_StartUp(void)
{
	SystemCoreClockUpdate();
	SysTick_Config(SYS_CLK_HZ/BSP_TICKS_PER_SEC);
	
	//set the SysTick	IRQ priorty
	//set the interrupt priorities of "kernel aware" interrupt
	AWARE_ISR_CMSIS_PRI(AWARE_LEVEL);
	NVIC_SetPriority(SysTick_IRQn, AWARE_LEVEL);
	
	NVIC_SetPriority(GPIOJ_IRQn, AWARE_LEVEL + 1); //For cortex m bigger prio number kernel aware
	
	NVIC_EnableIRQ(GPIOJ_IRQn); //Enable the interrupt for GPIOJ
}

/**
 * When a SysTick Interrupt happened, this function will start to work.
 * Generally, It is useful for counting ticks for delay and control scheduling
 * process.
 */
void SysTick_Handler(void)
{
	OS_Ticks();	  
	__disable_irq();	
	
	OS_Sched();
	__enable_irq();
}

/**
 * Start The Board Support Package and related hardware initialization
 */
void BSP_Init(void)
{
	//GPIO Initialization
	
	SYSCTL->RCGCGPIO |= (1U << 12); //GPION Clock Gating openning adress giving port
	SYSCTL->RCGCGPIO |= (1U << 5 ); //GPIOF Clock Gating openning adress giving port
	SYSCTL->RCGCGPIO |= (1U << 8 ); //GPIOJ Clock Gating openning adress giving port
	
	GPION->DIR          |= (LED1 | LED2); //GPIO direction to out
	GPIOF_AHB->DIR      |= (LED3 | LED4); //GPIO direction to out
	
	GPION->DEN          |= (LED1 | LED2); //GPIO digital enable
	GPIOF_AHB->DEN      |= (LED3 | LED4); //GPIO digital enable
	
	GPIOJ_AHB->DIR      &=~ (SW1  |  SW2); //GPIO direction to input
	GPIOJ_AHB->DEN      |=  (SW1  |  SW2); //GPIO digital enable
	GPIOJ_AHB->PUR      |=  (SW1  |  SW2); //GPIO pull-up resistor enable
	
	GPIOJ_AHB->IM       &=~ (SW1  |  SW2); //Mask the corresponding port by clearing the IME field in the GPIOIM register. Close interrupt
	GPIOJ_AHB->IS       &=~ (SW1  |  SW2); //Make SW1/SW2 edge sensitive(0 edge, 1 level sens)
	GPIOJ_AHB->IBE      &=~ (SW1  |  SW2); //Interrupt Both Edges (0 means IEV will make the decision)
	GPIOJ_AHB->IEV      &=~ (SW1  |  SW2); //Clearing is falling edge detection/low level (0 falling edge, 1 rising edge)
	GPIOJ_AHB->ICR      |=  (SW1  |  SW2); //Clear any prior interrupt
	GPIOJ_AHB->IM       |=  (SW1  |  SW2); //Unmask interrupt. Open Interrupt 
	
	
	//GPIO Initialization
  
  
//  OS_Sem_Init(&MorseSema,1U,1U);//binary semaphore
OS_Mux_Init(&MorseMutex, 6U);//priority ceiling

} 




void sendMorseCode(uint32_t bitmask){
  
  uint32_t volatile delayCnt;
  
  enum {DOT_DELAY = 67};  //to make it 2ms period
  
//  OS_Sem_Wait(&MorseSema);,
  OS_Mux_Lock(&MorseMutex);
  
  for(; bitmask != 0U; bitmask<<=1){
    
    if( (bitmask & (1U << 31)) != 0U ){
      LED4_On();
    }else{
      LED4_Off();
    }
    
    for(delayCnt = DOT_DELAY; delayCnt != 0U; --delayCnt){
      //..
    }
  
  }
  LED4_Off();
  
  for(delayCnt = 7*DOT_DELAY; delayCnt != 0U; --delayCnt){
    //..
  }  
  
//  OS_Sem_Sig(&MorseSema);
  OS_Mux_Unlock(&MorseMutex);
  
}




