*This project has been created as part of the 42 curriculum by abait-mo*

# Codexion

## Description
`Codexion` is a multithreading simulation of coders competing for USB dongles.
The project is based on a popular problem `Dining Philosophers`.
The program takes 8 arguments:
- `number_of_coders`: number of coders and also number of dongles
- `time_to_burnout`: time to burnout in milliseconds
- `time_to_compile`: time to compile in milliseconds
- `time_to_debug`: time to debug in milliseconds
- `time_to_refactor`: time to refactor in milliseconds
- `number_of_compile_required`: number of compile required in milliseconds
- `dongle_cooldown`: dongle cooldown in milliseconds
- `scheduler`: scheduler type `fifo` (first-in-first-out) or `edf` (earliest deadline first)

Each coder is represented by a thread, a coder has two dongles left and right, and must acquire both for it to start compiling.
Coders have a life cycle of compiling -> debuging -> refactor, and it need both dongles to start the cycle.
We must synchronize the coders for the limited resources using mutexes and condition variables.
Dongles acquisition must follow the provided scheduling rule `(fifo/edf)`.
After a dongles is released, it must be cooled down before it can be acquired again.
Coders compete for dongles acquisition to start compiling, when a coder misses its `time_to_burnout` deadline it *burns out*.
A separated monitor thread is responsible for monitoring the coders and detecting when they burn out and stoping the simulation within `10ms` of the `time_to_burnout` deadline.

## Instructions
### Compile
`make`: compiles the program.
`make re`: recompiles the program.

### Cleaning
`make clean`: removes the compiled objects and build folder.
`make fclean`: removes the compiled program.


### Run
`./codexion 5 3000 200 200 200 10 400 fifo`: runs the program, use any arguments.
            |   |   |   |   |   |   |   |
            |--------------------------------> `number_of_coders`
                |   |   |   |   |   |   |
                |----------------------------> `time_to_burnout`
                    |   |   |   |   |   |
                    |------------------------> `time_to_compile`
                        |   |   |   |   |
                        |--------------------> `time_to_debug`
                            |   |   |   |
                            |----------------> `time_to_refactor`
                                |   |   |
                                |------------> `number_of_compiles_required`
                                    |   |
                                    |--------> `dongle_cooldown`
                                        |
                                        |----> `scheduler`



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
