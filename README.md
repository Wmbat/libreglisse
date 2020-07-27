# monads

![Linux](https://github.com/Wmbat/monads/workflows/Linux/badge.svg)

Monads is a library to emulate the behaviour of `either` and `maybe` monads. This library was strongly inspired by the
[neither](https://github.com/LoopPerfect/neither) library.

## Table of Content
* [Requirements](#requirements)
* [Dependencies](#dependencies)
* [Installation](#installation)
* [License](#license)

## Requirements

This library currently makes use of C++20 features only available on GCC10 and above.

## Dependencies

* [googletest](https://github.com/google/googletest) For the unit tests.
* [CPM.cmake](https://github.com/TheLartians/CPM.cmake) For downloading packages.

## Installation

#### Local Install
To simply build the library as a standalone, you may clone the repository 
```sh
git clone https://github.com/Wmbat/monads
```

And to build it, you may run the python script `build.py`. the `--help` command will list the possible options supported
by the script
```sh
python build.py --help
```

#### Git-submodule

If your project uses [CMake](https://cmake.org/). You may also add the project as a subdirectory.
```sh
git submodule add https://github.com/Wmbat/monads external_dependencies
```

Then add the following lines in your `CMakelists.txt`
```cmake
add_subdirectory(external_dependencies/monads)
```

and
```cmake
target_link_libraries(your_project PUBLIC monads::monads)
```

#### Using CPM.cmake

If you are not using [CPM.cmake](https://github.com/TheLartians/CPM.cmake) for package managament, you can start using
it by adding the following lines at the top of your cmake 
```cmake
cmake_minimum_required( VERSION 3.14...3.17 FATAL_ERROR )

set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM.cmake")
set(CPM_DOWNLOAD_VERSION 0.27)

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION} AND CPM_VERSION STREQUAL CPM_DOWNLOAD_VERSION))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})
```

along with this command
```cmake
CPMAddPackage(
    NAME monads
    GITHUB_REPOSITORY Wmbat/monads
    GIT_TAG master
    OPTIONS
        "BUILD_TESTS OFF"
)
```

and finally, you will have to link the library against your project
```cmake 
target_link_libraries(your_project PUBLIC monads::monads
```

## License

> You can find the project's license [here](https://github.com/Wmbat/monads/blob/master/LICENSE)

This project is licensed under the terms of the **MIT** license.

