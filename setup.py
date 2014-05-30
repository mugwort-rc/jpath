# -*- coding: utf-8 -*-
"""
jpath
========

|travis-ci|_

.. |travis-ci| image:: https://travis-ci.org/mugwort-rc/jpath.png
.. _travis-ci: https://travis-ci.org/mugwort-rc/jpath

Basic Usage:

    import jpath

    parser = jpath.parse('"path"/"to"/"resource"')

"""

import os

from distutils.core import setup
from distutils.command import build_ext

from distutils.core import Extension

include_dirs = []
libraries = ['boost_python', 'stdc++']
library_dirs = []
sources = ['src/jpath.cpp', 'src/python.cpp']

jpath = Extension(name = 'jpath.__jpath',
                  include_dirs = include_dirs,
                  libraries = libraries,
                  library_dirs = library_dirs,
                  sources = sources,
                  extra_compile_args = ['-std=c++0x'])

# distutils c++ bug fix.
from distutils.sysconfig import get_config_vars
(opt,) = get_config_vars('OPT')
os.environ['OPT'] = " ".join([flag for flag in opt.split() if flag != '-Wstrict-prototypes'])

# ccache
for path in os.environ['PATH'].split(os.pathsep):
    path = path.strip('"')
    exe = os.path.join(path, 'ccache')
    if os.path.isfile(exe) and os.access(exe, os.X_OK):
        os.environ['CC'] = 'ccache gcc'
        break

setup(name = 'jpath',
      long_description = __doc__,
      version = '0.0.1',
      description = 'jpath: JsonPath parser',
      author = 'mugwort_rc',
      author_email = 'mugwort rc at gmail com',
      url = 'https://github.com/mugwort-rc/jpath',
      classifiers = [
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: C++',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2.7',
      ],
      license = 'Apache Software License, Version 2.0',
      packages = ['jpath'],

      ext_modules = [jpath])

