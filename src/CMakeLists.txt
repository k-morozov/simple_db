cmake_minimum_required(VERSION 3.26)

set(SIMPLE_DB_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/src)

add_library(simple_db
        common/log/log.cpp
        common/random/generator.cpp

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

        tx/clock/clock.cpp
        tx/runtime/runtime.cpp
        tx/runtime/proxy_runtime.cpp
        tx/server/server_tx.cpp
        tx/server/server.cpp

        tx/retrier/retrier.cpp

        tx/client/client_tx_spec.cpp
        tx/client/client_tx.cpp
        tx/client/client.cpp
        tx/client/tx_participant.cpp

        tx/msg/message.cpp
        tx/msg/msg_payload.cpp
        tx/msg/msg_types.cpp
        tx/msg/sent_message.cpp

        tx/generator/generator.cpp

        tx/storage/storage.cpp

        tx/discovery/discovery.cpp
)

target_include_directories(simple_db PUBLIC ${SIMPLE_DB_INCLUDE_DIRS})
