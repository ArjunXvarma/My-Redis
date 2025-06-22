# performance/memory_cpu_monitor.py
import psutil
import time

pid = int(input("Enter PID of Redis Clone: "))
proc = psutil.Process(pid)

for _ in range(10):
    mem = proc.memory_info().rss / (1024 * 1024)  # MB
    cpu = proc.cpu_percent(interval=1)
    print(f"Memory: {mem:.2f}MB, CPU: {cpu:.2f}%")
