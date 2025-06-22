# correctness/test_set_get.py
from utils.client import RedisClient

def test_set_get():
    client = RedisClient()
    assert client.request("*3\r\n$3\r\nSET\r\n$3\r\nfoo\r\n$3\r\nbar\r\n") == "+OK\r\n"
    assert client.request("*2\r\n$3\r\nGET\r\n$3\r\nfoo\r\n") == "$3\r\nbar\r\n"
    client.close()