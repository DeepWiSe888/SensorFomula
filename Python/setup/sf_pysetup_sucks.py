from distutils.core import setup, Extension

#cpp_args = ['-std=c++11 -stdlib=stdc++11']
cpp_args = ['-std=c++11']
head_dirs=['../../BaseLibs/UniversalData',
           '../../BaseLibs/BaseLibs/BLAS',
           '../../BaseLibs/BaseLibs/Utils',
           '/usr/local/include']


cpp_files=['sf_pysetup.cpp',
           '../../BaseLibs/UniversalData/UniversalData.cpp',
           '../../BaseLibs/UniversalData/BaseDataDefine.cpp',
           '../../BaseLibs/Utils/UniversalTcp.cpp',
           '../../BaseLibs/Utils/UniversalSocket.cpp',
           '../../BaseLibs/Utils/UniversalThread.cpp',
           #'../../BaseLibs/BLAS/fourier.cpp',
           #'../../BaseLibs/BLAS/filter.cpp',
           #'../../BaseLibs/BLAS/blas_clang.c'
           ]

sfmodule_cpp = Extension('sfpy',
                         #sources = ['sf_pysetup.cpp'],
                         sources = cpp_files,
                         include_dirs=head_dirs,
                         library_dirs = [ 'usr/local/lib/' ],
                         libraries=['pthread'],
                         language='c++',
                         #define_macros=[('__cplusplus', None)],
                         extra_compile_args=cpp_args)#,
                         #extra_link_args=['-std=c++11'])
'''
sfmodule_clang = Extension('sfpy',
                           sources = c_files,
                           include_dirs=head_dirs,
                           library_dirs = [ 'usr/local/lib/' ],
                           libraries=['pthread', 'fftw3'],
                           language='c')
'''

setup (name = 'sfpy',
       version = '1.0.1',
       description = 'Sensor Formula Python Interfaces',
       ext_modules = [sfmodule_cpp])