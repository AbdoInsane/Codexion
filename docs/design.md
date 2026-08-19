`parser`:
	Responsible for turning program arguments/configuration into a validated configuration.

`scheduler`:
	The project's core module.

	simulation_init()
	simulation_start()
	simulation_wait()
	simulation_destroy()

`coder`:
	Everything specific to a coder:

	coder initialization
	coder thread
	coder lifecycle
	coder requesting dongles
	coder coding
	coder debugging
	coder refactoring
	coder releasing dongles

`dongle`:
	The dongle module own dongle state and operations.

	dongle_init()
	dongle_acquire()
	dongle_release()
	dongle_destroy()


                              ┌──────────────┐
                              │    main.c    │
                              │  entrypoint  │
                              └──────┬───────┘
                                     │
                                     ▼
                         ┌─────────────────────┐
                         │     simulation      │
                         │─────────────────────│
                         │ init                │
                         │ start               │
                         │ wait                │
                         │ shutdown            │
                         └──────────┬──────────┘
                                    │
             ┌──────────────────────┼────────────────────────┐
             │                      │                        │
             ▼                      ▼                        ▼
     ┌──────────────┐      ┌────────────────┐       ┌────────────────┐
     │    coder     │      │    scheduler   │       │    monitor     │
     │──────────────│      │────────────────│       │────────────────│
     │ coder_init   │      │ scheduler_init │       │ monitor_init   │
     │ coder_thread │      │ scheduler_add  │       │ monitor_thread │
     │ lifecycle    │      │ scheduler_next │       │ burnout check  │
     │ acquire      │      │ scheduler_stop │       │ termination    │
     └───────┬──────┘      └───────┬────────┘       └───────┬────────┘
             │                     │                        │
             │                     │                        │
             ▼                     │                        │
     ┌──────────────┐              │                        │
     │    dongle    │◄─────────────┘                        │
     │──────────────│                                       │
     │ init         │                                       │
     │ acquire      │                                       │
     │ release      │                                       │
     │ state        │                                       │
     └──────────────┘                                       │
                                                            │
                         ┌──────────────────────────────────┘
                         │
                         ▼
                ┌───────────────────┐
                │ termination/state │
                │ synchronization   │
                └───────────────────┘


                 SCHEDULER IMPLEMENTATION
                 =========================

                       ┌──────────────┐
                       │   scheduler  │
                       │   interface  │
                       └──────┬───────┘
                              │
                     ┌────────┴────────┐
                     │                 │
                     ▼                 ▼
              ┌─────────────┐   ┌─────────────┐
              │     FIFO    │   │     EDF     │
              │─────────────│   │─────────────│
              │ arrival     │   │ deadline    │
              │ ordering    │   │ ordering    │
              └─────────────┘   └─────────────┘


                 INPUT / CONFIGURATION
                 ======================

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
                         simulation


                 RESOURCE MANAGEMENT
                 ===================

                       ┌──────────────┐
                       │    memory    │
                       │──────────────│
                       │ allocations  │
                       │ cleanup      │
                       │ destruction  │
                       └──────┬───────┘
                              │
             ┌────────────────┼─────────────────┐
             │                │                 │
             ▼                ▼                 ▼
           coder          scheduler          monitor
             │                │                 │
             └────────────────┼─────────────────┘
                              │
                              ▼
                            dongle


                 UTILITIES
                 =========

                       ┌──────────────┐
                       │    utils     │
                       │──────────────│
                       │ ft_isdigit   │
                       │ ft_strdup    │
                       │ small helpers│
                       └──────────────┘
                              │
                              ▼
                     used where needed
