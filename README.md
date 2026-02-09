# SquidgeOS

RISC-V Operating System.

## Kernel Development Roadmap

### Phase 1: Foundations & Memory Management (Current)
- [x] **Bootstrapping**: RISC-V `entry.S` and kernel entry point.
- [x] **UART Driver**: Basic serial communication for debugging.
- [x] **Panic System**: Kernel panic mechanism for fatal errors.
- [x] **Physical Memory**: Page-level allocator (free list based).
- [x] **Kernel Heap**: `kmalloc`/`kfree` with:
    - [x] Splitting of large blocks.
    - [x] Doubly-linked list headers.
    - [x] Bidirectional coalescing (Iterative).
- [~] **String Library**: Complete `lib/string.c` (`memset`, `memcpy`, `strcmp`, `strlen`).
- [~] **Formatted Printing**: Robust `kprintf` implementation for hex and decimal.

### Phase 2: Hardware Interfacing & Traps
- [ ] **Trap Handling**: 
    - [~] Setup `mtvec` (Machine Trap Vector).
    - [ ] Assembly "trampoline" to save/restore registers.
    - [~] C dispatcher for exceptions and interrupts.
- [ ] **Timer Interrupts**: 
    - [ ] Interface with RISC-V CLINT (Core Local Interrupter).
    - [ ] Implement a system heartbeat (ticks).
- [ ] **External Interrupts**: Setup PLIC (Platform-Level Interrupt Controller).

### Phase 3: Multitasking & Scheduling
- [ ] **Task Structure**: Define `task_struct` (PID, State, Stack Pointer, Priority).
- [ ] **Context Switching**: Assembly logic to switch register sets between tasks.
- [ ] **Scheduler**: 
    - [ ] Implement a Round-Robin scheduling algorithm.
    - [ ] Task lifecycle management (Create, Yield, Terminate).
- [ ] **Synchronization**: Basic Spinlocks or Semaphores for resource protection.

### Phase 4: Virtual Memory
- [ ] **Sv39 Paging**: 
    - [ ] Walk the 3-level page table structure.
    - [ ] Map/Unmap functions for virtual addresses.
- [ ] **Kernel/User Separation**: Isolate kernel memory from user processes.
- [ ] **Higher Half Kernel**: Remap kernel to upper virtual memory.

### Phase 5: Userspace & System Calls
- [ ] **User Mode Jump**: Switch CPU to User (U) mode.
- [ ] **ECALL Interface**: 
    - [ ] Implement system call dispatcher.
    - [ ] Basic syscalls: `write`, `exit`, `getpid`, `fork`.
- [ ] **ELF Loader**: Read and execute simple static binaries.

### Phase 6: Filesystem & Shell
- [ ] **VFS Layer**: Abstract File System interface.
- [ ] **Initial RAM Disk (initrd)**: Load basic files into memory.
- [ ] **User Shell**: Interactive CLI to execute commands and scripts.