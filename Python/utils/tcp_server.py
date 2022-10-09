import sfpy
import umat
import time
import struct


def on_tcp_data_default(channel_id, data):
    print('get frame data from channel {}'.format(channel_id))
    try:
        m = umat.umat()
        m.load_buf(data)
        print('recv data frame.size=', len(data))
        return m
    except Exception as e:
        print("parse_frame_data exception:", e)
        return None


class tcp_server(object):
    def __init__(self, port):
        self.port = port
        # sfpy.set_ondata(on_tcp_data)

    # on_data_cb is like 'on_tcp_data(...)' above
    def run(self, on_data_cb):
        if on_data_cb is not None:
            sfpy.set_ondata(on_data_cb)
        else:
            sfpy.set_ondata(on_tcp_data_default)
        sfpy.tcp_server(self.port)

    def stop(self):
        a = 1


def create_server(port):
    srv = tcp_server(port)
    return srv


def simulate_radar_test(ntx, nrx, bins, fps):
    a = 1
