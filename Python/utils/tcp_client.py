import sfpy
import umat


class tcp_client(object):
    def __init__(self):
        self.server_ip = ""
        self.server_port = 0

    def connect(self, ip=None, port=None):
        if ip is not None and port is not None:
            self.server_ip = ip
            self.server_port = port
            sfpy.tcp_client_connect_server(self.server_ip, self.server_port)
        else:
            self.find_server()

    def find_server(self):
        sfpy.tcp_client_connect_server('', 0)

    def send(self, m):
        buf = m.dump_buf()
        send_size = sfpy.tcp_client_send(buf)
        print('send size:', send_size)

