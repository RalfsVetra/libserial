include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz
  URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
)
FetchContent_MakeAvailable(googletest)

add_executable(unit_test UnitTests.cc)

target_link_libraries(unit_test
  GTest::gtest_main
  serial
)

include(GoogleTest)
gtest_discover_tests(unit_test)
