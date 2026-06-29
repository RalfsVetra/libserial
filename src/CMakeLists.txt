add_library(serial)

target_compile_features(serial PUBLIC cxx_std_20)

target_sources(serial
  PRIVATE
    serial.cc
  PUBLIC
    FILE_SET CXX_MODULES
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/../include/serial.ccm
)
