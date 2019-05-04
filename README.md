[![Codacy Badge](https://api.codacy.com/project/badge/Grade/427898c3a41a47a0b5d5d87539227837)](https://www.codacy.com/app/kacprzak/nbd-3dge?utm_source=github.com&utm_medium=referral&utm_content=kacprzak/nbd-3dge&utm_campaign=badger)
[![Build Status](https://travis-ci.org/kacprzak/nbd-3dge.svg?branch=master)](https://travis-ci.org/kacprzak/nbd-3dge)
[![Build status](https://ci.appveyor.com/api/projects/status/hxr95cc8py7m2blp?svg=true)](https://ci.appveyor.com/project/kacprzak/nbd-3dge)

nbd-3dge
========

No Big Deal 3D Game Engine 

This project is created to have fun while experimenting with modern OpenGL
and C++.

Build
-----

```shell
mkdir build
cd build
conan install .. --build=missing
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
