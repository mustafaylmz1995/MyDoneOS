/****************************************************
 *  miros.c                                         
 *  Created on: 27-�ub-2022 17:14:06                      
 *  Implementation of the Class miros       
 *  Original author: mustafa                     
 ****************************************************/

#include "miros.h"
EA_DEFINE_THIS_FILE


Queue_t osReadyq[MAX_PRIO_U8+1];
Queue_t osDelayq[MAX_PRIO_U8+1];


Queue_t threads[MAX_PRIO_U8+1];



/**
 * pointer to the current thread
 * 
 * extern it in the header file
 */
volatile OS_TCB *osCurrTCB_addr ;
/**
 * pointer to the next thread
 * 
 * extern it in the header file
 */
volatile OS_TCB *osNextTCB_addr ;
/**
 * OS Idle definition
 */
OS_TCB idleThreadTCB ;
/**
 * Idle Thread Stack
 */
uint32_t idleThreadStack [100];
/**
 * Stack Pointer for Idle Thread
 */
uint32_t *idleThread_stack_addr  =(uint32_t *)&idleThreadStack[0] ;



/**
 * It creates idle Thread, set priorities for hardware interrupts, set some flags
 * for OS
 */
void OS_Init(void)
{
  
  for(int8_t a = MAX_PRIO_U8; a>=0; a--){
    create_q(&threads[a]);
    create_q(&osReadyq[a]);
    create_q(&osDelayq[a]);
  }
  
  
  	// STKALIGN the stack is 8-byte aligned
	*(uint32_t volatile *)0xE000ED14 |= (1U<<9);
  
	//set the PENDSV interrupt priority to the lowest level
	*(uint32_t volatile *)0xE000ED20	|= (0xFF<<16);
	
	OS_ThreadCreate(	&idleThreadTCB,
                    -1, //idle thread priority 
                    &OS_Idle,
                    idleThreadStack,
                    sizeof(idleThreadStack)
	);
}

/**
 * transfer control to the RTOS to run the threads
 */
void OS_Run(void)
{
	//Callback to configure and start interrupts
	OS_StartUp();
	
	__disable_irq();
	OS_Sched();
	__enable_irq();
	
	//the following code should never execute
	EA_ERROR();
}

/**
 * Create Thread which will work concurrent
 */
void OS_ThreadCreate(OS_TCB *TCB_addr, int8_t Priority, OSTHREAD_HANDLER ThreadHandler, void* Stack_addr, uint32_t Stack_size)
{
	//must be in range and the prio level must be unused
	//EA_REQUIRE( (Priority < EA_DIM(OSTHREADS)) && (OSTHREADS[Priority_u8] == (OSTCB *)0));
	

	
	//Round down the Stack Top to the 8-byte aligned 
	uint32_t *StackTopAddr 		= (uint32_t *)( (((uint32_t)Stack_addr + Stack_size)&(0xFFFFFFF8U)) );
	//Round down the Stack Bottom to the 8-byte aligned 
	uint32_t *StackBottomAddr	= (uint32_t *)( ( ((uint32_t)Stack_addr +8U) &(0xFFFFFFF8U) )	);
  
	uint32_t *stack_ptr;	
	stack_ptr = StackTopAddr;
  
  if( FPU_active ) {
  		//fabricated Cortex-M ISR stack frame for blinky1
	*(--stack_ptr) = FPSCR_reg;							    // FPSCR
	*(--stack_ptr) = 0x00000000U;	              // S15
  *(--stack_ptr) = 0x00000000U;							  // S14		
	*(--stack_ptr) = 0x00000000U;							  // S13
	*(--stack_ptr) = 0x00000000U;							  // S12
	*(--stack_ptr) = 0x00000000U;							  // S11
	*(--stack_ptr) = 0x00000000U;	              // S10 
	*(--stack_ptr) = 0x00000000U;							  // S9 
	*(--stack_ptr) = 0x00000000U;							  // S8
	*(--stack_ptr) = 0x00000000U;	              // S7
  *(--stack_ptr) = 0x00000000U;							  // S6		
	*(--stack_ptr) = 0x00000000U;							  // S5
	*(--stack_ptr) = 0x00000000U;							  // S4
	*(--stack_ptr) = 0x00000000U;							  // S3
	*(--stack_ptr) = 0x00000000U;	              // S2 
	*(--stack_ptr) = 0x00000000U;							  // S1
	*(--stack_ptr) = 0x00000000U;							  // S0  

  }
 
		//fabricated Cortex-M ISR stack frame for blinky1
	*(--stack_ptr) = (1U<<24); 								// xPSR //THUMB instruction state
	*(--stack_ptr) = (uint32_t)ThreadHandler;	// PC		//address of thread return
	*(--stack_ptr) = 0x0000000EU;							// LR		//blinky1 doesnt return not important
	*(--stack_ptr) = 0x0000000CU;							// R12
	*(--stack_ptr) = 0x00000003U;							// R3
	*(--stack_ptr) = 0x00000002U;							// R2
	*(--stack_ptr) = (uint32_t)StackTopAddr;		// R1 R1 can be stk_limit_p
	*(--stack_ptr) = 0x00000001U;							// R0 R0 can be p_arg
	//additionally, fake registers R4-R11
	*(--stack_ptr) = 0x11111111u;							// R11
	*(--stack_ptr) = 0x10101010u;							// R10
	*(--stack_ptr) = 0x09090909u;							// R9
	*(--stack_ptr) = 0x08080808u;							// R8
	*(--stack_ptr) = 0x07070707u;							// R7
	*(--stack_ptr) = 0x06060606u;							// R6
	*(--stack_ptr) = 0x05050505u;							// R5
	*(--stack_ptr) = 0x04040404u;							// R4
  
  
	
	//Save The top of the stack in the thread's attribute
	TCB_addr->stack_ptr       = stack_ptr;
  TCB_addr->stack_bot_addr  = StackBottomAddr;
  TCB_addr->stack_top_addr  = StackTopAddr;
	TCB_addr->thread_priority	= Priority;

	
	
	for(uint32_t *sp = stack_ptr-1U; sp>=StackBottomAddr; --sp){
		*sp = 0xDEADBEEF;
	}
  
	
	if(Priority !=-1){
  
    //thread adreslerini tutan queueya ekliyoruz
    enqueue(&threads[Priority],   (uint32_t *)TCB_addr);
    
    //thread'in state'ini ready yapiyoruz
    TCB_addr->status          = READY;
    //thread'i ready queue ya ekliyoruz
    enqueue(&osReadyq[Priority],  (uint32_t *)TCB_addr);
    
	}
  
 
	
}

/**
 * When all threads on the Block State this thread will start to execute.
 */
void OS_Idle(void)
{
	while(1){
		OS_OnIdle(); //Callback function
	}
}

/**
 * When a Thread calls this function, it start to wait and its state will be
 * Blocked.
 */
void OS_Delay(uint32_t Ticks_u32)
{
	__disable_irq();
	
	//never call OS_Delay from the IdleThread
	//EA_REQUIRE(osCurrTCB_addr != osThreads_arr[0].threads_addr_arr[0]);
	
	osCurrTCB_addr->delay_timeout = Ticks_u32;

  //Running state'de olan bir thread delay ile waiting state'e ge�er
  if(osCurrTCB_addr->status == RUNNING){
    
    osCurrTCB_addr->status = BLOCKED;//State'ini degistiriyoruz
    //ready queue sundan alip delay queue suna ekliyoruz
    dequeue(&osReadyq[osCurrTCB_addr->thread_priority]);
    enqueue(&osDelayq[osCurrTCB_addr->thread_priority], (uint32_t *)osCurrTCB_addr);

  }



	OS_Sched();
	__enable_irq();
}

/**
 * Most Important Function for an OS.
 * It Schedules the threads.
 */
void OS_Sched(void)
{
  LED1_On();
  
	//OS_ready queue muz bossa
	//os_next idle thread olacak	
	osNextTCB_addr = &idleThreadTCB;
	
	//
	int8_t _prio = MAX_PRIO_U8;
	while(_prio+1){	//look each rows
    
		
		if(osReadyq[_prio].noe == 0){  //bu priorityli queue bos
		
			//..
			_prio--;//bir sonraki prio gec
			
		}else{
			OS_TCB *osDumyTCB_addr = (OS_TCB *)peek(&osReadyq[_prio]);  //bu priodeki ilk threadi aliyoruz(peek ediyoruz)
			
			osNextTCB_addr = osDumyTCB_addr;  //nextTCB'miz bu thread oluyor

		
		//ilk durumlari kontrol et
		
			break;
		}
    
	}
	
	//trigger pendsv exception
	if(osNextTCB_addr != osCurrTCB_addr){
    
    osNextTCB_addr->status = RUNNING; //running state'ine getiriyoruz
    if( (osCurrTCB_addr->status == RUNNING) )
      osCurrTCB_addr->status = READY; //CurrTCB yi ready state'ine getiriyoruz
       
		*(uint32_t volatile *)0xE000ED04	|=	(1U<<28);
	}
  LED1_Off();
}


__asm void PendSV_Handler(void)
{
  
  IMPORT 				osCurrTCB_addr  //extern variable
  IMPORT 				osNextTCB_addr	    //extern variable

  //  __disable_irq();
  CPSID         i	


  LDR           r0,=osCurrTCB_addr  ; @0x00000894 0x2000 0010  ==> &osCurrTCB_addr
  LDR           r0,[r0,#0x00]
  //if( osCurrTCB_addr != (OS_TCB *)0 ){ 
  CBZ           r0,PendSV_Restore   ; @0x0000087C ==>PendSV_Restore

  //osCurrTCB_addr->stack_top_addr = (void *)__get_MSP();

  NOP
  PUSH          {r4-r11}              //;r4-r11 arasindaki degerleri stack'e push ediyoruz
  //push edilmis stack_top_addr miz
  MOV           r0,SP
  LDR           r1,=osCurrTCB_addr  ; @0x00000894 0x2000 0010  ==> &osCurrTCB_addr
  LDR           r1,[r1,#0x00]
  STR           r0,[r1,#0x00]

  //}


PendSV_Restore
  LDR           r1, =osNextTCB_addr  ; @0x00000898 0x2000 0014  ==> &osNextTCB_addr
  LDR           r1,[r1,#0x00]
  LDR           r0,[r1,#0x00]


    
  //__set_MSP((uint32_t)osNextTCB_addr->stack_top_addr);
  MOV           SP,r0
  NOP 
  POP           {r4-r11}                  //;r4-r11 arasindaki degerleri stack'ten pop ediyoruz
  //pop edilmis stack_top_addr miz
  LDR           r1, =osNextTCB_addr  ; @0x00000898 0x2000 0014  ==> &osNextTCB_addr
  LDR           r1, [r1, #0x00]
  MOV           r0, SP
  STR           r0, [r1, #0x00]


  //osCurrTCB_addr = osNextTCB_addr;
  LDR           r0,=osNextTCB_addr ; @0x00000898 0x2000 0014  ==> &osNextTCB_addr
  LDR           r0,[r0,#0x00]
  LDR           r1,=osCurrTCB_addr  ; @0x00000894 0x2000 0010  ==> &osCurrTCB_addr
  STR           r0,[r1,#0x00]


  //__enable_irq();
  CPSIE         i
  BX            lr 
}

void OS_Ticks(void)
{

  	//t�m prio bakacagiz
	int8_t _prio = MAX_PRIO_U8;
	while(_prio+1){	//look each rows
    
    
    if(osDelayq[_prio].noe == 0){  //bu priorityli queue bos
    
    //..

    
    }else{
      
			//while dong�s�nde 1 fazla ile kiyaslamak icin
      int8_t _noe     = osDelayq[_prio].noe;
			
			while(_noe){
				OS_TCB *osDumyTCB_addr = (OS_TCB *)peek(&osDelayq[_prio]);
				osDumyTCB_addr->delay_timeout -= 1U; //delay_timeout 1 tane azaltildi

				if(osDumyTCB_addr->delay_timeout == 0){ //delay_timeout 0 oldu
					osDumyTCB_addr->status = READY; //ready queueya al
					dequeue(&osDelayq[_prio]);
					enqueue(&osReadyq[_prio], (uint32_t *)osDumyTCB_addr);
  
				}else{
          
					dequeue(&osDelayq[_prio]);
					enqueue(&osDelayq[_prio], (uint32_t *)osDumyTCB_addr);

				}
		
				_noe--;
			
			}
      
      
    }
    _prio--;
    
    
	}
} 

