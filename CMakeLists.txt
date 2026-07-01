cmake_minimum_required(VERSION 3.28)
project(libserial CXX)

option(BUILD_TESTS "Build all tests." ON)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

add_subdirectory(src)
