# performance/benchmark_latency.py
import time
from utils.client import RedisClient

N = 1000
latencies = []

client = RedisClient()

for _ in range(N):
    start = time.time()
    client.request("*2\r\n$4\r\nPING\r\n$1\r\nX\r\n")
    end = time.time()
    latencies.append((end - start) * 1e6)  # microseconds

client.close()

print(f"Min: {min(latencies):.2f}μs")
print(f"Max: {max(latencies):.2f}μs")
print(f"Avg: {sum(latencies)/N:.2f}μs")
