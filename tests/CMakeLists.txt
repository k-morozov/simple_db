cmake_minimum_required(VERSION 3.26)

set(TEST_APP_NAME unit_tests)

add_executable(${TEST_APP_NAME}
        main.cpp
        test_buffer_pool.cpp
        test_marshal.cpp
        test_tx_runtime.cpp
        test_tx_msg.cpp
        test_tx_start_msg.cpp
)

target_link_libraries(${TEST_APP_NAME}
        simple_db
        GTest::gtest_main
)

include(GoogleTest)

enable_testing()