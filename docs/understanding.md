### Threads/Process

- Process:
    A process is a program that being executed
    Each process has its own memory space, independent execution and other resources
    A process may contain multiple threads
- Thread:
    A thread is the smallest unit of execution within a process, threads within the same process share the same memory space.
    A thread has its own stack, CPU registers and execution context.


## TCB

The Operating System is maintains management information about each process in a **Process Control Block** (PCB).
When a process creates multiples threads, each thread has its own **Thread Control Block** (TCB).
A TCB is a data structure in an operating system kernel that stores the execution state, ID, stack pointer, and scheduling details needed to manage an individual thread.

Thread Control Block data structure:
- `Thread ID`: Unique identifier for the thread.
- `Thread state`: Current state of the thread (e.g., running, waiting, terminated).
- `Program counter`: Address of the next instruction to be executed.
- `Registers`: Saved state of the CPU registers.
- `Stack Pointer`: Address of the top of the thread's stack.
- `Priority`: Priority of the thread, This is a number that tells the scheduler how important the thread is.
- `Pointer to Process Control Block (PCB)`: Address of the PCB of the main process.

## Threads Creation

```C
pthread_create(&thread, NULL, start_routine, &args)
      ^           ^      ^         ^           ^
      |           |      |         |           |----- arguments to be passed to the thread's entry point
      |           |      |         |           
      |           |      |         |----------------- The function address of the thread's entry point
      |           |      |       
      |           |      |--------------------------- Used to customize the behavior, resources, and properties of the newly created thread. NULL for default
      |           |
      |           |---------------------------------- A memory address reference to the pthread struct wrapper for the TCB
      |
      |---------------------------------------------- POSIX library API for creating the thread TCB and pthread wrapper
```
When calling `pthread_create` requests is sent to the OS to create a **Kernel Space** TCB, then wrapps it in a **User Space** pthread struct.
The pthread struct address is saved in the `thread` struct we passed, to identify the thread.
The TCB stack pointer is initialized to the `start_routine` address, and the args are saved in the registers.
Then when the threads get scheduled, they will execute the `start_routine` function with the saved args.

## Threads sleeping
