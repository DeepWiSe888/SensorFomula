import sfpy

def on_tcp_data(data):
    print('call back from c')
    print(data)

sfpy.version()
sfpy.set_ondata(on_tcp_data)
