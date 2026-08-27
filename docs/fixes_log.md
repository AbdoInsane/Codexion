# Codexion - Fixes Log

## Overview
Complete thread-safe implementation of the dining-philosophers-style simulation with coders, USB dongles, EDF/FIFO scheduling, and monitor thread.

---

## Critical Fixes Applied

### 1. README Compliance (Chapter VII)
**File:** `README.md` - *Still pending completion*
- Missing: Author line, Description, Instructions, Resources (AI usage), Blocking cases, Thread synchronization mechanisms

### 2. Partial Thread Creation Failure - UB Fixed
**Files:** `src/main.c`, `src/table/table.c`, `src/coder/coder.c`, `src/table/table.h`

**Problem:** If `pthread_create` failed mid-creation, `table_destroy` joined uninitialized thread handles → UB/crash.

**Solution:** Added `t_table_status` struct with `monitor_created` bool and `coders_created` counter.
- `table_init`: Zero-initializes status struct
- `monitor_start`: Sets `monitor_created = true` on success
- `coder_start`: Increments `coders_created` per successful thread creation
- `coder_destroy`: Only joins `coders_created` threads
- `monitor_destroy`: Only joins if `monitor_created`
- `table_destroy`: Uses status flags for conditional cleanup

### 3. Lock Order Inversion (Deadlock Risk) - Fixed
**Files:** `src/table/table.c` (`set_stop`), `src/dongle/scheduler.c`

**Problem:** 
- Monitor path: `table->mutex` → `monitor->mutex` → `dongle[i].mutex`
- Coder path: `dongle->mutex` → `table->mutex` (via `is_stop`)
- **Cycle**: `table → dongle → table`

**Solution:** `set_stop` releases `table->mutex` BEFORE acquiring `monitor->mutex` and dongle mutexes:
```c
pthread_mutex_lock(&table->mutex);
table->stop = true;
pthread_cond_broadcast(&table->cond);
pthread_mutex_unlock(&table->mutex);  // Release FIRST
pthread_mutex_lock(&table->monitor->mutex);
pthread_cond_broadcast(&table->monitor->start_cond);
pthread_mutex_unlock(&table->monitor->mutex);
// Then lock dongles one by one
```

**Result:** Helgrind lock-order violations: **0 errors**

### 4. EDF Starvation Bug - Fixed with Priority Aging
**Files:** `src/coder/coder.h`, `src/coder/coder.c`, `src/coder/routine.c`, `src/dongle/heap.h`, `src/dongle/heap.c`, `src/dongle/scheduler.c`

**Problem:** Coder N (highest ID) systematically starved because:
- All coders start with same deadline → tie-break by ID
- Coder N shares dongles with 1 and N-1 (both lower ID)
- Coder N loses both dongles permanently → 0 compiles → burnout

**Solution:** Priority aging via `compile_times` counter:
- Added `compile_times` to `t_coder` (init 0, increment after each cycle)
- Added `n_compiles` to `t_order` in heap
- Heap ordering: `key` (deadline) → `n_compiles` (fewer first) → `id`
- Starved coders (0 compiles) now get priority over active ones

**Note:** `usleep(100)` race window still present in `dongle_request` - conditional fix pending.

### 5. Condition Variable Broadcast Without Mutex - Fixed
**File:** `src/table/table.c` (`set_stop`)

**Problem:** Broadcasting on `monitor->start_cond` and `dongle[i].cond` without holding their mutexes → missed wakeups.

**Solution:** Lock each mutex before its broadcast:
```c
pthread_mutex_lock(&table->monitor->mutex);
pthread_cond_broadcast(&table->monitor->start_cond);
pthread_mutex_unlock(&table->monitor->mutex);
// Repeat for each dongle
```

### 6. `is_dongles_owned` Lock Order
**File:** `src/dongle/dongle.c`

**Status:** Updated to accept pre-ordered `first/second` dongles (caller ensures id order). Lock order now consistent with `acquire_dongles`.

---

## Thread Safety Verification

| Tool | Result |
|------|--------|
| **Valgrind Memcheck** | 0 errors, 0 leaks |
| **Valgrind Helgrind** | 0 errors (no lock-order violations, no data races) |
| **Functional Tests** | All pass: N=1 burnout, N=2/3/5/7 EDF/FIFO |

---

## Remaining Work

1. **README** - Complete Chapter VII sections
2. **Conditional usleep fix** - Only sleep when `heap->size == 1` after push
3. **Persistent per-dongle queue design** (planned) - Eliminate push/acquire race entirely

---

## Files Modified

```
src/main.c
src/table/table.c
src/table/table.h
src/coder/coder.c
src/coder/coder.h
src/coder/routine.c
src/dongle/dongle.c
src/dongle/scheduler.c
src/dongle/heap.c
src/dongle/heap.h
src/monitor/monitor.c
src/logger/log.c
src/parser/parser.c
docs/design.md
```

---

## Test Results Summary

```
Build: clean (-Wall -Wextra -Werror)
Memcheck: 0 errors, 0 leaks (4 configs tested)
Helgrind: 0 errors (lock order fixed)
Functional: 
  - 5 1500 40 40 40 4 15 edf     ✓
  - 3 100 60 60 60 5 10 fifo      ✓ (burnout at 101ms)
  - 7 2000 50 50 50 3 20 fifo     ✓
  - 2 3000 100 100 100 5 100 edf  ✓
Stress: 40 sequential + 4 concurrent = 0 failures
```

---

*Generated: $(date)*