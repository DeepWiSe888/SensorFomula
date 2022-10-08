import umat
import tcp_server
import numpy as np


def test_load_file():
    m = umat.load_file('/Users/link/Projects/MCU/SensorFomula/Python/utils/a_umat.dat')
    buf = m.dump_buf()
    f = open('/Users/link/Projects/MCU/SensorFomula/Python/utils/a_umat2.dat', 'wb')
    f.write(buf)
    f.close()
    m2 = umat.load_file('/Users/link/Projects/MCU/SensorFomula/Python/utils/a_umat2.dat')
    diff = m.data - m2.data
    diff_sum = np.abs(np.sum(diff))
    print('diff=', diff_sum)


def test_tcp_server():
    a = 1


if __name__ == '__main__':
    test_load_file()
    # test_tcp_server()
