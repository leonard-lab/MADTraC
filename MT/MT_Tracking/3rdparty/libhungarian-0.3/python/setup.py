#!/usr/bin/env python

from distutils.core import setup, Extension

module = Extension('hungarian', 
                    include_dirs = ['..'],
                    libraries = ['hungarian'],
                    library_dirs = ['..'],
                    sources = ['pyhungarian.c'])

setup(name="hungarian",
      version="0.2", # Let the bindings and the lib be the same version number
      description="Python bindings for libhungarian",
      author="Dylan Shell",
      author_email="dshell@robotics.usc.edu",
      url="http://robotics.usc.edu/~dshell/",
      ext_modules = [module]
     )

