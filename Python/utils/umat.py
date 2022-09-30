import struct
#import sfpy
import numpy as np


class umat(object):
    def __init__(self):
        self.RADAR = 1
        self.LIDAR = 2
        self.THERMAL = 4
        self.CAMERA = 8
        self.sensor_type = 1
        self.data_type = 0
        self.timestamp = 0
        self.fps = 0
        self.dims = None  # np array
        self.data = None  # np array

    # def __init__(self, buf_bytes=None):
    #    self.RADAR = 1
    #    self.LIDAR = 2
    #    self.THERMAL = 4
    #    self.CAMERA = 8
    #    self.load_buf(self, buf_bytes)

    # note : buf_bytes contains a whole frame packet, no need seeking for frame head.
    def load_buf(self, buf_bytes):
        # title size 64: char: '#sf01#umat#....'
        # label size 20: ver(i8), sensor_type(i8), data_type(i8), floatsize(i8), dims(i32*4)
        self.sensor_type, = struct.unpack("B", buf_bytes[65:66])
        read_dims = struct.unpack("4i", buf_bytes[68:68+4*4])
        fdatacnt = 2
        self.dims = np.array(read_dims)
        self.dims[self.dims == 0] = 1
        for a in self.dims:
            fdatacnt = fdatacnt*a
        # timestamp size 8 : secs(i32), milli-sec(i32)
        self.timestamp, = struct.unpack("i", buf_bytes[84:88])
        # data : complex array
        fdata = struct.unpack("{}f".format(fdatacnt), buf_bytes[92:])
        f = np.array(fdata)
        iq = f[0::2] + 1j*f[1::2]
        iq_dims = iq.reshape(self.dims)
        iq_squeeze = iq_dims.squeeze()
        self.data = iq_squeeze

    def dump_buf(self):
        buf = self.data
        return buf

    def create_mimo(self, ntx, nrx, bins, fps):
        self.dims = [ntx, nrx, bins]
        self.data = None

    def create_siso(self, bins, fps):
        self.dims = [bins]
        self.data = None


def load_file(filename):
    f = open(filename, 'rb')
    buf = f.read()
    m = umat()
    m.load_buf(buf)
    f.close()
    return m
