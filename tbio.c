/****************************************************
 *  tbio.c                                         
 *  Created on: 27-Þub-2022 17:14:06                      
 *  Implementation of the Class tbio       
 *  Original author: mustafa                     
 ****************************************************/

#include "tbio.h"

void LED1_On(void)
{
	GPION->DATA |= LED1;
}

void LED2_On(void)
{
	GPION->DATA |= LED2;
}

void LED3_On(void)
{
	GPIOF_AHB->DATA |= LED3;
}

void LED4_On(void)
{
	GPIOF_AHB->DATA |= LED4;
}

void LED1_Off(void)
{
	GPION->DATA &= ~LED1;
}

void LED2_Off(void)
{
	GPION->DATA &= ~LED2;
}

void LED3_Off(void)
{
	GPIOF_AHB->DATA &= ~LED3;
}

void LED4_Off(void)
{
	GPIOF_AHB->DATA &= ~LED4;
}

void LED1_Toggle(void)
{
	GPION->DATA ^= LED1;
}

void LED2_Toggle(void)
{
	GPION->DATA ^= LED2;
}

void LED3_Toggle(void)
{
	GPIOF_AHB->DATA ^= LED3;
}

void LED4_Toggle(void)
{
	GPIOF_AHB->DATA ^= LED4;
}

void LED1_Impulse(void)
{
	LED1_On();
	LED1_Off();
	LED1_On();
}

void LED2_Impulse(void)
{
	LED2_On();
	LED2_Off();
	LED2_On();
}

void LED3_Impulse(void)
{
	LED3_On();
	LED3_Off();
	LED3_On();
}

void LED4_Impulse(void)
{
	LED4_On();
	LED4_Off();
	LED4_On();
}

void GPIOJ_Handler(void)
{
	uint8_t volatile readback;
	__disable_irq();	
	
		if( (GPIOJ_AHB->RIS  & SW1) != 0U){ // IRQ occurs for SW1
			OS_Sem_Sig(&Sema1);
		}
	
		GPIOJ_AHB->ICR |= (SW1  |  SW2); //clear any prior interrupt
		readback = GPIOJ_AHB->ICR;
	
	__enable_irq();
} 


