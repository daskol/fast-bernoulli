#   encoding: utf8
#   filename: setup.py

from setuptools import find_packages, setup


setup(name='fast-bernoulli',
      author='Daniel Bershatsky',
      author_email='daniel.bershatsky@skolkovotech.ru',
      url='https://github.com/daskol/fast-bernoulli',
      license='BSD-3-Clause',
      packages=find_packages(),
      py_modules=['bernoulli'],
      install_requires=[
          'llvmlite',
      ])
