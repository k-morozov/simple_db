cmake_minimum_required(VERSION 3.25)

set(TEST_APP_NAME unit_tests)

add_executable(${TEST_APP_NAME}
        main.cpp
        test_buffer_pool.cpp
)

target_link_libraries(${TEST_APP_NAME}
        simple_db
        GTest::gtest_main
)

include(GoogleTest)

enable_testing()