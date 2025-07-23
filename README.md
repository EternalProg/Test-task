# Test task

##  Description

Implements a multithreaded command-line application in C++ (C++14) that meets the following requirements:

- One **producer** thread generates a sequence of integers.
- Two **consumer** threads retrieve and print values from a **shared queue**.
- **No mutexes or locks** are used (lock-free behavior).
- The program **exits appropriately** after all data is processed.
- Compatible with **Linux x86_64**, builded with Make using (g++ compiler);

---

## Build

Ensure you have GCC installed (with C++14 support). Then run:
```bash
make
./main
```
```bash
#To clean the build:
make clean
```

## Project Structure
```
├── lock_free_queue.hpp   # Lock-free queue class (header)
├── lock_free_queue.cpp   # Lock-free queue class (implementation)
├── main.cpp              # Main logic: producer and consumers
├── Makefile              # Build instructions
```

## Implementation Details
####  [THOUGHTS]

The task requirements were somewhat unclear for me. It’s not specified whether **third-party libraries** are allowed, or whether I need to keep the output order.

Therefore, I assumed only standard C++ is allowed, and strict output ordering is not required.

Because of this, and because of limitations on synchronization primitives (mutexes...), the output is not synchronized and may be unordered.

My queue implementation is acceptable only to this specific case.

---
The shared queue is implemented as a **Lock-Free Queue** (ring buffer) using a fixed-size array of ```slot_t``` elements. Each ```slot_t``` contains:
- a ```value``` field to hold the integer,
- an ```atomic<bool>``` flag **ready_to_consume** that indicates whether the value is available to read.

I use:

```atomic<uint32_t> write_index_``` — incremented by the producer.

```atomic<uint32_t> read_index_``` — incremented by the consumers.

### Push (Producer)
Producer writes a value at ```write_index_ % capacity```, and waits if the slot is not yet consumed, achieving bounded buffer behavior.

### Pop (Consumers)
Consumers read at ```read_index_ % capacity```, and process values only if ```ready_to_consume``` is set to true.

### Shutdown Signal
A shared atomic flag ```shutdown``` is set to true once all values are produced. Consumers exit once they’ve consumed all the data.