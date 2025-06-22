import socket
import pytest

HOST = "127.0.0.1"
PORT = 8080

def send_command(command):
    with socket.create_connection((HOST, PORT), timeout=2) as sock:
        sock.sendall(command.encode())
        response = sock.recv(1024).decode()
        return response

def test_ping_simple():
    resp = send_command("*1\r\n$4\r\nPING\r\n")
    assert resp.strip() == "+PONG"

def test_ping_with_message():
    resp = send_command("*2\r\n$4\r\nPING\r\n$5\r\nhello\r\n")
    assert resp.strip() == "+hello"

def test_ping_case_insensitive():
    resp = send_command("*1\r\n$4\r\nping\r\n")
    assert resp.strip() == "+PONG"

def test_ping_extra_args():
    resp = send_command("*3\r\n$4\r\nPING\r\n$5\r\nhello\r\n$5\r\nworld\r\n")
    assert resp.startswith("-ERR")