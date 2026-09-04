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
```bash
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
```

## Resources
- AI was used to find the best structure for the project.
- [CodeDumpped Channel for CPU execution cycle, OS scheduling, Process and Threads creation](https://www.youtube.com/@CoreDumpped)

## Blocking Cases Handled
- Deadlock prevention:
    Deadlock happens when multiple threads wait for each other to release a resource.
    
    In our matter, it happens when all coders take their left dongle and wait for the right dongle to be released,
    while the right dongle is held by another coder who is also waiting for their right dongle.
    This circular wait causes all coders to be stuck indefinitely — the classic *deadlock*.
    
    To prevent this, the program uses staggered thread startup:
    - reverse acquisition order:
        A very simple and effective technique, is the reverse the acquisition order for the last coder in the table.
        It works because the last coder breaks the circle by requesting the other dongle.
    - Even-indexed coders are started first, then odd-indexed coders.
    - This breaks the symmetry so not all coders attempt to acquire their left dongle simultaneously.
    - Combined with the scheduler's queue, this ensures at least one coder can always acquire both dongles.

- Starvation prevention (FIFO/EDF scheduling):
    Starvation happens when a thread is not given the CPU time it needs to complete its task.

    It occurs when a coder is kept waiting for a dongle to compile, until the deadline is reached, and it *burns out*.
    To prevent the burnout and secure a fair distribution of resources (dongles), the program uses scheduling mechanisms.

    - FIFO (first-in-first-out):
        It gives the priority to the first coder to request a dongle.
        First coder to request the dongle will be the first in the queue.
    - EDF (earliest deadline first):
        It gives the priority to the coder with the earliest deadline time.
        Coders with the closest time to burnout will be given priority.

    The scheduling mechanisms needs a queue to store the coders waiting for a dongle.
    In this project, A min heap is used to store the coders waiting for a dongle.
    It helps store the orders with the lowest value, which makes capable of being used in different scheduling algorithms.
    These scheduling mechanisms ensure that coders are given fair access to the dongles, preventing starvation and ensuring a smooth compilation process.
    

- Cooldown handling (per-dongle wait):
    When a dongle is released, it enters a `COOLDOWN` state for `dongle_cooldown` milliseconds.
    During cooldown, the dongle cannot be acquired by any coder.
    A timestamp `cooldown_end_ms` is set, and coders wait using `pthread_cond_timedwait`
    on the dongle's condition variable until the cooldown expires.
    The dongle automatically transitions to `FREE` when the cooldown elapses.

- Burnout detection (monitor thread, 10ms precision):
    A dedicated monitor thread tracks all coders' deadlines.
    Deadline = last_compile_time (or simulation start time if no compiles yet) + time_to_burnout.
    The monitor polls deadlines and immediately stops the simulation when a deadline is missed.
    On detection, it broadcasts on all condition variables to wake waiting coders.
    The burnout message is printed as the last line, within 10ms of the actual deadline.

- Log serialization (logger mutex):
    Log serialization is done using a logger mutex to prevent multiple coders from writing to the log file simultaneously.
    When a coder is ready to compile, it acquires the logger mutex and prints.
    `has taken a dongle`
    `has taken a dongle`
    `is compiling`
    Notice that it prints both `has taken a dongle` in the compiling, why didn't we print the acquisition message when acquiring a dongle?
    Well, if a coder printed the first acquisition message, it will wait for the second dongle to be acquired then it logs, which is not what we want.
    Instead, we print both acquisition messages when compiling, each acquisition message is printed with the actual acquisition time.
    This ensures both actual time log and project satisfaction.
    
    In case of a burnout, the logger will stop coders from logging and a `burned out` message will be printed last.

- N=1 edge case (single dongle, must burn out):
    With one coder, there is only one dongle.
    The coder attempts to acquire the same dongle twice (left and right are identical).
    The second acquisition blocks indefinitely because the coder already holds the dongle.
    The monitor detects the missed deadline and prints `burned out` as required.

## Thread Synchronization Mechanisms
Thread synchronization controls how multiple threads access shared resources to prevent data inconsistency and race conditions.

**Mutexes** protect shared data from concurrent access:
- Each `t_coder` has a `mutex` protecting its state, compile count, and last compile time.
- Each `t_dongle` has a `mutex` protecting its state, owner, heap, and cooldown timestamp.
- The `t_monitor` has a `mutex` protecting `working_coders`, `started_coders`, and `simulation_started`.
- The `t_table` has a `mutex` for the global `stop` flag, and a `logger_mutex` for serialized output.
- All shared struct fields are accessed only while holding the corresponding mutex.

**Condition variables** coordinate thread execution:
- Each `t_coder` has a `cond` for `sleep_coder_ms` (timed waits for compile/debug/refactor).
- Each `t_dongle` has a `cond` for waiters in `acquire_dongle` and for cooldown expiration.
- The `t_monitor` has `cond` for `working_coders` changes and `start_cond` for simulation start synchronization.
- `pthread_cond_wait`/`pthread_cond_timedwait` are used with associated mutexes held.
- `pthread_cond_broadcast`/`pthread_cond_signal` wake waiting threads on state changes.

**Min-heap per dongle** implements the scheduling queue:
- Requests are pushed with scheduler-specific keys (timestamp, deadline).
- `pop_heap` grants the dongle to the highest-priority waiter.
- Heap operations are protected by the dongle's mutex.
