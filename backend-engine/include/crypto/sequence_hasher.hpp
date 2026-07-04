#pragma once

#include <string>
#include <cstddef>
#include <cstdint>

namespace nombacrypt {

class SequenceHasher {
public:
    
    static uint64_t fnv1a_hash(const char* data, size_t len) noexcept;

    
    static uint64_t fnv1a_hash(const std::string& data) noexcept;
};

} 
