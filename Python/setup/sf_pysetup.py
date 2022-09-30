from distutils.core import setup, Extension

cpp_args = ['-std=c++11']
sf_lib_path = '/Users/link/Projects/MCU/SensorFomula/BaseLibs/lib/build/libsfbase.a'
sf_lib_path2 = '/Users/link/Projects/MCU/SensorFomula/BaseLibs/lib/build/libsfbasec.a'


sfmodule = Extension('utils',
                    sources = ['sf_pysetup.cpp'],
                    libraries=[sf_lib_path, sf_lib_path2, 'pthread'],
                    language='c++',
                    extra_compile_args=cpp_args,)
setup (name = 'utils',
       version = '1.0',
       description = 'Sensor Formula Python Interfaces',
       ext_modules = [sfmodule])