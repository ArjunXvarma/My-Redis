# utils/client.py
import socket

class RedisClient:
    def __init__(self, host='127.0.0.1', port=8080):
        self.sock = socket.create_connection((host, port))
    
    def send(self, command: str):
        self.sock.sendall(command.encode())

    def receive(self):
        return self.sock.recv(4096).decode()

    def request(self, command: str):
        self.send(command)
        return self.receive()

    def close(self):
        self.sock.close()