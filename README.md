*This project has been created as part of the 42 curriculum by abait-mo*

# Codexion

## Description
`Codexion` is a multithreaded simulation of coders competing for USB dongles.
The project is based on the classic `Dining Philosophers` problem.
The program takes 8 arguments:
- `number_of_coders`: number of coders (also the number of dongles)
- `time_to_burnout`: time before a coder burns out, in milliseconds
- `time_to_compile`: time to compile, in milliseconds
- `time_to_debug`: time to debug, in milliseconds
- `time_to_refactor`: time to refactor, in milliseconds
- `number_of_compiles_required`: number of compiles required per coder
- `dongle_cooldown`: dongle cooldown period, in milliseconds
- `scheduler`: scheduling policy, `fifo` (first-in-first-out) or `edf` (earliest deadline first)

Each coder runs in its own thread and has a left and right dongle; it must acquire both to start compiling.
A coder's life cycle is compile → debug → refactor, and it needs both dongles to start the cycle.
Coders are synchronized for the limited dongle resources using mutexes and condition variables.
Dongle acquisition must follow the chosen scheduling policy (`fifo` or `edf`).
After a dongle is released, it enters a cooldown period before it can be acquired again.
Coders compete to acquire dongles; if a coder misses its `time_to_burnout` deadline, it *burns out*.
A separate monitor thread watches all coders, detects burnout, and stops the simulation within 10 ms of the deadline.

## Instructions
### Compile
`make`: compiles the program.
`make re`: recompiles the program.

### Cleaning
`make clean`: removes compiled objects and the build folder.
`make fclean`: removes the compiled program.

### Run
`./codexion 5 3000 200 200 200 10 400 fifo`
            |    |   |   |   |  |   |   |
            |    |   |   |   |  |   |   └─────── `scheduler` (fifo/edf)
            |    |   |   |   |  |   └─────────── `dongle_cooldown` (ms)
            |    |   |   |   |  └─────────────── `number_of_compiles_required`
            |    |   |   |   └────────────────── `time_to_refactor` (ms)
            |    |   |   └────────────────────── `time_to_debug` (ms)
            |    |   └────────────────────────── `time_to_compile` (ms)
            |    └────────────────────────────── `time_to_burnout` (ms)
            └─────────────────────────────────── `number_of_coders`


## Resources
<!-- References used (POSIX threads, 42 subject, etc.). How AI was used - specify for which tasks (code, debugging, docs, testing, design). -->

## Blocking Cases Handled
<!-- List each concurrency issue addressed with one-line explanation:
- Deadlock prevention (ordered dongle acquisition)
- Starvation prevention (FIFO/EDF scheduling)
- Cooldown handling (per-dongle wait)
- Burnout detection (monitor thread, 10ms precision)
- Log serialization (logger mutex)
- N=1 edge case (single dongle, must burn out) -->

## Thread Synchronization Mechanisms
<!-- List mutexes, condition variables, lock hierarchy, and 2-3 race prevention examples with code location references. -->
