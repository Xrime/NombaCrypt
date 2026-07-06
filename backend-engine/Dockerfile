# NombaCrypt Shell C++ Engine Deployment
# Uses a multi-stage build to keep the final image incredibly small and secure

# -----------------
# 1. BUILD STAGE
# -----------------
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Copy the CMake config and source files
COPY CMakeLists.txt ./
COPY include/ ./include/
COPY src/ ./src/
COPY tests/ ./tests/
COPY main.cpp ./

# Build the C++ Engine for Release
RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc) NombaCrypt_Shell

# -----------------
# 2. RUNTIME STAGE
# -----------------
FROM ubuntu:22.04

WORKDIR /app

# Copy only the compiled blazing-fast binary from the builder stage
COPY --from=builder /app/build/NombaCrypt_Shell .

# Expose the HTTP Port
EXPOSE 8080

# Run the engine
CMD ["./NombaCrypt_Shell"]
