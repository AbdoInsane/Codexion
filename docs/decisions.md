[8/18]
The project structure has been change due to unreadability and hard to maintain.
The current structure is easy to navigate and maintain.
Restructured from flat layout into per-module directories (memory, parser, coder, dongle, monitor, scheduler).
Renamed core types: t_table -> t_contex, t_alloc -> t_memory, t_args -> t_config.
Separated t_state into t_task (coder states) and t_state (dongle states).
Removed function pointers and back-pointers from structs, replaced with a single contex pointer.
Added dongle module (dongle_init with mutex/cond per dongle).
Moved orchestrator logic into scheduler_boot().

[8/19]
Restructured from flat layout into per-module directories (memory, parser, coder, dongle, monitor).
Removed the scheduler module entirely, moved heap to dongle module.
Each dongle now owns its own heap for per-dongle FIFO/EDF arbitration.
Dongles use ordered acquisition (lower-indexed first) to prevent deadlock.
Added table module (table_init, table_start, table_destroy, is_stop, set_stop).
Added logger module with serialized task reporting (logger_mutex).
Implemented full coder lifecycle (acquire, compile, release, debug, refactor).
Implemented monitor with deadline checking and burnout detection.
Fixed get_time_ms() returning microseconds instead of milliseconds.
Fixed dongle_release setting left dongle to FREE instead of COOLDOWN.
Fixed check_deadlines burnout condition inverted (<= should be >).
Fixed heapify_up and heapify_down bounds checking.

[8/20]
Fixed heap key type: unsigned int to long for EDF deadline support.
Replaced usleep with interruptible sleep_or_stop (pthread_cond_timedwait).
Fixed cooldown busy-wait spinloop with pthread_cond_timedwait.
set_stop now broadcasts table->cond to wake sleeping coder threads.
Each dongle broadcasts its cond on release, waking all waiting coders.

[8/21]
Added generic wait_ms() in time.c: pthread_cond_timedwait wrapper that locks
and unlocks the passed mutex internally. Replaces sleep_or_stop() (removed
from table.c; the table.h prototype is now stale).
Coder state writes go through set_coder_task() (protected by coder->mutex),
which also stamps last_compile_time_ms when entering COMPILING. t_coder
now carries a mutex instead of a cond.
Coder ids are 1-indexed (i + 1); check_deadlines returns the coder id.
Removed ANSI color macros from log.h; report_task output is plain text.
ACQUIRING is no longer logged: the COMPILING branch of report_task prints
the two "has taken a dongle" lines then "is compiling", matching the
subject's required output.
check_deadlines reads coder state under coder->mutex (was monitor->mutex)
and flags burnout strictly after the deadline (<= -> <).
dongle.c: extracted wait_for_dongl() splitting the heap position wait from
the cooldown wait (which uses wait_ms).
Known bug: wait_for_dongl does not re-verify heap position after the
cooldown wait, so a late push with a lower key can be popped by the
wrong coder.

[idea] Round-based dongle granting
Problem: per-dongle heap allows first-pusher to be served immediately
before other contenders arrive, defeating EDF priority.
Fix: remove immediate-serve. Always wait for a broadcast after push.
table_start broadcasts all dongle conds after starting threads (bootstrap).
dongle_release broadcasts on free (normal path). Each broadcast triggers
a "round" where all pending requests are checked, heap sorts them,
and the top-priority coder is granted.
