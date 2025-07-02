import threading
import time
import traceback
from utils.client import RedisClient

N_THREADS = 16
COMMANDS_PER_THREAD = 50
latencies = []
lock = threading.Lock()
log_lock = threading.Lock()

LOG_FILE = "test_log.txt"

def log(message):
    timestamp = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    full_message = f"[{timestamp}] {message}\n"
    with log_lock:
        with open(LOG_FILE, "a") as f:
            f.write(full_message)

def worker(thread_id):
    try:
        client = RedisClient()
        log(f"[Thread-{thread_id}] Connected to Redis server")
        local_latencies = []

        for i in range(COMMANDS_PER_THREAD):
            key = f"key{thread_id}_{i}"
            list_key = f"list{thread_id}_{i}"
            hash_key = f"hash{thread_id}_{i}"

            command = ""
            if i % 6 == 0:
                command = f"*3\r\n$3\r\nSET\r\n${len(key)}\r\n{key}\r\n$5\r\nvalue\r\n"
            elif i % 6 == 1:
                command = f"*2\r\n$3\r\nGET\r\n${len(key)}\r\n{key}\r\n"
            elif i % 6 == 2:
                command = f"*3\r\n$5\r\nLPUSH\r\n${len(list_key)}\r\n{list_key}\r\n$5\r\nvalue\r\n"
            elif i % 6 == 3:
                command = f"*4\r\n$6\r\nLRANGE\r\n${len(list_key)}\r\n{list_key}\r\n$1\r\n0\r\n$2\r\n10\r\n"
            elif i % 6 == 4:
                command = f"*4\r\n$4\r\nHSET\r\n${len(hash_key)}\r\n{hash_key}\r\n$5\r\nfield\r\n$5\r\nvalue\r\n"
            elif i % 6 == 5:
                command = f"*3\r\n$4\r\nHGET\r\n${len(hash_key)}\r\n{hash_key}\r\n$5\r\nfield\r\n"

            start = time.time()
            try:
                log(f"[Thread-{thread_id}] Sending command {i}: {repr(command.strip())}")
                response = client.request(command)
                time.sleep(0.001)
                end = time.time()
                local_latencies.append((end - start) * 1e6)  # microseconds
                log(f"[Thread-{thread_id}] Received response: {repr(response.strip())}")
            except Exception as e:
                log(f"[Thread-{thread_id}] Exception on command {i}: {str(e)}")
                log(traceback.format_exc())

        client.close()
        log(f"[Thread-{thread_id}] Closed connection")
        with lock:
            latencies.extend(local_latencies)

    except Exception as e:
        log(f"[Thread-{thread_id}] Fatal thread error: {str(e)}")
        log(traceback.format_exc())

# Clear old log
with open(LOG_FILE, "w") as f:
    f.write("==== Redis Concurrent Test Log ====\n")

threads = []
for t in range(N_THREADS):
    th = threading.Thread(target=worker, args=(t,))
    th.start()
    threads.append(th)

for th in threads:
    th.join()

print(f"Total requests: {N_THREADS * COMMANDS_PER_THREAD}")
print(f"Min: {min(latencies):.2f}μs")
print(f"Max: {max(latencies):.2f}μs")
print(f"Avg: {sum(latencies)/len(latencies):.2f}μs")
