import threading
import time
from utils.client import RedisClient

N_THREADS = 16
COMMANDS_PER_THREAD = 50
latencies = []
lock = threading.Lock()

def worker(thread_id):
    client = RedisClient()
    local_latencies = []
    for i in range(COMMANDS_PER_THREAD):
        # Alternate between SET, GET, LPUSH, LRANGE, HSET, HGET
        key = f"key{thread_id}_{i}"
        start = time.time()
        if i % 6 == 0:
            client.request(f"*3\r\n$3\r\nSET\r\n${len(key)}\r\n{key}\r\n$5\r\nvalue\r\n")
        elif i % 6 == 1:
            client.request(f"*2\r\n$3\r\nGET\r\n${len(key)}\r\n{key}\r\n")
        elif i % 6 == 2:
            client.request(f"*3\r\n$5\r\nLPUSH\r\n$5\r\nlist1\r\n$5\r\nvalue\r\n")
        elif i % 6 == 3:
            client.request(f"*4\r\n$6\r\nLRANGE\r\n$5\r\nlist1\r\n$1\r\n0\r\n$2\r\n10\r\n")
        elif i % 6 == 4:
            client.request(f"*4\r\n$4\r\nHSET\r\n$5\r\nhash1\r\n$5\r\nfield\r\n$5\r\nvalue\r\n")
        elif i % 6 == 5:
            client.request(f"*3\r\n$4\r\nHGET\r\n$5\r\nhash1\r\n$5\r\nfield\r\n")
        end = time.time()
        local_latencies.append((end - start) * 1e6)  # microseconds
    client.close()
    with lock:
        latencies.extend(local_latencies)

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