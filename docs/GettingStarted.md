# Getting Started with libreglisse

## Downloading the project

* Using `https`: `git clone https://github.com/Wmbat/libreglisse`
* Using `SSH`: `git clone git@github.com:Wmbat/libreglisse.git`

## Building using [build2](https://build2.org/)

### As stand-alone

1. Setting up:
    * `cd libreglisse`
    * `bdep init -C ../libreglisse-gcc-release/ @gcc-release cc config.cxx=g++ config.cc.coptions="-O3"`
2. Installing:
    * `b install config.install.root=${install_dir}`

        Where `${install_dir}` is the installation path of your choosing
3. Running tests:
    * `b test`

### In an existing project

1. Add library to repositories.manifest:
    ```
    :
    role: prerequisite
    location: https://github.com/Wmbat/libreglisse
    ```
2. Add depency to manifest: `depends: libreglisse ^0.1.0`
3. Import library in the buildfile and import as target
    ```
    import libs = libreglisse%lib{reglisse}
    
    exe{example}: {hxx cxx}{**} $libs
    ``` 
