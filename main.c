/****************************************************
 *  main.c                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class main       
 *  Original author: musta                     
 ****************************************************/

#include "main.h"

uint32_t blinky1_a [100];
uint32_t blinky2_a [100];
uint32_t blinky3_a [100];
uint32_t blinky4_a [100];
OS_TCB blinky1_st ;
OS_TCB blinky2_st ;
OS_TCB blinky3_st ;
OS_TCB blinky4_st ;
OS_Semaphore Sema1 ;
OS_Mutex Mux1;



void main_blinky1(void)
{
	while(1){
		uint32_t volatile i;
    OS_Sem_Wait(&Sema1);
		OS_Mux_Lock(&Mux1);
    for(i = 3*1500U; i!=0U; --i){ 
      LED3_Toggle();
		}
    OS_Mux_Unlock(&Mux1);
	}
}

void main_blinky2(void)
{
	while(1){
    OS_Mux_Lock(&Mux1);
    sendMorseCode(0xA8EEE2A0U); //SOS,
    OS_Mux_Unlock(&Mux1);
    OS_Delay(10U);
    
	}
}

void main_blinky3(void)
{
	while(1){
    OS_Mux_Lock(&Mux1);
    sendMorseCode(0xEE22A300U); //TEST
    sendMorseCode(0xEE22A300U); //TEST
    OS_Mux_Unlock(&Mux1);
    OS_Delay(50U);
	}
}

void main_blinky4(void)
{
	while(1){
    uint32_t volatile i;
    for(i= 3*1500U; i!=0U; --i){
      LED4_On();
			LED4_Off();
		}

		OS_Delay(50U); //block for 50 tickS
	}
} 


int main(void)
{

	OS_Init();
	BSP_Init();
  
  OS_Sem_Init(&Sema1,0U,1U);//binary semaphore
	OS_Mux_Init(&Mux1, 7U);
  
	OS_ThreadCreate( &blinky1_st, 2U, &main_blinky1, blinky1_a, sizeof(blinky1_a) );
	OS_ThreadCreate( &blinky2_st, 5U, &main_blinky2, blinky2_a, sizeof(blinky2_a) );
	OS_ThreadCreate( &blinky3_st, 1U, &main_blinky3, blinky3_a, sizeof(blinky3_a) );
//	OS_ThreadCreate( &blinky4_st, 0U, &main_blinky4, blinky4_a, sizeof(blinky4_a) );
	
	
	//transfer control to the RTOS to run the threads
	OS_Run();

	
}

