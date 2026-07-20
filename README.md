# MyDoneOS

**Minimal Real-Time Kernel for ARM Cortex-M4**

[![ARM](https://img.shields.io/badge/Target-ARM_Cortex--M4F-red)](https://developer.arm.com)
[![Language](https://img.shields.io/badge/Language-C-blue)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License](https://img.shields.io/badge/License-MIT-yellow)](LICENSE)

---

## 📖 Overview

MyDoneOS is a minimal real-time operating system kernel for ARM Cortex-M4F microcontrollers. It features:

- ⚡ **O(1) bitmap scheduler** — single `CLZ` instruction task selection
- 🔄 **Preemptive multitasking** — PendSV-based context switching
- 🔐 **Counting semaphore** — with blocking wait set
- 🔒 **Mutex with priority inheritance** — prevents priority inversion
- ⏱️ **Software timer** — SysTick-driven timeout management
- 🧩 **~500 lines of C** — fully auditable, MISRA-C friendly

---

## 📦 Files

| File | Description |
|:-----|:------------|
| `miros.c` | Kernel implementation — scheduler, context switch, sync primitives |
| `miros.h` | Kernel API — TCB, semaphore, mutex declarations |
| `bsp.c` | Board Support Package — hardware initialization |
| `bsp.h` | BSP header |
| `embed_assert.h` | Embedded assertion macros |

---

## 🚀 Quick Start

This kernel targets the **TI TM4C1294** (ARM Cortex-M4F) on the EK-TM4C1294XL LaunchPad.

### Prerequisites

- Keil MDK-ARM v5+ (or any ARM toolchain)
- TI EK-TM4C1294XL LaunchPad (or any TM4C1294 board)

### Building

```bash
# Clone the repository
git clone https://github.com/mustafaylmz1995/MyDoneOS.git
cd MyDoneOS

# Import into Keil MDK and build
# Or use arm-none-eabi-gcc with appropriate linker script
```

### Example: Creating Tasks

```c
#include "miros.h"
#include "bsp.h"

static void task1(void) {
    while (1) {
        BSP_ledRedOn();
        OS_delay(500);
        BSP_ledRedOff();
        OS_delay(500);
    }
}

static void task2(void) {
    while (1) {
        BSP_ledBlueOn();
        OS_delay(1000);
        BSP_ledBlueOff();
        OS_delay(1000);
    }
}

int main(void) {
    static uint32_t stack1[40], stack2[40];
    static OSTCB tcb1, tcb2;
    
    BSP_init();
    OSINIT();
    
    OSTHREAD_CREATE(&tcb1, 1, task1, &stack1[0], sizeof(stack1));
    OSTHREAD_CREATE(&tcb2, 2, task2, &stack2[0], sizeof(stack2));
    
    OSRUN();  // Start scheduler — never returns
    return 0;
}
```

---

## 📊 Kernel Architecture

```
┌─────────────────────────────────────────────────────┐
│                   APPLICATION                        │
│  ┌──────────┐  ┌──────────┐  ┌──────────────────┐  │
│  │  Task 1  │  │  Task 2  │  │  Task N (idle)   │  │
│  └────┬─────┘  └────┬─────┘  └────────┬─────────┘  │
│       │              │                 │             │
├───────┴──────────────┴─────────────────┴────────────┤
│                   KERNEL (miros)                     │
│  ┌─────────────┐  ┌──────────┐  ┌────────────────┐ │
│  │  Scheduler  │  │ PendSV   │  │  Sync Primitives│ │
│  │  O(1) bitmap│  │ Handler  │  │  Sem, Mutex    │ │
│  └─────────────┘  └──────────┘  └────────────────┘ │
├──────────────────────────────────────────────────────┤
│                 HARDWARE (TM4C1294)                   │
│  ┌──────────┐  ┌──────────┐  ┌──────────────────┐  │
│  │ SysTick  │  │   NVIC   │  │  GPIO/Timer/UART  │  │
│  └──────────┘  └──────────┘  └──────────────────┘  │
└──────────────────────────────────────────────────────┘
```

---

## 📈 Performance

| Metric | Value |
|:-------|:------|
| Scheduler | O(1) — single CLZ instruction |
| Context switch | ~50 cycles (~0.4 µs @ 120 MHz) |
| Max tasks | 32 |
| RAM per task | 12 bytes (TCB) + stack |
| Code size | ~500 lines C + 20 ASM |

---

## 🙏 Credits

This project was inspired by:
- **Miro Samek's** embedded systems YouTube series
- **Jonathan Valvano's** book *"Embedded Systems: RTOS for ARM Cortex-M Microcontrollers"*

The synchronization primitives (semaphore and priority inheritance mutex) were designed and tested independently based on these foundational concepts.

---

## 📄 License

MIT License — see [LICENSE](LICENSE).

---

## 📬 Contact

**Mustafa YILMAZ** — Embedded Software Engineer

[![GitHub](https://img.shields.io/badge/GitHub-mustafaylmz1995-181717?style=flat-square&logo=github)](https://github.com/mustafaylmz1995)
