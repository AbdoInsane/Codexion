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
- AI was used to find the best structure for the project.
- [CodeDumpped Channel for CPU and OS scheduling](https://www.youtube.com/@CoreDumpped)

## Blocking Cases Handled
- Deadlock prevention:
    Deadlock happens when multiple threads wait for each other to release a resource.
    
    In our matter, it happens when all coders take their left/right dongle and wait for the other to release it
    This way all coders will be stuck waiting for each other to release the dongle
    And it will never happen, thats the *deadlock*.
    There are several ways to prevent it, but i only used one.
    - reverse acquisition order:
        A very simple and effective technique, is the reverse the acquisition order for the last coder in the table.
        It works because the last coder breaks the circle by requesting the other dongle.

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
    When a dongle is released, it can't be acquired by any coder for a certain period of time.
    After release the dongle is giving a COOLDOWN state and a available_at timestamp, which is used to determine when the dongle can be acquired again.
    Coders who request a cooldown dongle will be slept using *pthread_cond_timedwait* for the remaining cooldown time before being woken up.

- Burnout detection (monitor thread, 10ms precision):
    A separated monitor thread is used to detect burnout of coders.
    It keeps track of coders deadline times and checks for a passed deadline.
    When the monitor thread detects a passed deadline, it immediately stops the simulation.
    When the simulation is stopped, All channels are broadcasted to notify the coders.
    This ensures the burnout is detected under 10ms precision from the deadline time.

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
    When a single coder is passed to the program, that coder only finds one dongle available, so it must burn out.
    The coder only acquires the only dongle, and stucks waiting for the other one (which he already has), which burns out after the deadline.

## Thread Synchronization Mechanisms
<!-- List mutexes, condition variables, lock hierarchy, and 2-3 race prevention examples with code location references. -->
