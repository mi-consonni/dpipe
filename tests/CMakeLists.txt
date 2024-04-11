find_package(GTest REQUIRED)

add_executable(dpipe-tests tests.cpp)
target_link_libraries(dpipe-tests PRIVATE dpipe::dpipe gtest::gtest)

include(GoogleTest)
gtest_discover_tests(dpipe-tests)

add_executable(dpipe-example example.cpp)
target_link_libraries(dpipe-example PRIVATE dpipe::dpipe)
