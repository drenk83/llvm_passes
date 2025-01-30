cmake_minimum_required(VERSION 3.13.4)
project(uafml)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LLVM_INSTALL_DIR "/usr/lib/llvm-14" CACHE PATH "/usr/lib/llvm-14" FORCE)
set(LLVM_INSTALL_INCLUDE "${LLVM_INSTALL_DIR}/include/llvm")

set(LLVM_INSTALL_CMAKE "${LLVM_INSTALL_DIR}/lib/cmake/llvm/LLVMConfig.cmake")
list(APPEND CMAKE_PREFIX_PATH "${LLVM_INSTALL_DIR}/lib/cmake/llvm/")

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

set(CMAKE_CXX_STANDARD 17 CACHE STRING "")

if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

add_subdirectory(lib)
