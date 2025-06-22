# stress/high_concurrency.py
import threading
from utils.client import RedisClient

def spam(client_id):
    client = RedisClient()
    for i in range(1000):
        client.request(f"*3\r\n$3\r\nSET\r\n${len(str(client_id))}\r\n{client_id}\r\n$1\r\nX\r\n")
    client.close()

threads = []
for i in range(100):
    t = threading.Thread(target=spam, args=(f"id{i}",))
    t.start()
    threads.append(t)

for t in threads:
    t.join()
print("Completed concurrent writes.")
