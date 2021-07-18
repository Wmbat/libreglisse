# Getting Started with libreglisse

## Building as stand-alone

1. Checkout libreglisse
    * Using `https`: `git clone https://github.com/Wmbat/libreglisse`
    * Using `SSH`: `git clone git@github.com:Wmbat/libreglisse.git`
2. Build libreglisse with [build2](https://build2.org/):
    * `cd libreglisse`
    * `bdep init -C ../libreglisse-gcc-release/ @gcc-release cc config.cxx=g++ config.cc.coptions="-O3"`
    * `b install config.install.root=${install_dir}`
3. Building & running the unit tests
    * `b test`

## Building as a third-party library
