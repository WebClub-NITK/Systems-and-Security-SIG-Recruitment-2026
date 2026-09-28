# Build Your Own Redis

**Tags:** DataStores · Socket Programming · Event-Driven Architecture · Crash Recovery

## Introduction

Redis is an in-memory key-value database. Clients connect over TCP, send commands like `SET name alice` and `GET name`, and the server replies on the same connection. Everything lives in RAM, which is why it is fast, and it writes to disk periodically so data survives a restart.

Writing a version that works in the normal case is straightforward. Most of the engineering lives in the cases that are not normal: data arriving in fragments across multiple `read()` calls, a client that stops reading mid-reply, a hashtable resize that stalls every other connection, a process killed halfway through a disk write.

This task requires building a Redis-compatible server from scratch — protocol parser, event loop, hashtable, expiry, and a write-ahead log with crash recovery — and then measuring it honestly.

The real `redis-cli` must be able to connect to your server and use it.

**Language:** C, C++, Rust, or Zig. No garbage-collected languages. No async frameworks (Tokio, libuv, asio, boost::asio). No hashmap libraries — write your own.

---

## Problem Statements

### Phase 0: Per-Student Constants

Compute the SHA-256 hash of your roll number. Derive four constants from its bytes and use them throughout your implementation.

**Requirements:**

| Constant | Derivation | Range |
|---|---|---|
| `HASH_SEED` | bytes 8–15 as a 64-bit little-endian integer | any |
| `PROBE_CAP` | `8 + (byte[16] % 9)` | 8–16 |
| `EVICT_SAMPLE` | `3 + (byte[17] % 6)` | 3–8 |
| `MAGIC` | `"RDB"` followed by the first 5 hex characters of the hash | 8 bytes |

Record them in a file named `params.txt` at the repository root.

The grading script recomputes these values from your roll number and runs your binary against them. A submission built with different constants will not pass.

---

### Phase 1: Protocol Parsing

Implement a TCP server on port 6379 that speaks RESP, the Redis wire protocol.

A command such as `SET name alice` arrives on the socket as:

```
*3\r\n$3\r\nSET\r\n$4\r\nname\r\n$5\r\nalice\r\n
```

`*3` means three arguments follow. `$3` means the next argument is three bytes long. Replies use the same encoding.

**Requirements:**

- Accept and correctly parse RESP commands. `redis-cli` must work against your server.
- Support inline commands. Running `telnet localhost 6379` and typing `SET a b` followed by Enter must work. There is no `*3`/`$3` framing in this case.
- On a malformed command, reply with an error and leave the connection usable for the next command. Document in your README which error classes are recoverable and which force a close.
- Reject an oversized bulk header (for example `$536870912`) before performing any allocation.

**The parser must be a resumable state machine.**

The grading script delivers every command one byte per `write()`, with random delays between bytes.

TCP does not preserve message boundaries. A single `read()` may return a whole command, half a command, or two and a half commands. The common approach — read into a buffer, search for `\r\n`, parse what was found — works during casual testing because messages usually arrive whole, and fails here.

Your parser must retain its position between calls: *"I have consumed `*3`, I am inside argument 2, I require 4 more bytes."* Each byte advances that state.

This decision constrains everything built on top of it. Make it in week one.

---

### Phase 2: Single-Threaded Event Loop

Handle thousands of simultaneous connections on one thread.

**Requirements:**

- Use `epoll` (Linux) or `kqueue` (macOS/BSD). Do not use `select()`. No threads in the request path.
- Maintain per-connection read buffer, write buffer, and an explicit state machine (`READING`, `WRITING`, `DRAINING`, `CLOSING`).
- **Handle partial writes.** A `write()` of a 10 KB reply may accept only 1 KB and return 1024. Retain the remainder and complete it when the socket becomes writable. The grading script sets a small `SO_SNDBUF` so this occurs constantly.
- **Handle slow readers.** A client that issues `KEYS *` against a large dataset and then stops reading will accumulate queued output. Enforce a soft limit (log) and a hard limit (disconnect).
- **Batch syscalls.** Under pipelining, a client sending 100 commands should be serviced with approximately one `read()` and one `write()`, not 100 of each. Target ~2 syscalls per client per loop iteration, verified with `strace -c`.
- Handle `EINTR`, `EAGAIN`, short writes, and `accept()` returning `EMFILE` without busy-looping.

---

### Phase 3: Hashtable and Command Set

Implement your own hashtable. `std::unordered_map`, Rust's `HashMap`, and equivalent library containers are not permitted.

Open addressing or chaining are both acceptable. Two invariants apply regardless.

**Invariant 1 — bounded probes.** No single lookup may examine more than `PROBE_CAP` slots.

A hashtable locates a key by hashing to a slot and, on collision, checking nearby slots. This is normally one or two checks. With adversarial keys it degrades to hundreds, and on a single-threaded server one slow lookup blocks every connection. Track the running maximum and expose it via `DEBUG PROBESTAT`.

**Invariant 2 — incremental resize.** Migrate at most a small fixed number of buckets per command.

Allocating a larger table and moving every entry at once means one `SET` takes hundreds of milliseconds while the other five million take microseconds. Keep both tables live during a resize, migrate a few buckets per command, and consult both until the old table is drained.

The grading script feeds you key sets constructed to collide under common default seeds (zero, the FNV basis constant, SipHash's zero key). These keys are harmless under your own `HASH_SEED`. Collapsing throughput on that set indicates a hardcoded seed.

**Commands required:**

| Category | Commands |
|---|---|
| Strings | `SET` (with `EX`, `PX`, `NX`, `XX`), `GET`, `GETDEL`, `APPEND`, `INCR`, `INCRBY`, `STRLEN`, `MSET`, `MGET` |
| Generic | `DEL`, `EXISTS`, `TYPE`, `RENAME`, `KEYS`, `SCAN`, `TTL`, `PTTL`, `EXPIRE`, `PEXPIRE`, `PERSIST`, `DBSIZE`, `FLUSHDB` |
| Hash | `HSET`, `HGET`, `HDEL`, `HGETALL`, `HINCRBY` |
| List | `LPUSH`, `RPUSH`, `LPOP`, `RPOP`, `LRANGE`, `LLEN` |
| Set | `SADD`, `SREM`, `SMEMBERS`, `SISMEMBER`, `SCARD` |
| Server | `PING`, `ECHO`, `INFO`, `CONFIG GET/SET`, `DEBUG PROBESTAT`, `DEBUG DIGEST` |
| Transactions | `MULTI`, `EXEC`, `DISCARD`, `WATCH` |

`SCAN` carries a specific guarantee: any key present for the entire duration of an iteration must be returned at least once, even if the table resizes mid-iteration. Naive cursors violate this. Determine how Redis increments its cursor and explain in your README why that scheme survives a resize.

---

### Phase 4: Expiry and Memory Limits

#### Expiry

Keys may carry a TTL. An expired key must disappear without anyone reading it.

**Requirements:**

- **On access:** check the TTL before returning a value; if expired, delete it and report the key as missing.
- **In the background:** each loop iteration, sample a number of volatile keys and delete those that have expired. If more than 25% of the sample was expired, repeat. Bound the cycle with a wall-clock deadline so it cannot starve client I/O.
- Use `CLOCK_MONOTONIC`. The grading script steps the system clock backwards by one hour during a TTL-heavy run.

Lazy expiry alone fails: the script inserts one million keys with TTLs drawn from a known distribution and samples `DBSIZE` over time. The resulting decay curve is compared against a reference.

#### Memory limits

**Requirements:**

- Track bytes allocated and freed in your own accounting. Do not poll RSS from `/proc`.
- Support `noeviction`, `allkeys-random`, `allkeys-lru`, `volatile-lru`, `volatile-ttl`.
- Implement approximate LRU: store a small timestamp per key, sample `EVICT_SAMPLE` random candidates on eviction, and remove the oldest of the sample.

Exact LRU via an intrusive linked list is not acceptable. Justify this in your README in terms of bytes per key and pointer-chasing cost.

---

### Phase 5: Write-Ahead Log and Crash Recovery

#### The log

**Requirements:**

- Append every write command to a log file before acknowledging it to the client.
- Each record carries a sequence number and a checksum.
- On startup, replay the log to reconstruct the dataset.
- Support three modes via `CONFIG SET appendfsync`:

| Mode | Behaviour | Loss window |
|---|---|---|
| `always` | fsync before every reply | none |
| `everysec` | fsync once per second | up to 1 second |
| `no` | leave it to the OS | unbounded |

- Implement log compaction: rewrite the log as the minimum command set reproducing the current dataset. Writes arriving during a rewrite must not be lost.

#### Crash recovery

The grading script kills your server with `SIGKILL` at a uniformly random point — frequently mid-write — then restarts it and verifies the data. **One hundred iterations.**

The recovered dataset must equal the state after **some prefix** of the acknowledged commands. Losing the most recent writes is acceptable. Corrupting data, resurrecting a deleted key, or failing to start is not. A half-written trailing record must be detected by checksum and discarded cleanly.

This code path never executes during normal testing, so it is never accidentally correct. The resulting bugs are rare — one appearance in a few hundred runs is typical — so you will need a way to reproduce a specific crash deterministically rather than re-running and hoping. Building that harness is part of the work.

#### Durable rename

If you rewrite the log to a temporary file and `rename()` it into place, `fsync()` on the temporary file is not sufficient.

`fsync` makes the file's *contents* durable. It does not make the *directory entry* durable — the mapping from the name `log.aof` to those contents lives inside the directory, which is itself a file, and that change sits in the page cache. Lose power in that window and the data survives on disk under no name at all.

Determine what additional call is required and where it belongs in the sequence. Write the answer in your README.

Note that your own `SIGKILL` testing will not catch this. `SIGKILL` terminates the process; the pending directory update is held by the kernel, which is still running, so it reaches disk moments later and recovery appears to work. Only a genuine power loss, a hard VM reset, or a tool such as `dm-flakey` exposes it.

---

### Phase 6: Benchmarking

Produce a `BENCH.md` containing four artefacts, all from runs on your own machine:

1. **Throughput and latency** — operations per second and p50, p99, p99.9 at pipeline depths of 1, 10, and 100.
2. **Syscall counts** — `strace -c` output from a pipelined run, supporting your Phase 2 claim.
3. **The durability table** — throughput and worst-case loss window for each of the three `appendfsync` modes.
4. **A flame graph** — with a paragraph identifying what sits at the top and whether it was expected.

Compare against a baseline: real Redis on the same machine, same workload, same client.

These four artefacts must be mutually consistent. A latency table implying 200,000 ops/sec alongside `strace` output showing three million syscalls for one million operations did not come from the same run.

Percentiles must be computed from a histogram, not by sorting every sample. Retaining ten million timestamps to sort at the end allocates roughly 80 MB during the measurement and perturbs the system under test.

---

### Required Deviations from Redis

Your server must implement the following, and they are checked:

| Command | Your behaviour | Real Redis |
|---|---|---|
| `EXPIRE key 0` | error, key survives | deletes the key |
| `PEXPIRE key -1` | deletes, returns `2` | returns `1` |
| `KEYS` | sorted by raw byte order | hash order |
| `TYPE` on missing key | error | returns `none` |
| `INCR` on `"+5"` | succeeds | error |
| `INCR` on `"05"` | error | error |
| `FLUSHDB ASYNC` | unsupported | supported |

---

### `DEBUG DIGEST`

Return a SHA-256 over the entire dataset in a canonical form:

- Keys sorted by raw byte comparison (`memcmp`), never hash order.
- Each field length-prefixed with a 4-byte little-endian length.
- TTLs contribute a single volatile/non-volatile flag byte, never the deadline. Absolute deadlines are not reproducible across a restart.
- Empty containers must not appear — a list with zero remaining elements should already have been deleted.

The grading script compares your digest before and after a restart.

---

### What the Grading Script Does

1. Delivers every command one byte at a time with random delays
2. Sends 50,000 pipelined commands in a single large write
3. Opens 2,000 connections that each send a partial command and stop
4. Uses keys containing NUL bytes, CRLF, empty keys, and keys named after commands
5. Feeds collision sets targeting default hash seeds
6. Steps the system clock forwards and backwards under TTL load
7. Kills and restarts the server 100 times at random points
8. Counts syscalls with `strace -c`
9. Compares `DEBUG DIGEST` before and after restart
10. Sets `ulimit -n 256` and opens 500 connections, checking for graceful degradation rather than an `EMFILE` busy-loop

---

## Bonus Tasks

### Sorted Sets

Implement `ZADD`, `ZSCORE`, `ZRANGE`, `ZRANGEBYSCORE`, `ZRANK`, `ZINCRBY`, `ZPOPMIN` using your own skiplist paired with a hashtable. Explain why a skiplist rather than a balanced tree.

### Replication

Implement `REPLICAOF host port` with a full sync followed by streaming the log, and partial resynchronisation after a brief disconnect using a replication ID and offset. Demonstrate that the replica converges to the master's `DEBUG DIGEST` after a simulated network partition.

### Huge Pages

Back your value arena with 2 MB huge pages via `MAP_HUGETLB` or `madvise(MADV_HUGEPAGE)`. Measure TLB misses before and after with `perf stat -e dTLB-load-misses`. Report the delta at several dataset sizes and identify where it stops mattering.

### Sanitizer Integration

Build and run the full grading workload under AddressSanitizer and UndefinedBehaviorSanitizer. Include the clean output. If any phase cannot run under ASan due to your allocator design, say so and explain why.

### Serverless Deployment

Deploy your server on a serverless or scale-to-zero platform — Google Cloud Run, Fly Machines, AWS App Runner, or equivalent — with a persistent volume attached for your log file.

This will not work cleanly, and documenting exactly how it fails is the deliverable. Answer each of the following with evidence, not speculation:

- **Cold start.** Measure the time from first request to first reply on a cold instance, across dataset sizes of 10 MB, 100 MB, and 1 GB. Your startup cost is dominated by replaying the log. Plot it. At what dataset size does cold start become unacceptable for a cache?
- **Scale to zero.** The platform stops your process when idle. What happens to your in-memory hashtable? What happens to keys whose TTL expires while the process is not running — does your expiry accounting handle a gap in wall-clock time correctly, and does `CLOCK_MONOTONIC` still behave as you assumed?
- **Host migration.** The platform may move your container to a different machine. What happens to your log file? If the volume does not follow, what does recovery do?
- **Concurrency limits.** These platforms scale by running multiple instances. Run two instances against the same volume. Describe precisely what breaks — two processes appending to one log file is a correctness problem, not a performance one.
- **The fundamental tension.** Write a short section explaining why a stateful, single-process, in-memory database and a scale-to-zero runtime are architecturally opposed, and what a serverless-native design (for example Upstash or Momento) does differently to resolve it.

Include deployment configuration, the URL if still live, `docker stats` or platform metrics during a load test, and raw timing output for every claim.

---

## Resources

- *Redis Design and Implementation* — Huangz Zhang (covers the dict, incremental rehashing, and the expiry cycle in detail)
- Redis protocol specification — [redis.io/docs/reference/protocol-spec](https://redis.io/docs/reference/protocol-spec)
- *The Linux Programming Interface* — Michael Kerrisk, chapters on `epoll`, file I/O, and the `fsync` semantics behind Phase 5
- ARIES recovery paper — Mohan et al., for background on write-ahead logging (you are implementing a simplified subset)
- `man 2 epoll_wait`, `man 2 fsync`, `man 7 tcp`
- Brendan Gregg on flame graphs — [brendangregg.com/flamegraphs.html](https://www.brendangregg.com/flamegraphs.html)
- Gil Tene, *How NOT to Measure Latency* — required viewing before writing `BENCH.md`

---

## Submission

Create a **private** GitHub repository containing your solution. The repository must remain private until you are asked to make it public.

- Add mentors as collaborators.
- Include `params.txt` with your derived constants.
- Include a `README.md` explaining your implementation, supported by screenshots and shell logs wherever relevant. Document every failure you encountered and the steps taken to resolve it — particularly for Phase 5, where a log of the crash bugs you found and how you reproduced them is worth more than a clean narrative.
- Include a `DESIGN.md` with at least 8 dated entries. Each entry states a decision you made, the alternative you rejected, and the measurement or reasoning that decided it.
- Include `BENCH.md` with the four artefacts from Phase 6.
- Include a `Makefile` or `CMakeLists.txt` so tests and benchmarks compile with a single command.
- Commit incrementally. A repository whose history consists of three commits of four thousand lines each will be returned without review.

## Mentors:
1. Antony Thaikadavil (Ph. No.: +91 8976086924, GitHub ID: antonyth18)
