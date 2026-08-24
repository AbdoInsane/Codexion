`parser`:
	Responsible for turning program arguments/configuration into a validated configuration.

`table`:
	The orchestrator module. Owns config, coders, dongles, monitor and the
	global stop flag.

	table_init()
	table_start()
	table_destroy()
	set_stop()
	is_stop()

`coder`:
	Everything specific to a coder:

	coder initialization
	coder thread
	coder lifecycle (compile -> debug -> refactor -> repeat)
	coder requesting dongles
	coder coding
	coder releasing dongles
	coder debugging
	coder refactoring

`dongle`:
	The dongle module owns dongle state and operations, plus the per-dongle
	heap used for FIFO/EDF arbitration.

	dongle_init()
	acquire_dongles()
	dongle_release()
	dongle_destroy()
	push_heap() / pop_heap()

`monitor`:
	Watches deadlines and detects burnout, terminates the simulation.

	monitor_init()
	monitor_start()
	monitor_thread()
	check_deadlines()
	monitor_destroy()

`logger`:
	Serialized plain-text task reporting.

	report_task()

`memory`:
	Single-block allocation tracking.

	ft_malloc()
	ft_free()

`time`:
	get_time_ms()
	wait_ms()


                              ┌──────────────┐
                              │    main.c    │
                              │  entrypoint  │
                              └──────┬───────┘
                                     │
                                     ▼
                          ┌─────────────────────┐
                          │       table         │
                          │─────────────────────│
                          │ init                │
                          │ start               │
                          │ destroy             │
                          │ stop flag           │
                          └──────────┬──────────┘
                                     │
              ┌──────────────────────┼────────────────────────┐
              │                      │                        │
              ▼                      ▼                        ▼
      ┌────────────────┐      ┌────────────────┐       ┌────────────────┐
      │    coder       │      │    monitor     │       │     parser     │
      │────────────────│      │────────────────│       │────────────────│
      │ coder_init     │      │ monitor_init   │       │ get_config     │
      │ coder_start    │      │ monitor_thread │       │ config object  │
      │ coder_routine  │      │ check_deadlines│       └────────────────┘
      │ set_coder_task │      │ set_stop       │
      └───────┬────────┘      └───────┬────────┘
              │                       │
              ▼                       │
      ┌──────────────┐                │
      │    dongle    │                │
      │──────────────│                │
      │ dongle_init  │                │
      │ acquire      │                │
      │ release      │                │
      │ heap         │                │
      │ FIFO / EDF   │                │
      └──────────────┘                │
                                      │
                ┌─────────────────────┘
                ▼
        ┌───────────────────┐
        │ termination/state │
        │ synchronization   │
        └───────────────────┘


                 DONGLE SCHEDULING
                 =================

              Each dongle owns a min-heap (max 2 entries, the two coders
              sharing that dongle). A coder pushes its request key, then
              waits until its entry is at the top and the dongle is not
              ACQUIRED. It pops only after re-verifying its position.

                       ┌──────────────┐
                       │   per-dongle │
                       │     heap     │
                       └──────┬───────┘
                              │
                     ┌────────┴────────┐
                     │                 │
                     ▼                 ▼
              ┌─────────────┐   ┌─────────────┐
              │     FIFO    │   │     EDF     │
              │─────────────│   │─────────────│
              │ key =       │   │ key =       │
              │ get_time_ms │   │ last_compile│
              │             │   │ + burnout   │
              └─────────────┘   └─────────────┘

              Deadlock prevention: coders always acquire dongles in
              ascending id order (lower-indexed dongle first).


                 INPUT / CONFIGURATION
                 =====================

                       ┌──────────────┐
                       │    parser    │
                       │──────────────│
                       │ parse argv   │
                       │ validate     │
                       │ build config │
                       └──────┬───────┘
                              │
                              ▼
                       ┌──────────────┐
                       │ configuration│
                       └──────┬───────┘
                              │
                              ▼
                          table (simulation)


                 RESOURCE MANAGEMENT
                 ===================

                       ┌──────────────┐
                       │    memory    │
                       │──────────────│
                       │ ft_malloc    │
                       │ ft_free      │
                       │ cleanup      │
                       └──────┬───────┘
                              │
              ┌────────────────┼─────────────────┐
              │                │                 │
              ▼                ▼                 ▼
            coder          dongle            monitor
              │                │                 │
              └────────────────┼─────────────────┘
                               │
                               ▼
                            table


                 UTILITIES
                 =========

                       ┌──────────────┐
                       │     time     │
                       │──────────────│
                       │ get_time_ms  │
                       │ wait_ms      │
                       └──────┬───────┘
                              │
                      used where needed
