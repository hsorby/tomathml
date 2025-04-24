cmake_minimum_required(VERSION 3.20)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

# Project name and version
project(ToMathML VERSION 1.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/cmake")

include(FetchContent)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build Unit Tests" On)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

set(BUILD_GMOCK OFF)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
)

FetchContent_MakeAvailable(googletest)

include(environmentchecks)

add_subdirectory(src)

if(BUILD_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()

if(BINDINGS_AVAILABLE)
  add_subdirectory(src/bindings)
endif()
