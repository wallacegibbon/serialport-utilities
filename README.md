## Introduction

Utilities for handling serial port that support multi-platform (with the help of [libserialport](https://www.sigrok.org/wiki/Libserialport)).


## Preparations

On Debian, all you need can be installed from system package manager.

```sh
sudo apt install libserialport-dev libuv1-dev
```

On Windows, you need to install [MSYS2](https://www.msys2.org/), and install MinGW-w64 like this:

```sh
pacman -S mingw-w64-ucrt-x86_64-gcc
```

Then build libserialport from source.


