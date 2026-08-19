[8/18]
The project structure has been change due to unreadability and hard to maintain.
The current structure is easy to navigate and maintain.

[8/19]
Restructured from flat layout into per-module directories (memory, parser, coder, dongle, monitor, scheduler).
Renamed core types: t_table -> t_contex, t_alloc -> t_memory, t_args -> t_config.
Separated t_state into t_task (coder states) and t_state (dongle states).
Removed function pointers and back-pointers from structs, replaced with a single contex pointer.
Added dongle module (dongle_init with mutex/cond per dongle).
Moved orchestrator logic into scheduler_boot().
Fixed heapify_up and heapify_down bounds checking.
