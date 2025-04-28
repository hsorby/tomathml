cmake_minimum_required(VERSION 3.25)

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

set(_OPTIONS BUILD_TESTS BUILD_SHARED_LIBS)
set(_OPTOINS_DOCS "Build unit tests." "Build using shared libraries.")

list(LENGTH _OPTIONS options_len)
list(LENGTH _OPTOINS_DOCS docs_len)

if(NOT options_len EQUAL docs_len)
  message(FATAL_ERROR "VALUES and DOCS lists must have the same length!")
endif()

math(EXPR last_index "${options_len} - 1")
foreach(i RANGE 0 ${last_index})
  list(GET _OPTIONS ${i} value)
  list(GET _OPTOINS_DOCS ${i} doc)

  # Read an environment variable into a CMake variable
  if(DEFINED ENV{${value}})
    set(${value}_DEFAULT "$ENV{${value}}")
  else()
    set(${value}_DEFAULT ON) # or OFF if you prefer
  endif()
  option(${value} "${doc}" ${${value}_DEFAULT})
endforeach()

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  if(DEFINED ENV{CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE_DEFAULT "$ENV{CMAKE_BUILD_TYPE}")
  else()
    set(CMAKE_BUILD_TYPE_DEFAULT Release)
  endif()
  message(STATUS "Setting build type to '${CMAKE_BUILD_TYPE_DEFAULT}' as none was specified.")
  set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE_DEFAULT} CACHE STRING "Choose the type of build." FORCE)

  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

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
