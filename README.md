<div align="center">

<br/>

```
 █████╗ ███╗   ███╗ ██████╗ ███████╗
██╔══██╗████╗ ████║██╔═══██╗██╔════╝
███████║██╔████╔██║██║   ██║███████╗
██╔══██║██║╚██╔╝██║██║   ██║╚════██║
██║  ██║██║ ╚═╝ ██║╚██████╔╝███████║
╚═╝  ╚═╝╚═╝     ╚═╝ ╚═════╝ ╚══════╝
```

### **Academic Multi-programming Operating System Simulator**
#### Phase 2 — Direct Memory Execution · SI Interrupt Handling · Multi-Job Processing

<br/>

![Language](https://img.shields.io/badge/Language-C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![Compiler](https://img.shields.io/badge/Compiler-GCC%20%2F%20MinGW-A42E2B?style=for-the-badge&logo=gnu&logoColor=white)
![Status](https://img.shields.io/badge/Status-Active-brightgreen?style=for-the-badge)
![Phase](https://img.shields.io/badge/Phase-2-blueviolet?style=for-the-badge)

<br/>

</div>

---

## 📖 Overview

**AMOS** (Academic Multi-programming Operating System) is a simulated operating system built in C that models core OS concepts including instruction fetch-decode-execute cycles, interrupt handling, memory management, and multi-job batch processing.

Phase 2 simplifies the architecture to **direct RAM addressing** — programs are loaded sequentially into memory starting at address `0`, with no paging or virtual memory layer. The OS handles **Supervisor Interrupts (SI)** for I/O and termination, and processes multiple jobs from a single jobcard file in sequence.

---

## ✨ Features

| Feature | Description |
|---|---|
| 🧠 **Fetch-Decode-Execute** | Full instruction cycle simulation with IR, IC, and register R |
| 📦 **Direct RAM Loading** | Programs loaded word-by-word into RAM from address 0, no paging |
| 📋 **Batch Job Processing** | Reads and executes multiple `$AMJ...$END` job blocks sequentially |
| 🔔 **SI Interrupt Handling** | Supervisor interrupts for Read (`GD`), Write (`PD`), and Halt (`H`) |
| 🗂️ **File-based I/O** | Reads job input from `jobcard.txt`, writes output to `output.txt` |
| 🧹 **Memory Lifecycle** | RAM is cleared and CPU state is fully reset between jobs |
| 🖨️ **RAM Dump Utility** | Visual frame-by-frame memory dump for debugging |

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────┐
│                        AMOS Phase 2                     │
│                                                         │
│  ┌──────────┐    ┌──────────┐    ┌────────────────────┐ │
│  │  main.c  │───▶│  mos.c   │───▶│    jobcard.txt     │ │
│  │          │    │          │    │  $AMJ...$DTA...$END │ │
│  └──────────┘    └────┬─────┘    └────────────────────┘ │
│                       │                                  │
│              ┌────────▼────────┐                         │
│              │     boot()      │                         │
│              │  SI Dispatcher  │                         │
│              └────────┬────────┘                         │
│                       │                                  │
│          ┌────────────▼────────────┐                     │
│          │         cpu.c           │                     │
│          │  fetch → decode → run   │                     │
│          └────────────┬────────────┘                     │
│                       │                                  │
│          ┌────────────▼────────────┐                     │
│          │        machine.c        │                     │
│          │  RAM[300][4]  IR  IC  R │                     │
│          └─────────────────────────┘                     │
└─────────────────────────────────────────────────────────┘
```

---

## 📁 Project Structure

```
P1/
│
├── Header Files/
│   ├── machine.h       # Machine struct: RAM, IR, IC, R, SI, C
│   ├── job.h           # Job struct: jobId, time_limit, line_limit
│   ├── cpu.h           # fetch(), decode(), run() declarations
│   ├── mos.h           # MOS interface: init, read, write, boot, terminate
│   └── main.h          # Top-level include aggregator
│
├── Source Files/
│   ├── machine.c       # initialize(), clear_memory(), print_memory()
│   ├── job.c           # extract_params(), extract_address(), reset_job()
│   ├── cpu.c           # fetch(), decode(), run() — full CPU simulation
│   ├── mos.c           # MOS kernel: readCard(), boot(), mos_read/write()
│   └── main.c          # Entry point — job loop
│
├── Data Files/
│   ├── jobcard.txt     # Input: job definitions and data
│   └── output.txt      # Output: program results
│
└── Makefile            # Build script
```

---

## 🧩 Instruction Set

AMOS supports a minimal 6-instruction ISA. Each instruction is **4 characters wide** stored as one word in RAM.

| Mnemonic | Format | SI | Description |
|---|---|---|---|
| `LR` | `LRxx` | — | Load word at address `xx` into register R |
| `SR` | `SRxx` | — | Store register R into address `xx` |
| `CR` | `CRxx` | — | Compare R with word at `xx`; set C = T/F |
| `BT` | `BTxx` | — | Branch to address `xx` if C == T |
| `GD` | `GDxx` | SI=1 | Read input line into address `xx` (Get Data) |
| `PD` | `PDxx` | SI=2 | Print word at address `xx` to output (Put Data) |
| `H`  | `H   ` | SI=3 | Halt — terminate the current job |

> **Word size:** 4 characters. **RAM size:** 300 words (indices 0–299). **Frame size:** 10 words.

---

## 📜 Jobcard Format

Jobs are defined in `jobcard.txt` using sentinel cards:

```
$AMJ<jobId><timeLimit><lineLimit>
<program instructions — 4 chars per word, packed per line>
$DTA
<input data lines for GD instructions>
$END
```

### Example Jobcard

```
$AMJ00010050005
LR00CR01BT06SR02PD02H
$DTA
HELLO WORLD
$END
$AMJ00020050005
GD10PD10H
$DTA
AMOS WORKS
$END
```

### Field Layout for `$AMJ`

```
$AMJ | 0001 | 0050 | 0005
      ──┬──   ──┬──   ──┬──
        │       │       └── Line limit (4 digits)
        │       └────────── Time limit (4 digits)
        └────────────────── Job ID     (4 digits)
```

---

## ⚙️ How It Works

### 1. Boot Sequence
```
main()
 └── initialize()          ← zero out RAM, reset CPU registers
 └── init_mos()            ← open jobcard.txt and output.txt
 └── loop:
      └── readCard()       ← parse $AMJ, load program into RAM from address 0
      └── boot()           ← set IC=0, start CPU
           └── run()       ← fetch → decode → execute until SI triggers
           └── SI dispatch ← mos_read / mos_write / mos_terminate
      └── clear_memory()   ← wipe RAM, reset SI/IC/lines for next job
```

### 2. CPU Cycle
```
fetch()   → copy RAM[IC] into IR, increment IC
decode()  → parse IR[0..1] for opcode, IR[2..3] for address
          → set SI=1/2/3 for GD/PD/H
          → execute LR/SR/CR/BT directly
run()     → loop fetch→decode while SI==0 and within time limit
```

### 3. SI Interrupt Dispatch (in `boot()`)
```
SI == 1 (GD) → mos_read()   reads next data line from jobcard into RAM[addr]
SI == 2 (PD) → mos_write()  prints RAM[addr..addr+FRAME_SIZE] to output.txt
SI == 3 (H)  → mos_terminate() flushes output, skips to $END, breaks loop
```

---

## 🛠️ Build & Run

### Prerequisites

| Tool | Purpose | Download |
|---|---|---|
| GCC / MinGW | C compiler | [mingw-w64.org](https://www.mingw-w64.org/) |
| `mingw32-make` | Build tool (Windows) | Included with MinGW |
| `make` | Build tool (Linux/Mac) | Pre-installed |

---

### 🪟 Windows

**Option A — Direct gcc (no Makefile needed):**
```powershell
gcc -Wall -I"Header Files" "Source Files/machine.c" "Source Files/job.c" "Source Files/cpu.c" "Source Files/mos.c" "Source Files/main.c" -o os.exe
.\os.exe
```

**Option B — Makefile with mingw32-make:**

Create `Makefile` in project root (indent with **Tab**, not spaces):
```makefile
CC     = gcc
CFLAGS = -Wall -I"Header Files"
SRC    = "Source Files/machine.c" "Source Files/job.c" \
         "Source Files/cpu.c" "Source Files/mos.c" "Source Files/main.c"
OUT    = os.exe

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	del $(OUT)
```

```powershell
mingw32-make
.\os.exe
```

**Save as `build.bat` for one-click builds:**
```bat
gcc -Wall -I"Header Files" "Source Files/machine.c" "Source Files/job.c" "Source Files/cpu.c" "Source Files/mos.c" "Source Files/main.c" -o os.exe
```
```powershell
.\build.bat
.\os.exe
```

---

### 🐧 Linux / macOS

```bash
make
./os
```

Or directly:
```bash
gcc -Wall -I"Header Files" Source\ Files/machine.c Source\ Files/job.c \
    Source\ Files/cpu.c Source\ Files/mos.c Source\ Files/main.c -o os
./os
```

---

## 🔬 Machine State

The entire CPU and memory state lives in a single `Machine` struct:

```c
typedef struct {
    char RAM[300][4];   // 300 words × 4 chars — main memory
    char IR[4];         // Instruction Register — current instruction
    char R[4];          // General-purpose Register
    int  IC;            // Instruction Counter (program counter)
    int  SI;            // Supervisor Interrupt: 0=none 1=GD 2=PD 3=H
    char C;             // Condition flag: 'T' or 'F'
} Machine;
```

---

## 📊 Sample Output

```
Job ID: 1 | Time: 50 | Lines: 5

========= RAM DUMP =========
       [0]  [1]  [2]  [3]
       --- Frame 00 (RA 000-009) ---
000  |  L    R    0    0
001  |  C    R    0    1
002  |  B    T    0    6
...
============================

HELLO WORLD
Program Terminated Successfully
```

---

## 🔄 Phase Comparison

| Feature | Phase 1 | Phase 2 |
|---|---|---|
| Memory addressing | Virtual (paged) | Direct (flat) |
| Page table | ✅ Yes | ❌ Removed |
| MMU / `compute_address` | ✅ Yes | ❌ Removed |
| Frame allocation | Random, tracked | ❌ N/A |
| Error handling (PI/TI) | ✅ Full | ❌ Removed |
| SI interrupts | ✅ Yes | ✅ Yes |
| Multi-job support | ✅ Yes | ✅ Yes |
| Complexity | High | Low — clean baseline |

---

## 📚 Concepts Demonstrated

- 🖥️ **Instruction Set Architecture (ISA)** — designing and simulating a minimal ISA
- 🔁 **Fetch-Decode-Execute Cycle** — the fundamental CPU pipeline
- 🧱 **Flat Memory Model** — direct word-addressed RAM without abstraction
- 📬 **Interrupt-Driven I/O** — SI-based supervisor call mechanism
- 📂 **Batch Processing** — sequential multi-job execution from a job queue
- 🔃 **OS Lifecycle** — load → execute → terminate → reset per job

---

## 👤 Author

> Built as part of the **Operating Systems** coursework  
> Second Year — Computer Engineering  

---

<div align="center">

*"An OS is just a program that thinks it's in charge."*

![C](https://img.shields.io/badge/Written%20in-C-00599C?style=flat-square&logo=c)
![Love](https://img.shields.io/badge/Made%20with-Pain%20%26%20Printf-red?style=flat-square)

</div>
