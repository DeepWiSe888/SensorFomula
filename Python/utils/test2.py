import umat
import tcp_server
import tcp_client
import numpy as np
import time


def test_umat_rw():
    m1 = umat.load_file('/Users/link/Projects/MCU/SensorFomula/Python/utils/a_umat.dat')
    m2 = umat.load_file('/Users/link/Projects/MCU/SensorFomula/Python/utils/a_umat3.dat')
    diff = m1.data - m2.data
    diff_sum = np.abs(np.sum(diff))
    print('diff=', diff_sum)


def test_load_file():
    m = umat.load_file('/Users/link/Projects/MCU/SensorFomula/Python/utils/a_umat.dat')
    return m
    # buf = m.dump_buf()
    # f = open('/Users/link/Projects/MCU/SensorFomula/Python/utils/a_umat2.dat', 'wb')
    # f.write(buf)
    # f.close()
    # m2 = umat.load_file('/Users/link/Projects/MCU/SensorFomula/Python/utils/a_umat2.dat')
    # diff = m.data - m2.data
    # diff_sum = np.abs(np.sum(diff))
    # print('diff=', diff_sum)


def on_recv_data(channel_id, data):
    print('get frame data from channel {} size {}'.format(channel_id, len(data)))
    try:
        m = umat.umat()
        m.load_buf(data)
        print('recv data frame. frame size=', m.dims)
        # f = open('/Users/link/Projects/MCU/SensorFomula/Python/utils/a_umat3.dat', 'wb')
        # f.write(data)
        # f.close()
        # return m
    except Exception as e:
        print("parse_frame_data exception:", e)
        # return None


def create_tcp_server():
    port = 11223
    srv = tcp_server.create_server(port)
    srv.run(on_recv_data)


def create_tcp_client():
    cli = tcp_client.tcp_client()
    cli.connect()
    m = test_load_file()
    cli.send(m)


if __name__ == '__main__':
    # test_umat_rw()
    create_tcp_server()
    create_tcp_client()
    while True:
        time.sleep(1)
