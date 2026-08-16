# CacheBenchmark - BCC266

A CPU cache hierarchy benchmark simulator built in C, designed as a practical project for the BCC266 course at UFOP (Universidade Federal de Ouro Preto).

This tool simulates a complete memory hierarchy — from CPU registers through L1, L2, and L3 caches, down to RAM and a physical hard disk — allowing you to configure cache sizes, replacement policies, and other parameters to analyze performance metrics such as hit rates, miss rates, and access latency.

## Members

- **Eliseu Eloi**
- **Marcos Vinicius**
- **Yuri Henrique Reis**

- ## Features

- **Multi-level cache hierarchy** — Configurable L1, L2, and L3 cache sizes (in number of blocks)
- **Cache replacement policies** — LRU (Least Recently Used), LIP (Likely Page Replacement), LFU (Least Frequently Used), and SRRIP (Steering Round Robin)
- **Physical disk simulation** — Real `HD.bin` file used as backing store with actual file I/O to simulate disk access latency
- **Write buffer** — Optional write-back buffer to reduce RAM stalls on store operations
- **Interrupt handling** — Nested interrupt support with a stack-based execution context system; interrupt handlers loaded from `TI.txt`
- **Instruction generation** — Random memory access programs with configurable repetition probability (50%, 75%, 90%)
- **Comprehensive benchmarking** — Tracks hits/misses per cache level, RAM hit rate, disk miss rate, simulated clock cycles, and wall-clock time
- **Standard test suite** — Five predefined machine configurations (M1–M5) for consistent comparison
- **Interactive terminal UI** — ncurses-based menus, checkboxes, and color-coded Unicode tables for configuration and results

## Memory Hierarchy Costs

| Component  | Cost (clock cycles) |
| ---------- | ------------------- |
| L1 access  | 10                  |
| L2 access  | 30                  |
| L3 access  | 100                 |
| RAM access | 600                 |
| HD access  | 10000               |

## Memory Sizes

| Resource | Default Size             |
| -------- | ------------------------ |
| RAM      | 1000 cache lines         |
| HD       | 50000 cache lines (~2MB) |
| L1/L2/L3 | User-configurable        |

## Getting Started

### Prerequisites

- **gcc** (C compiler)
- **libncurses-dev** (for the terminal UI)

### Build and Run

```bash
make        # Compile the project
make run    # Build and execute
./exe       # Run the compiled binary
```

### Clean

```bash
make clean  # Remove compiled binaries and object files
```

### Memory Checking

```bash
valgrind --suppressions=valgrind.supp ./exe
```

## Project Structure

```
TP-BCC266/
├── src/              # Source files (.c)
│   ├── main.c        # Application entry point and main loop
│   ├── cpu.c         # CPU instruction execution engine
│   ├── ram.c         # RAM (main memory) management
│   ├── MMU.c         # Memory Management Unit — cache policies & data movement
│   ├── disco.c       # Disk (HD) I/O simulation
│   ├── pilha.c       # Execution stack for interrupt handling
│   ├── menu.c        # ncurses-based interactive menu system
│   ├── tabela.c      # Report generation with Unicode tables
│   └── utils.c       # Instruction generation & benchmark orchestration
├── include/          # Header files (.h)
│   ├── structs.h     # Core data structures & constants
│   ├── cpu.h         # CPU function declarations
│   ├── ram.h         # RAM function declarations
│   ├── MMU.h         # Cache/MMU function declarations
│   ├── disco.h       # Disk I/O function declarations
│   ├── pilha.h       # Stack data structure declarations
│   ├── menu.h        # Menu/ncurses function declarations
│   ├── tabela.h      # Table rendering declarations
│   ├── utils.h       # Utility function declarations
│   └── cores.h       # ANSI color codes & drawing characters
├── Makefile
├── HD.bin            # Hard disk image (created at runtime)
├── TI.txt            # Interrupt instruction program data
└── valgrind.supp     # Valgrind suppression file for ncurses
```

## How It Works

The simulator executes a virtual instruction set with the following opcodes:

- **ADD (0)** — Reads two memory locations, adds them, writes the result
- **SUB (1)** — Reads two memory locations, subtracts them, writes the result
- **INTERRUPT (2)** — Triggers a nested interrupt: pushes the current PC to the execution stack and jumps to the interrupt handler (PC=0)
- **HALT (-1)** — Stops execution

During execution, every memory access traverses the cache hierarchy (L1 → L2 → L3 → RAM → HD). The MMU tracks hits and misses at each level, applies the configured replacement policy when evicting cache lines, and measures the simulated and real time taken for each access.

## Strengths

- **Well-modularized architecture** — Clean separation between CPU, MMU, RAM, disk, stack, menu, and utility modules, each with dedicated source and header files
- **Complete cache hierarchy simulation** — Realistic multi-level cache system with accurate cycle-accurate latency modeling from L1 through physical disk
- **Flexible configuration** — User can adjust cache sizes, replacement policies, write buffer settings, and instruction generation parameters via an interactive ncurses UI
- **Accurate replacement policies** — Implements LRU, LIP, LFU, and SRRIP, all with proper cache line priority tracking
- **Physical disk backing store** — Uses a real binary file (`HD.bin`) with actual file I/O, providing a realistic simulation of disk access latency
- **Interrupt support** — Stack-based nested interrupt handling with configurable interrupt handler programs
- **Comprehensive benchmarking** — Detailed metrics (hits/misses per level, RAM hit rate, disk miss rate, simulated and real time) rendered in formatted Unicode tables
- **Memory safety** — Includes a Valgrind suppression file for ncurses and follows careful memory management practices throughout the codebase
