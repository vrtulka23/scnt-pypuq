cmake_minimum_required(VERSION 3.22)
cmake_policy(SET CMP0074 NEW)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-deprecated -O3")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(MODULE_NAME pypuq)
#set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
set(CMAKE_INSTALL_BINDIR bin)
set(CMAKE_INSTALL_LIBDIR lib)
set(CMAKE_INSTALL_INCLUDEDIR include)

project(pypuq VERSION 1.0 LANGUAGES CXX)

# compile libraries and executables
###################################

message("Code version: ${CODE_VERSION}")

find_package(SCNT-PUQ REQUIRED)

# create pydipl module for Python
#################################

find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
include_directories(${Python3_INCLUDE_DIRS})
find_package(pybind11 CONFIG)

pybind11_add_module(${MODULE_NAME} src/pypuq.cpp)
target_link_libraries(${MODULE_NAME} PRIVATE puq-cpp)
target_compile_definitions(${MODULE_NAME} PRIVATE CODE_VERSION="${CODE_VERSION}")

