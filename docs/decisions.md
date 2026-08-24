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

[idea] Round-based dongle granting
Problem: per-dongle heap allows first-pusher to be served immediately
before other contenders arrive, defeating EDF priority.
Fix: remove immediate-serve. Always wait for a broadcast after push.
table_start broadcasts all dongle conds after starting threads (bootstrap).
dongle_release broadcasts on free (normal path). Each broadcast triggers
a "round" where all pending requests are checked, heap sorts them,
and the top-priority coder is granted.

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

[8/21 – cooldown-before-acquire review]
moved cooldown wait (wait_ms) to execute BEFORE the heap-position wait in
wait_for_dongl. intent: the cooldown window acts as a gather period so
other contenders can push before the position check.

bug 1 – deadlock:
wait_for_dongl is called with dongle->mutex held (from dongle_request after
push_heap). the cooldown loop calls wait_ms, which internally does
pthread_mutex_lock(mutex) on the same non-recursive mutex -> deadlock.
fix: uncomment the pthread_mutex_unlock / pthread_mutex_lock around each
wait_ms call; wait_ms manages its own lock/unlock cycle.

bug 2 – acquire during active cooldown:
after the position wait exits (thread at top, state != ACQUIRED), a new
dongle_release can set state = COOLDOWN. the thread proceeds to pop_heap
and set ACQUIRED without waiting for the new cooldown to expire.
scenario: thread A holds dongle, thread B enters wait_for_dongl, no active
cooldown -> phase 1 skips, phase 2 waits (state == ACQUIRED). A releases ->
state = COOLDOWN. B wakes, state != ACQUIRED -> exits position wait -> acquires.
but cooldown just started and hasn't expired.
fix: add phase 3 after position wait — re-check for COOLDOWN and wait if
active, then re-verify heap position (another coder may have pushed with a
lower key during the phase 3 wait).

remaining issues to address (edf testing + further fixes):
- heap-position race: after any wait_ms (which releases the mutex), a late
  push with a lower key can displace the current thread. pop_heap does not
  verify orders[0].id == coder->id. needs re-verification after cooldown.
- table.h still has stale sleep_or_stop declaration (removed from table.c).
- log.c has leftover debug line (// return;).
- monitor.c busy-loop with no usleep (100% cpu).
- coder_destroy missing pthread_mutex_destroy for per-coder mutexes.
- Makefile 'all' target runs the binary (should be removed).

[8/24] - dongle.c hang root causes identified
Critical hang causes not yet fixed:
- A: wait_for_dongl line 35 sets state=FREE but omits pthread_cond_broadcast
- B: dongle_request line 80 pop_heap removes top but omits broadcast to wake next waiter
- C: acquire_dongles leaks first dongle on second dongle failure (no rollback)

These three issues explain the observed deadlock/hang in EDF testing.
Fix requires: (1) add broadcasts at A and B, (2) add release_single helper for C rollback.

[edf test cases to validate]
1. N=3, short time_to_burnout (e.g. 800ms), long compile/debug/refactor
   (e.g. 400/200/200). EDF should prioritize the coder nearest to burnout.
   if coder A compiled recently and B compiled long ago, B should get the
   dongle first (lower deadline = earlier last_compile + burnout).
   expected: B compiles before A, A's deadline pushed further out after B's
   compile resets B's last_compile_time_ms.

2. N=3, equal deadlines (all start at same time). EDF should fall back to
   FIFO (earlier push gets served first). verify by checking log order —
   first "has taken a dongle" lines should follow push order.

3. N=3, one coder stuck waiting long (debug + refactor phase while others
   compile repeatedly). that coder's deadline should approach burnout.
   monitor should flag burnout if the coder doesn't compile before deadline.
   verify: burnout message prints correct coder id, simulation stops.

4. N=3, dongle_cooldown = 0. verify EDF still works without cooldown
   interference — no false acquires during non-existent cooldown.

5. N=3, large time_to_burnout (e.g. 10000ms). EDF should behave like FIFO
   since all deadlines are far out. verify log order matches push order.

6. N=2, EDF, time_to_burnout = 200ms, short phases (10/10/10).
   both coders burn out quickly. verify monitor catches the first one to
   expire and stops cleanly without deadlock.
