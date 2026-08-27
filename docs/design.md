## main.c
The program entry point.
It initialize the table data, start the simulation and cleanup.

## table
The core data structure that assembles the table data.
For easy access and manipulation of the table data.

- **table.c**:
    `table_init`: Initializes all structures and resources _(structs and arguments)_ used in the program.
    `table_start`: Creat _monitor_ and _coders_ threads, and start simulation
    `table_destroy`: Cleans up all program resources _(threads, mutexes, cond and heap memory)_.
    `is_stop`: Chcecks the shared variable _stop_, and returns whether the simulation stoped.
    `set_stop`: Sets the shared variable _stop_ to stop the simulation, safely with no data races.
- **table.h**:
    Defines the table data structure and functions prototypes.
    `t_table`: Struct representing the table data, gives easy access to all the program components.
- **time.c**:
    Contains functions for time management.
    `get_time_ms`: Returns the current time in milliseconds.
    `wait_ms`: Sleeps the thread for a specified number of milliseconds, the releases the passed mutex while sleeping under the condition.

## parser
Parses and allocates the input arguments.

- **parser.c**:
    `get_args`: Validate number of arguments, allocate an _char *_ array of 8 for each argument and return the array
    `validate_number`: Validate numeric arguments *> 0*, except for the seventh argument *>= 0*, returns 0 on success, -1 on failure.
    `validate_scheduler`: Validate the scheduler argument only _fifo_ or _edf_ allowed, returns _edf_ or _fifo_ on success, -1 on failure.
    `get_config`: validate the input arguments, initializes and returns the configuration struct.
- **parser.h**:
    Defines the Colors, parser functions and argument structures.
- **ft_strdup.c**:
    Duplicates a string using the memory collector _ft_malloc_, returns the duplicated string on success, _NULL_ on failure.
- **ft_isnumber.c**:
    Checks if a string represents a valid number, returns `true` on success, `false` on failure.

## memory
The program's garbage collection.

- **memory.c**:
    `ft_collect`: Adds a new allocation to the garbage collector _memory_ linked list.
    `ft_malloc`: Allocates memory and adds the allocated block address to the garbage collector.
    `ft_free`: Frees all the memory blocks in the _memory_ linked list.
- **memory.h**:
    Defines the _memory_ linked list structure and garbage collector functions.

## coder
The coder threads and lifecycle management.

- **coder.c**:
    `coder_init`: Allocates and initializes the coders structure.
    `coder_start`: Creates and starts the coder thread.
    `coder_destroy`: Frees the coder thread, mutex and cond resources.
- **coder.h**:
    Defines the coder structure and function prototypes.

    `t_task`: The coder states
        _WAITING_: The coder is waiting for a dongle
        _COMPILING_: The coder is in the compiling task.
        _DEBUGGING_: The coder is in the debugging task.
        _REFACTORING_: The coder is in the refactoring task.
        _BURNOUT_: The coder burned out.
        _FINISHED_: Coder finished its life cycle.

    `t_coder`: The coder structure, contains:
        _id_: Coder id.
        _thread_: The coder thread.
        _mutex_: The coder mutex for safe reading of the shared resources.
        _state_: The coder state varible.
        _d_left_: Pointer to coder's left dongle.
        _d_right_: Pointer to coder's right dongle.
        _last_compile_time_ms_: The last compile time in milliseconds.
- **routine.c**:
    `set_coder_task`: Sets the coder state, and updates `last_compile_time_ms` when entering COMPILING state, thread-safe.
    `coder_task`: Executes a task (COMPILING, DEBUGGING, REFACTORING) by setting state, reporting, and waiting the configured duration under `table->mutex`.
    `coder_finish`: Marks coder as FINISHED, decrements `monitor->working_coders` under monitor mutex.
    `register_coder`: Registers coder at startup, increments `started_coders`, signals monitor, waits on `start_cond` for broadcast.
    `coder_routine`: Main coder thread loop: registers, repeatedly acquires two dongles, compiles, releases, debugs, refactors, waits, until required compiles reached, then finishes.
## dongle
The USB dongle resource management and synchronization.

- **dongle.c**:
    `is_dongles_owned`: Checks if both left and right dongles are owned by this coder (handles N=1 edge case where both point to same dongle).
    `acquire_dongles`: Acquires both dongles in id order to prevent deadlock, loops until both owned or stop signaled.
    `dongle_release`: Releases both dongles, sets state to COOLDOWN, records cooldown end time, broadcasts cond.
    `dongle_destroy`: Destroys all dongle mutexes and conds.
- **scheduler.c**:
    `dongle_cooldown`: Waits until cooldown expires, releasing mutex during wait, then sets state to FREE.
    `restore_dongle`: On failed second dongle acquisition, releases the first dongle back to FREE.
    `acquire_dongle`: Waits until dongle is FREE and this coder is at heap top (turn), handles cooldown, acquires ownership.
    `dongle_request`: Pushes request to heap with priority key (EDF: deadline, FIFO: current time), waits 100us, then attempts acquire.
- **heap.c**:
    Min-heap with capacity 2 (max 2 adjacent coders per dongle).
    `is_less`: Compares by key (deadline/time), tie-breaks by coder id for determinism.
    `heapify_up` / `heapify_down`: Maintains heap property.
    `push_heap`: Inserts new order, heapifies up.
    `pop_heap`: Removes and returns top order id, heapifies down.
- **init_dongle.c**:
    `setup_dongle`: Initializes a single dongle: id, owner=0, state=FREE, mutex, cond, heap (capacity=2).
    `dongle_init`: Allocates and initializes array of dongles (one per coder), circular adjacency.

## monitor
The monitor thread: synchronization barrier at startup, deadline checking, and shutdown orchestration.

- **monitor.c**:
    `monitor_start`: Creates monitor thread running `monitor_routine`.
    `monitor_init`: Allocates and initializes monitor: time_ms=0, started_coders=0, working_coders=N, mutex, cond, start_cond.
    `monitor_destroy`: Joins monitor thread, destroys mutex and cond.
- **routine.c**:
    `check_deadlines`: Scans all coders, returns index of first whose deadline passed (last_compile + burnout) and not FINISHED.
    `wait_for_coders`: Waits for all coders to register (started_coders == N), records global start time, broadcasts `start_cond` to release all coders simultaneously.
    `monitor_routine`: Waits for coders, then loops: if working_coders==0 exit; check deadlines, on burnout sets coder state to BURNOUT, reports, sets global stop, returns.

## logger
Thread-safe event logging with relative timestamps.

- **log.c**:
    `get_coder_data`: Reads coder id and state under coder mutex.
    `display_event`: Formats and prints event based on task (COMPILING prints two "has taken a dongle" lines + "is compiling"; others print single line).
    `report_task`: Called by coder/monitor to log state changes; computes relative time from monitor start; returns early if stop signaled; uses `logger_mutex` to serialize stdout.
