#include "crypto/sequence_hasher.hpp"

namespace nombacrypt {

uint64_t SequenceHasher::fnv1a_hash(const char* data, size_t len) noexcept {
    constexpr uint64_t FNV_offset_basis = 14695981039346656037ULL;
    constexpr uint64_t FNV_prime = 1099511628211ULL;

    uint64_t hash = FNV_offset_basis;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint8_t>(data[i]);
        hash *= FNV_prime;
    }
    return hash;
}

uint64_t SequenceHasher::fnv1a_hash(const std::string& data) noexcept {
    return fnv1a_hash(data.data(), data.size());
}

}
