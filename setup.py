# -*- coding: utf-8 -*-

from distutils.core import setup
from distutils.extension import Extension

woff = Extension("webfonts_lib/woff",
                 sources=["webfonts_lib/woff.c",
                          "webfonts_lib/woff/woff.c"])

eot = Extension("webfonts_lib/eot",
                sources=["webfonts_lib/eot.cpp",
                         "webfonts_lib/eot/OpenTypeUtilities.cpp"])

setup(
    name='webfonts',
    description='webfonts encoding utility',
    version='0.1',
    author='Viktor Kotseruba',
    author_email='barbuzaster@gmail.com',
    license='Apache / Mozilla / GNU GPL v2',
    keywords='web',
    ext_modules=[woff, eot],
    scripts=["webfonts"],
    packages=["webfonts_lib"],
    package_dir={
        "webfonts_lib": "webfonts_lib",
    },
    package_data={
        "webfonts_lib": ["*.jar", "lib/*.jar"],
    },
)
