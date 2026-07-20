# MyDoneOS

**Minimal Real-Time Kernel for ARM Cortex-M4 — Priority Scheduling, Semaphore, Mutex, Queue**

[![ARM](https://img.shields.io/badge/Target-ARM_Cortex--M4F-red)](https://developer.arm.com)
[![Language](https://img.shields.io/badge/Language-C-blue)](https://en.wikipedia.org/wiki/C_(programming_language))
[![RTOS](https://img.shields.io/badge/RTOS-Minimal-green)]
[![License](https://img.shields.io/badge/License-MIT-yellow)](LICENSE)

---

## 📖 Overview


> ⚠️ **DISCLAIMER**
> This is an **educational project** created for learning RTOS internals. It is **not production-ready software**.
> It may contain bugs, incomplete error handling, and untested edge cases.
> Use at your own risk. The author assumes no liability for any damages or issues arising from its use.

MyDoneOS is a minimal real-time kernel for ARM Cortex-M4F microcontrollers. Built as a deep-dive project into RTOS internals. Features O(1) bitmap scheduling, counting semaphores, mutexes with priority inheritance, and message queues.

```
Timer Interrupt
     │
     ▼
┌────────────┐     ┌──────────────────────┐
│  SysTick   │────▶│   OS_ticks()         │
│  Handler   │     │   decrements timeouts │
└────────────┘     └──────────┬───────────┘
                              │
                              ▼
┌─────────────────────────────────────────────┐
│              OSSCHED()                      │
│  Find highest-prio ready task via CLZ       │
│  If different from current → trigger PendSV │
└────────────────────┬────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────┐
│         PendSV Handler (context switch)      │
│  Save context (R4-R11, LR) of current task   │
│  Restore context of next task                │
│  Return → now executing new task            │
└─────────────────────────────────────────────┘
```

---

## 📦 Files

| File | Description |
|:-----|:------------|
| **Kernel Core** | |
| `miros.c` / `miros.h` | Scheduler, TCB management, context switch, time management |
| **Synchronization** | |
| `OS_Sem.c` / `OS_Sem.h` | Counting semaphore with blocking wait set |
| `OS_Mux.c` / `OS_Mux.h` | Mutex with priority inheritance & ceiling protocol |
| `embed_queue.c` / `embed_queue.h` | Message queue for inter-task communication |
| **Support** | |
| `embed_assert.c` / `embed_assert.h` | Embedded assertion macros |
| `config_constant.c` / `config_constant.h` | System configuration constants
| **Hardware** | |
| `startup_TM4C129.s` | Startup code — vector table, reset handler, interrupt handlers
| `system_TM4C129.c` / `system_TM4C129.h` | System initialization — clock config, peripheral setup |
| **Board Support** | |
| `bsp.c` / `bsp.h` | Hardware init, LED control, GPIO, timers |
| `tbio.c` / `tbio.h` | TM4C1294 Basic Input Output (TBIO) — UART debug I/O |
| **Application** | |
| `main.c` / `main.h` | Example tasks and application entry |
| `thread.c` / `thread.h` | Thread definitions and handlers |

---

## 🚀 Kernel API

### Task Management
```c
void OSINIT(void);                    // Initialize kernel
void OSTHREAD_CREATE(OSTCB *tcb, uint8_t prio, 
                     void (*handler)(void), 
                     void *stack, uint32_t stackSize);
void OSRUN(void);                    // Start scheduler
void OS_delay(uint32_t ticks);       // Blocking delay
```

### Semaphore
```c
void OSSem_init(OS_Semaphore *sem, uint16_t val, uint16_t max);
uint8_t OSSem_wait(OS_Semaphore *sem);    // Block until available
uint8_t OSSem_signal(OS_Semaphore *sem);  // Release
```

### Mutex
```c
void OSMutex_init(OS_Mutex *mux, uint8_t ceiling);
uint8_t OSMutex_lock(OS_Mutex *mux);      // Acquire with inheritance
uint8_t OSMutex_unlock(OS_Mutex *mux);    // Release
```

### Queue
```c
void OSQueue_init(OS_Queue *q, void *buf, uint32_t size, uint32_t itemSize);
uint8_t OSQueue_send(OS_Queue *q, void *item);    // Send message
uint8_t OSQueue_recv(OS_Queue *q, void *item);    // Receive message
```

---

## 🧪 Example: Blinky with Two Tasks

```c
#include "miros.h"
#include "bsp.h"
#include "OS_Sem.h"

static OS_Semaphore sem1;

static void task1(void) {
    while (1) {
        OSSem_wait(&sem1);
        BSP_ledRedOn();
        OS_delay(500);
        BSP_ledRedOff();
    }
}

static void task2(void) {
    while (1) {
        OS_delay(1000);
        OSSem_signal(&sem1);  // Trigger task1
    }
}

int main(void) {
    static uint32_t stack1[40], stack2[40];
    static OSTCB tcb1, tcb2;
    
    BSP_init();
    OSINIT();
    OSSem_init(&sem1, 0, 1);
    
    OSTHREAD_CREATE(&tcb1, 2, task1, &stack1[0], sizeof(stack1));
    OSTHREAD_CREATE(&tcb2, 1, task2, &stack2[0], sizeof(stack2));
    
    OSRUN();  // Never returns
    return 0;
}
```

---

## 📊 Performance

| Metric | Value |
|:-------|:------|
| Scheduler | **O(1)** — single CLZ instruction |
| Context switch | ~50 cycles (~0.4 µs @ 120 MHz) |
| Max tasks | 32 |
| RAM per task | 12 bytes (TCB) + task stack |
| Code size | ~800 lines C + 20 ASM |

---

## 🙏 Credits

This project was completed **3-4 years ago** as a deep-dive into RTOS internals. It was developed over several months of study and hands-on experimentation on a TI Tiva LaunchPad.

Inspired by:
- **Miro Samek's** embedded systems YouTube series
- **Jonathan Valvano's** book *"Embedded Systems: RTOS for ARM Cortex-M Microcontrollers"*

---

## 📄 License

MIT License.

---

## 📬 Contact

**Mustafa YILMAZ** — Embedded Software Engineer

[![GitHub](https://img.shields.io/badge/GitHub-mustafaylmz1995-181717?style=flat-square&logo=github)](https://github.com/mustafaylmz1995)
