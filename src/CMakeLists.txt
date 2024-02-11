cmake_minimum_required(VERSION 3.26)

set(SIMPLE_DB_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src)

add_library(simple_db
        db/database.cpp
        store/store.cpp
        buffer_pool/buffer_pool.cpp
        buffer_pool/pool_page.cpp
        buffer_pool/frame_pool.cpp
        buffer_pool/file.cpp
        table/table.cpp
        table/table_page.cpp
        table/row.cpp
        table/fixed_table_page.cpp
        transform/fixed_page_provider.cpp
        transform/marshal.cpp
        tx/runtime.cpp
        common/log/log.cpp
        common/random/generator.cpp
        tx/msg/message.cpp
        tx/msg/sent_message.cpp
        tx/clock/clock.cpp
)

target_include_directories(simple_db PUBLIC ${SIMPLE_DB_INCLUDE_DIRS})
