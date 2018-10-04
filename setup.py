from Cython.Distutils import build_ext
from Cython.Build import cythonize
from distutils.core import setup, Extension

import numpy
import subprocess

 
opencv_libs = subprocess.check_output("pkg-config --libs opencv".split()).decode('utf-8')
opencv_libs = [lib for lib in opencv_libs.split()]

opencv_libdirs = [s[2:].strip() for s in subprocess.check_output("pkg-config --libs-only-L opencv".split()).decode('utf-8').split(' ')]

opencv_headers = subprocess.check_output("pkg-config --cflags-only-I opencv".split()).decode('utf-8')
opencv_headers = [header[2:] for header in opencv_headers.split()]

def link_args(*libs, **kwargs):
    _link_all = kwargs.get('link_all', True)
    if _link_all:
        link_args = {'extra_link_args': opencv_libs}
    else:
        link_args = {'library_dirs': opencv_libdirs,
                     'libraries': list(libs)}
    return link_args

inc_dirs = [numpy.get_include()]
inc_dirs.extend(opencv_headers)


ext_modules = [

	Extension('stitchup.lib.cvt', ['stitchup/lib/cvt.pyx'],
		language = 'c++',
		extra_compile_args = ['-std=c++11'],
        include_dirs=inc_dirs,
        **link_args('opencv_core', link_all=False)),

	Extension('stitchup.lib.stitch', ['stitchup/lib/stitch.pyx', 'stitchup/lib/src/simplestitcher.cpp', 'stitchup/lib/src/stitcher2.cpp'],
		language = 'c++',
		extra_compile_args = ['-std=c++11'],
        include_dirs=inc_dirs,
        **link_args(link_all=True)),

]

setup(
    name='stitchup',
    description='Stitch multiple images into one panorama mosaic',
    version='0.1.0',
    author='John Stowers',
    author_email='john.stowers@gmail.com',
    cmdclass = {'build_ext': build_ext},
    ext_modules = ext_modules,
)
