# CMake project initialization

cmake_minimum_required( VERSION 3.14...3.17 FATAL_ERROR )

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Download CPM.cmake

set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM.cmake")
set(CPM_DOWNLOAD_VERSION 0.27)
set(FIND_GCOV_DOWNLOAD_LOCATION "${CMAKE_CURRENT_LIST_DIR}/cmake/FindGcov.cmake")
set(FIND_LCOV_DOWNLOAD_LOCATION "${CMAKE_CURRENT_LIST_DIR}/cmake/FindLcov.cmake")
set(FIND_CODECOV_DOWNLOAD_LOCATION "${CMAKE_CURRENT_LIST_DIR}/cmake/Findcodecov.cmake")
set(FIND_LLVM_COV_DOWNLOAD_LOCATION "${CMAKE_CURRENT_LIST_DIR}/cmake/llvm-cov-wrapper")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION} AND CPM_VERSION STREQUAL CPM_DOWNLOAD_VERSION))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

file(DOWNLOAD https://github.com/RWTH-HPC/CMake-codecov/blob/master/cmake/FindGcov.cmake ${FIND_GCOV_DOWNLOAD_LOCATION})
file(DOWNLOAD https://github.com/RWTH-HPC/CMake-codecov/blob/master/cmake/FindLcov.cmake ${FIND_LCOV_DOWNLOAD_LOCATION})
file(DOWNLOAD https://github.com/RWTH-HPC/CMake-codecov/blob/master/cmake/Findcodecov.cmake ${FIND_CODECOV_DOWNLOAD_LOCATION})
file(DOWNLOAD https://github.com/RWTH-HPC/CMake-codecov/blob/master/cmake/llvm-cov-wrapper.cmake ${FIND_LLVM_COV_DOWNLOAD_LOCATION})

include(${CPM_DOWNLOAD_LOCATION})

# Set the project language toolchain, version and description

project(monads
    VERSION 0.0.1
    DESCRIPTION "A small library containing C++ monad-like classes"
    LANGUAGES CXX
)

# User interface declarations

option(BUILD_TESTS "Build unit tests" ON)
option(ENABLE_COVERAGE "Enable coverage" OFF)

message(STATUS "[${PROJECT_NAME}] Compiling with ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "[${PROJECT_NAME}] ${PROJECT_VERSION}")

if (CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "[${PROJECT_NAME}] Build type: Release")
else ()
    message(STATUS "[${PROJECT_NAME}] Build type: Debug")
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake")

message(STATUS "[${PROJECT_NAME}] Building unit tests: ${BUILD_TESTS}")

if (ENABLE_COVERAGE)
    find_package(codecov)

    set(TEST_ENABLE_COVERAGE ON)
endif ()

if (BUILD_TESTS) 
    enable_testing( )

    add_subdirectory(tests)
endif ()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

target_include_directories(${PROJECT_NAME}
    INTERFACE 
        $<INSTALL_INTERFACE:include>    
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
)
