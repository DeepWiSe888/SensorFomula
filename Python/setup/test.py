import sfpy
import time
import struct


def parse_frame_data(data):
    head_size = 92
    bincnt = int((len(data)-head_size)/4/2)  # float iq
    print("bin cnt = {}".format(bincnt))
    float_buf = data[head_size:]
    offset = 0
    frame_iq =  [ complex(0, 0) for i in range(bincnt) ]
    for i in range(bincnt):
        real_float, = struct.unpack("f", float_buf[offset:offset+4])
        imag_float, = struct.unpack("f", float_buf[offset+4:offset+8])
        frame_iq[i] = complex(real_float, imag_float)
    print(frame_iq)


def on_tcp_data(channel_id, data):
    print('get frame data from channel {}'.format(channel_id))
    try:
        parse_frame_data(data)
    except Exception as e:
        print("parse_frame_data exception:", e)


sfpy.set_ondata(on_tcp_data)
sfpy.version()
sfpy.tcp_server(15646)
#sfpy.create_simulate_data_channel(5, 96)
while True:
    time.sleep(1)

