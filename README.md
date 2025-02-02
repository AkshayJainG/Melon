<p align="center"><img width="210" src="https://github.com/Water-Melon/Melon/blob/master/docs/logo.png?raw=true" alt="Melon logo"></p>
<p align="center"><img src="https://img.shields.io/github/license/Water-Melon/Melang" /></p>
<h1 align="center">Melon</h1>



Melon is a C framework library for simplifying-development. 
It includes many data structures, algorithms, architectures and many other useful components.
You can pick some components or whole framework based on your demand.
Melon supports both multi-process and multi-thread models with module development.

On Windows, framework can NOT be activated, but other components still working.

### Installation

On Windows, please install `mingw`, `git bash` and `make` at first. Install [MingW-W64-builds](https://www.mingw-w64.org/downloads/#mingw-builds) with the installation settings:

- `Version`: `8.1.0`

- `Architecture`: `i686`

- `Threads`: `posix`

- `Exception`: `dwarf`

- `Build revision`: `0`

Then execute these shell commands on git bash or terminal (on UNIX).

```
git clone https://github.com/Water-Melon/Melon.git
cd Melon
./configure [--prefix=LIB_INSTALL_PATH]
make
make install
```

### License

[BSD-3-Clause License](https://github.com/Water-Melon/Melang/blob/master/LICENSE)

Copyright (c) 2014-present, Niklaus F. Schen



### Document

Please visit [Melon Developer Guide](https://water-melon.github.io/Melon/) for more details.

中文文档请参考：[中文指南](https://water-melon.github.io/Melon/cn/)

