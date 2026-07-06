#include "crypto/hmac_verifier.hpp"
#include <vector>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <cstdint>

namespace nombacrypt {
namespace {

// SHA-256 constants
constexpr uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

inline uint32_t rotr(uint32_t x, uint32_t n) noexcept {
    return (x >> n) | (x << (32 - n));
}

inline uint32_t choose(uint32_t x, uint32_t y, uint32_t z) noexcept {
    return (x & y) ^ (~x & z);
}

inline uint32_t majority(uint32_t x, uint32_t y, uint32_t z) noexcept {
    return (x & y) ^ (x & z) ^ (y & z);
}

inline uint32_t sig0(uint32_t x) noexcept {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

inline uint32_t sig1(uint32_t x) noexcept {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

inline uint32_t sum0(uint32_t x) noexcept {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

inline uint32_t sum1(uint32_t x) noexcept {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

std::string sha256(const std::string& message) noexcept {
    uint32_t H[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    std::vector<uint8_t> padded;
    padded.reserve(message.size() + 64);
    padded.insert(padded.end(), message.begin(), message.end());
    uint64_t bit_len = static_cast<uint64_t>(message.size()) * 8;
    
    padded.push_back(0x80);
    while ((padded.size() + 8) % 64 != 0) {
        padded.push_back(0x00);
    }
    
    for (int i = 7; i >= 0; --i) {
        padded.push_back(static_cast<uint8_t>((bit_len >> (i * 8)) & 0xFF));
    }

    for (size_t block_offset = 0; block_offset < padded.size(); block_offset += 64) {
        uint32_t W[64] = {0};
        
        for (int t = 0; t < 16; ++t) {
            W[t] = (static_cast<uint32_t>(padded[block_offset + t * 4]) << 24) |
                   (static_cast<uint32_t>(padded[block_offset + t * 4 + 1]) << 16) |
                   (static_cast<uint32_t>(padded[block_offset + t * 4 + 2]) << 8) |
                   (static_cast<uint32_t>(padded[block_offset + t * 4 + 3]));
        }
        for (int t = 16; t < 64; ++t) {
            W[t] = sig1(W[t - 2]) + W[t - 7] + sig0(W[t - 15]) + W[t - 16];
        }

        uint32_t a = H[0];
        uint32_t b = H[1];
        uint32_t c = H[2];
        uint32_t d = H[3];
        uint32_t e = H[4];
        uint32_t f = H[5];
        uint32_t g = H[6];
        uint32_t h = H[7];

        for (int t = 0; t < 64; ++t) {
            uint32_t T1 = h + sum1(e) + choose(e, f, g) + K[t] + W[t];
            uint32_t T2 = sum0(a) + majority(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
        }

        H[0] += a;
        H[1] += b;
        H[2] += c;
        H[3] += d;
        H[4] += e;
        H[5] += f;
        H[6] += g;
        H[7] += h;
    }

    std::string digest;
    digest.reserve(32);
    for (int i = 0; i < 8; ++i) {
        digest.push_back(static_cast<char>((H[i] >> 24) & 0xFF));
        digest.push_back(static_cast<char>((H[i] >> 16) & 0xFF));
        digest.push_back(static_cast<char>((H[i] >> 8) & 0xFF));
        digest.push_back(static_cast<char>(H[i] & 0xFF));
    }
    return digest;
}

} // namespace

std::string HmacVerifier::compute_hmac(const std::string& payload, const std::string& secret_key) noexcept {
    constexpr size_t BLOCK_SIZE = 64;
    thread_local std::string cached_key = "";
    thread_local std::string o_key_pad(BLOCK_SIZE, '\0');
    thread_local std::string i_key_pad(BLOCK_SIZE, '\0');

    if (cached_key != secret_key || cached_key.empty()) {
        std::string key = secret_key;
        if (key.size() > BLOCK_SIZE) key = sha256(key);
        if (key.size() < BLOCK_SIZE) key.append(BLOCK_SIZE - key.size(), '\0');
        for (size_t i = 0; i < BLOCK_SIZE; ++i) {
            o_key_pad[i] = key[i] ^ 0x5c;
            i_key_pad[i] = key[i] ^ 0x36;
        }
        cached_key = secret_key;
    }

    std::string inner_msg;
    inner_msg.reserve(BLOCK_SIZE + payload.size());
    inner_msg.append(i_key_pad);
    inner_msg.append(payload);
    std::string inner_hash = sha256(inner_msg);

    std::string outer_msg;
    outer_msg.reserve(BLOCK_SIZE + 32);
    outer_msg.append(o_key_pad);
    outer_msg.append(inner_hash);
    return sha256(outer_msg);
}

std::string HmacVerifier::compute_hmac_hex(const std::string& payload, const std::string& secret_key) noexcept {
    std::string raw_hmac = compute_hmac(payload, secret_key);
    std::string result;
    result.reserve(raw_hmac.size() * 2);
    static const char* hex_chars = "0123456789abcdef";
    for (unsigned char c : raw_hmac) {
        result.push_back(hex_chars[c >> 4]);
        result.push_back(hex_chars[c & 0x0F]);
    }
    return result;
}

bool HmacVerifier::verify_signature(const std::string& payload, const std::string& signature_hex, const std::string& secret_key) noexcept {
    std::string computed_hex = compute_hmac_hex(payload, secret_key);

    if (computed_hex.size() != signature_hex.size()) {
        return false;
    }

    int result = 0;
    for (size_t i = 0; i < computed_hex.size(); ++i) {
        char c1 = std::tolower(static_cast<unsigned char>(computed_hex[i]));
        char c2 = std::tolower(static_cast<unsigned char>(signature_hex[i]));
        result |= (c1 ^ c2);
    }
    return result == 0;
}

std::string HmacVerifier::sha256_hex(const std::string& message) noexcept {
    std::string raw = sha256(message);
    std::string result;
    result.reserve(raw.size() * 2);
    static const char* hex_chars = "0123456789abcdef";
    for (unsigned char c : raw) {
        result.push_back(hex_chars[c >> 4]);
        result.push_back(hex_chars[c & 0x0F]);
    }
    return result;
}

}
