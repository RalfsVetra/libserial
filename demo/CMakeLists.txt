cmake_minimum_required(VERSION 3.28)
project(demo)

add_executable(demo main.cc)

include(FetchContent)
FetchContent_Declare(
  libserial
  GIT_REPOSITORY https://github.com/RalfsVetra/libserial.git
  GIT_TAG main)
FetchContent_MakeAvailable(libserial)

target_link_libraries(demo PRIVATE serial)
