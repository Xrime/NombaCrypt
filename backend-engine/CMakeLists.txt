cmake_minimum_required(VERSION 3.20)
project(NombaCrypt_Shell)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

# Add all our source files to the build
add_executable(NombaCrypt_Shell
        main.cpp
        src/core/config.cpp
        src/core/logger.cpp
        src/buffer/ring_buffer.cpp
        src/rate_limiter/token_bucket.cpp
)