A connection pool is a cache of database connections that can be reused, avoiding enormous cost of opening a new TCP connection, performing TLS handshakes.

Without pool, this what happens:
1. API receives request , opens a connection
2. Runs the query
3. Close the connection

HikariCP is the default connection pool in Spring Boot 2.x. HikariCP achieves both throughput and latency through a custom `ConcurrentBag` bytecode-level proxy generation and a meticulously turned locking strategy.


### The Core Architecture

Every HikariCP pool contains

![[Pasted image 20260812223351.png]]

**ConcurrentBag** - custom, lock-free collection that holds all PoolEntry objects

**PoolEntry** - wrapper around the actual `java.sql.Connection`. 
	Tracks state: **IN_USE, NOT_IN_USE, RESERVED, REMOVED**.

**HouseKeeper** - a background thread that runs every 30s to evict idle connections, enforce *maxLifeTime* and execute *keepalive* queries

### Connection Lifecycle
1. **Pool Startup** - creates minimumIdle connections
2. **Borrowing a connection** - HikariCP first checks a **thread-local handoff list** (fast-path). If a connection was used by the same thread recently, it’s returned immediately without any locking.
	1. If not, it scans the **ConcurrentBag** for **NOT_IN_USE** entries. If found, it marks the entry **IN_USE**.
3. **Returning a connection** - The entry state is set back to **NOT_IN_USE**, and the semaphore releases a waiting thread.
4. **Idle connections** - **HouseKeeper** removes connections idle longer than **idleTimeout**, down to **minimumIdle**.
5. **Max lifetime** — any connection older than **maxLifetime** is retired and replaced transparently.

Example

**`maximumPoolSize: 10`**  
The hard cap on how many connections (idle + active) the pool will ever hold. Requests beyond this wait in a queue.

**`minimumIdle: 10`**
The minimum number of idle connections Hikari tries to keep ready

**`connectionTimeout: 3000`**
How long (in ms) a thread will wait for a connection from the pool before giving up and throwing a `SQLException`. Here, 3 seconds — if all 10 connections are busy and none free up in time, the caller gets an exception.

**`idleTimeout: 600000`**
How long (ms) a connection can sit idle before being retired — 10 minutes. **Note:** this setting only takes effect when `minimumIdle < maximumPoolSize`.

**`maxLifetime: 1800000`**
The maximum lifetime (ms) of any connection, regardless of activity — 30 minutes. Once a connection hits this age, Hikari retires it (after it's returned) and opens a fresh one.

**`leakDetectionThreshold: 15000`**
If a connection is checked out for longer than 15 seconds without being returned, Hikari logs a warning with a stack trace of where it was borrowed.