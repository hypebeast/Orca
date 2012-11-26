#!/usr/bin/env python

import os
from glob import glob
from distutils.core import setup
import py2exe

packages = []
data_files = [("Microsoft.VC90.CRT", glob(r'data\*.*'))]
print data_files
matunus_dir = "src"

for dirpath, dirnames, filenames in os.walk(matunus_dir):
	pass

setup(name='Matunus',
      version='0.1.0',
      description='Orca Ground Control System',
      author='Sebastian Ruml',
      author_email='sebastian.ruml@gmail.com',
      url='https://github.com/hypebeast/Orca',
      console=['matunus.py'],
      data_files=data_files
     )