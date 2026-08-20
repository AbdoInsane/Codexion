[8/18]
The project structure has been change due to unreadability and hard to maintain.
The current structure is easy to navigate and maintain.

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

[idea] Round-based dongle granting
Problem: per-dongle heap allows first-pusher to be served immediately
before other contenders arrive, defeating EDF priority.
Fix: remove immediate-serve. Always wait for a broadcast after push.
table_start broadcasts all dongle conds after starting threads (bootstrap).
dongle_release broadcasts on free (normal path). Each broadcast triggers
a "round" where all pending requests are checked, heap sorts them,
and the top-priority coder is granted.
