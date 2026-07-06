cmake_minimum_required(VERSION 3.20)
project(NombaCrypt_Shell)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

set(CORE_SOURCES
        src/core/config.cpp
        src/core/logger.cpp
        src/buffer/ring_buffer.cpp
        src/rate_limiter/token_bucket.cpp
        src/crypto/sequence_hasher.cpp
        src/crypto/hmac_verifier.cpp
        src/crypto/anti_replay_ledger.cpp
        src/crypto/event_ledger.cpp
        src/server/http_server.cpp
        src/reconciler/reconciler.cpp
        src/pipeline/crypto_worker.cpp
        src/billing/installment_plan.cpp
        src/billing/card_tokenizer.cpp
        src/billing/subscription_manager.cpp
        src/forwarder/payload_sanitizer.cpp
        src/forwarder/downstream_forwarder.cpp
)

add_executable(NombaCrypt_Shell
        main.cpp
        ${CORE_SOURCES}
)

add_executable(test_hmac_verifier
        tests/test_hmac_verifier.cpp
        src/crypto/hmac_verifier.cpp
)

add_executable(test_anti_replay
        tests/test_anti_replay.cpp
        ${CORE_SOURCES}
)

add_executable(test_reconciler
        tests/test_reconciler.cpp
        ${CORE_SOURCES}
)

if (WIN32)
    target_link_libraries(NombaCrypt_Shell ws2_32)
    target_link_libraries(test_anti_replay ws2_32)
    target_link_libraries(test_reconciler ws2_32)
endif()
