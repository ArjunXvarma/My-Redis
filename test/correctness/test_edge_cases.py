import socket
import pytest

HOST = "127.0.0.1"
PORT = 8080

def send_command(command):
    with socket.create_connection((HOST, PORT), timeout=2) as sock:
        sock.sendall(command.encode())
        response = sock.recv(2048).decode()
        return response

def test_empty_command():
    resp = send_command("\r\n")
    assert resp.startswith("-Invalid") or resp == ""

def test_unknown_command():
    resp = send_command("*1\r\n$7\r\nFOOBAR\r\n")
    assert resp.startswith("-Invalid")

def test_wrong_number_of_args():
    resp = send_command("*1\r\n$3\r\nGET\r\n")
    assert '-ERR' in resp
    resp2 = send_command("*2\r\n$3\r\nSET\r\n$4\r\nkey1\r\n")
    assert '-ERR' in resp2

def test_large_bulk_string():
    large_value = "x" * 10000
    cmd = f"*3\r\n$3\r\nSET\r\n$4\r\nkeyL\r\n${len(large_value)}\r\n{large_value}\r\n"
    resp = send_command(cmd)
    print(resp)
    assert resp.startswith("+OK")
    get_resp = send_command("*2\r\n$3\r\nGET\r\n$4\r\nkeyL\r\n")
    print(get_resp)
    assert large_value in get_resp

def test_nonexistent_key():
    resp = send_command("*2\r\n$3\r\nGET\r\n$7\r\nno_such\r\n")
    print(resp)
    assert resp.strip() == '$5\r\n$-1'

def test_case_insensitivity():
    resp = send_command("*3\r\n$3\r\nset\r\n$5\r\nkCaSe\r\n$5\r\nVaLuE\r\n")
    assert "+OK" in resp
    resp2 = send_command("*2\r\n$3\r\nget\r\n$5\r\nkCaSe\r\n")
    assert "VaLuE" in resp2

def test_binary_safe():
    # Send a value with null bytes
    value = "abc\x00def"
    cmd = f"*3\r\n$3\r\nSET\r\n$4\r\nbink\r\n$7\r\n{value}\r\n"
    resp = send_command(cmd)
    assert "+OK" in resp
    get_resp = send_command("*2\r\n$3\r\nGET\r\n$4\r\nbink\r\n")
    # The response should include the correct length
    assert "$7" in get_resp

# def test_multiple_commands_in_one_packet():
#     cmd = (
#         "*3\r\n$3\r\nSET\r\n$4\r\nmkey\r\n$5\r\nmval1\r\n",
#         "*2\r\n$3\r\nGET\r\n$4\r\nmkey\r\n"
#     )
#     resp = send_command(cmd)
#     # Should get two responses
#     assert "+OK" in resp and "$5" in resp and "mval1" in resp

def test_empty_bulk_string():
    cmd = "*3\r\n$3\r\nSET\r\n$4\r\nempty\r\n$0\r\n\r\n"
    resp = send_command(cmd)
    print(resp)
    assert resp.startswith("+OK")
    get_resp = send_command("*2\r\n$3\r\nGET\r\n$5\r\nempty\r\n")
    assert "$0" in get_resp

def test_list_out_of_range():
    send_command("*3\r\n$5\r\nLPUSH\r\n$5\r\nalist\r\n$1\r\na\r\n")
    resp = send_command("*4\r\n$6\r\nLRANGE\r\n$5\r\nalist\r\n$2\r\n$5\r\n")
    # Should return empty array
    assert resp.strip() == "*0"

def test_set_wrong_type():
    send_command("*3\r\n$3\r\nSET\r\n$4\r\ntype\r\n$1\r\nx\r\n")
    resp = send_command("*3\r\n$5\r\nSADD\r\n$4\r\ntype\r\n$1\r\ny\r\n")
    assert resp.startswith("-Invalid")