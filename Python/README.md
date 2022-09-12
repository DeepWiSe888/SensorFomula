# How to run sfpy  _[Sensor Formula Python Interfaces]_  

  
  
_It is strong recommended installing sfpy in virtual env._  
  
## 1. Build & install  
    (venv) $ cd setup  
    (venv) $ python3 sf_pysetup_sucks.py build install  
_C/C++ mixed compiling was not worked on my laptop(apple m1)，so I had to use a sucks temporary .py script to compile and link c/c++ files.  
Normally, the libsfbase.a library should be linked, not the source file ._  

## 2. Run TCP server
    (venv) $ python3  
    >>> import sfpy

    >>> def on_tcp_data(data):
    >>>   print('call back from c')
    >>>   print(data)

    >>> sfpy.version()
    >>> sfpy.set_ondata(on_tcp_data) 
    >>> sfpy.tcp_server(12345)

## 3. Examples  
see test.py

## 4. Modify or rewrite your own python interfaces  
It is not recommended using dlopen at py site to run the so library because it is less readable and lower efficient.  

___Python /C API reference:___
https://docs.python.org/3/extending/extending.html#calling-python-functions-from-c

