#include "crypto/hmac_verifier.hpp"
#include <iostream>
#include <cassert>
#include <exception>

using namespace nombacrypt;

void test_hmac_computation() {
    std::string key = "secret_key";
    std::string message = "hello world";
    
    std::string sig = HmacVerifier::compute_hmac_hex(message, key);
    std::string expected = "cf1a418afaafc798df48fd804a2abf6970283afd8c40b41f818ad9b6ca4f8ca8";
    
    std::cout << "[HMAC Test] Computed: " << sig << std::endl;
    std::cout << "[HMAC Test] Expected: " << expected << std::endl;
    
    assert(sig == expected);
    std::cout << "[HMAC Test] test_hmac_computation PASSED" << std::endl;
}

void test_hmac_verification() {
    std::string key = "secret_key";
    std::string message = "hello world";
    std::string valid_sig = "cf1a418afaafc798df48fd804a2abf6970283afd8c40b41f818ad9b6ca4f8ca8";
    std::string invalid_sig = "cf1a418afaafc798df48fd804a2abf6970283afd8c40b41f818ad9b6ca4f8ca9";
    
    assert(HmacVerifier::verify_signature(message, valid_sig, key) == true);
    
    std::string upper_sig = "CF1A418AFAAFC798DF48FD804A2ABF6970283AFD8C40B41F818AD9B6CA4F8CA8";
    assert(HmacVerifier::verify_signature(message, upper_sig, key) == true);
    
    assert(HmacVerifier::verify_signature(message, invalid_sig, key) == false);
    assert(HmacVerifier::verify_signature("hello world!", valid_sig, key) == false);
    assert(HmacVerifier::verify_signature(message, valid_sig, "wrong_key") == false);
    
    std::cout << "[HMAC Test] test_hmac_verification PASSED" << std::endl;
}

void test_sha256() {
    std::string message = "hello world";
    std::string computed = HmacVerifier::sha256_hex(message);
    std::string expected = "b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9";
    std::cout << "[SHA256 Test] Computed: " << computed << std::endl;
    std::cout << "[SHA256 Test] Expected: " << expected << std::endl;
    assert(computed == expected);
    std::cout << "[SHA256 Test] test_sha256 PASSED" << std::endl;
}

int main() {
    try {
        test_sha256();
        test_hmac_computation();
        test_hmac_verification();
        std::cout << "[HMAC Test] All HMAC Verifier Tests Passed Successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[HMAC Test] Failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
