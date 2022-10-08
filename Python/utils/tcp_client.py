import sfpy
import umat


class tcp_client(object):
    def __init__(self):
        self.server_ip = ""
        self.server_port = ""

    def connect(self, ip=None, port=None):
        if ip is not None and port is not None:
            self.server_ip = ip
            self.server_port = port
        else:
            self.find_server()

    def find_server(self):
        a = 1

    def send(self, m):
        a = 1
