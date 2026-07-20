/* Board Support Package */
#include "TM4C129.h"                    // Device header
#include "system_TM4C129.h" //assert_failed
#include "miros.h"
#include "bsp.h"


extern OS_Semaphore Sema1;
//Sema2 for prio inversion
//static OS_Semaphore Sema2;


void SysTick_Handler(void){
  OS_ticks();	  
	__disable_irq();	

	OSSCHED();
  
	__enable_irq();
}



void GPIOJ_Handler(void){
  
  uint8_t volatile readback;
  __disable_irq();	
  
  if( (GPIOJ_AHB->RIS  & SW1) != 0U){ // IRQ occurs for SW1
    OSSemaphore_signal_wakeup_up(&Sema1);
  }
  
  GPIOJ_AHB->ICR |= (SW1  |  SW2); //clear any prior interrupt
	readback = GPIOJ_AHB->ICR;
  
  __enable_irq();
  
}




void BSP_init(void){
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
	
  //Sema2 for prio inversion
  //OSSemaphore_init(&Sema2,1U,1U); //binary semaphore
	
}


void BSP_1On(void) {
    GPION->DATA |= LED1;
}
void BSP_1Off(void) {
    GPION->DATA &= ~LED1;
}
void BSP_1Toggle(void){
    __disable_irq();
		GPION->DATA ^= LED1;
    __enable_irq();
}


void BSP_2On(void) {
    GPION->DATA |= LED2;
}
void BSP_2Off(void) {
    GPION->DATA &= ~LED2;
}
void BSP_2Toggle(void){
    __disable_irq();
		GPION->DATA ^= LED2;
    __enable_irq();
}


void BSP_3On(void) {
    GPIOF_AHB->DATA |= LED3;
}
void BSP_3Off(void) {
    GPIOF_AHB->DATA &= ~LED3;
}
void BSP_3Toggle(void){
		GPIOF_AHB->DATA ^= LED3;
}


void BSP_4On(void) {
    GPIOF_AHB->DATA |= LED4;
}
void BSP_4Off(void) {
    GPIOF_AHB->DATA &= ~LED4;
}
void BSP_4Toggle(void){
		GPIOF_AHB->DATA ^= LED4;
}
void BSP_4Impulse(void){
    GPIOF_AHB->DATA &= ~LED4;
    GPIOF_AHB->DATA |= LED4;
    GPIOF_AHB->DATA &= ~LED4;
}



void BSP2_sendMorseCode(uint32_t bitmask){
  uint32_t  volatile delay_ctr;
  
  enum  { DOT_DELAY = 75};
  
  //Sema2 for prio inversion
  //OSSemaphore_wait_sleep_down(&Sema2);
  
  OS_SSched_Lock(5U);
  
  for( ; bitmask != 0U; bitmask<<=1U){
    if( (bitmask & (1U << 31)) != 0U){
      BSP_2On();
    }else{
      BSP_2Off();
    }
    
    for(delay_ctr = DOT_DELAY; delay_ctr!= 0U; --delay_ctr){};
  }
  
  BSP_2Off();
  
  for(delay_ctr = 7*DOT_DELAY; delay_ctr != 0U; --delay_ctr){};
  //Sema2 for prio inversion  
  //OSSemaphore_signal_wakeup_up(&Sema2);
  OS_SSched_unLock();
    
    
}




void OSSTARTUP(void){
	
	SystemCoreClockUpdate();
	SysTick_Config(SYS_CLK_HZ/BSP_TICKS_PER_SEC);
	
	//set the SysTick	IRQ priorty
  //set the interrupt priorities of "kernel aware" interrupt
  AWARE_ISR_CMSIS_PRI(AWARE_LEVEL);
	NVIC_SetPriority(SysTick_IRQn, AWARE_LEVEL);
  
  NVIC_SetPriority(GPIOJ_IRQn, AWARE_LEVEL + 1); //For cortex m bigger prio number kernel aware
	
  NVIC_EnableIRQ(GPIOJ_IRQn); //Enable the interrupt for GPIOJ
  
}


void OS_onIdle(void){
	//wait for interrupt low power sleep mode
	//__WFI();
  BSP_3Toggle();

}


void EA_onAssert(char const *module, int loc) {
    /* TBD: damage control */
    (void)module; /* avoid the "unused parameter" compiler warning */
    (void)loc;    /* avoid the "unused parameter" compiler warning */
    NVIC_SystemReset();
}




