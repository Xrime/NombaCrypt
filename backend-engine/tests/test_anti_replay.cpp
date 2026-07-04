#include "crypto/anti_replay_ledger.hpp"
#include "crypto/sequence_hasher.hpp"
#include "crypto/event_ledger.hpp"
#include <iostream>
#include <cassert>
#include <thread>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <exception>

using namespace nombacrypt;

void test_sequence_hasher() {
    std::string m1 = "payload_data_1";
    std::string m2 = "payload_data_2";

    uint64_t h1 = SequenceHasher::fnv1a_hash(m1);
    uint64_t h2 = SequenceHasher::fnv1a_hash(m2);
    uint64_t h1_again = SequenceHasher::fnv1a_hash(m1);

    assert(h1 != 0);
    assert(h1 != h2);
    assert(h1 == h1_again);

    std::cout << "[Replay Test] Hash 1: " << h1 << ", Hash 2: " << h2 << std::endl;
    std::cout << "[Replay Test] test_sequence_hasher PASSED" << std::endl;
}

void test_anti_replay_basic() {
    AntiReplayLedger::get_instance().clear();
    assert(AntiReplayLedger::get_instance().size() == 0);

    uint64_t hash = 123456789ULL;

    bool success1 = AntiReplayLedger::get_instance().insert_and_check(hash);
    assert(success1 == true);
    assert(AntiReplayLedger::get_instance().size() == 1);

    bool success2 = AntiReplayLedger::get_instance().insert_and_check(hash);
    assert(success2 == false);
    assert(AntiReplayLedger::get_instance().size() == 1);

    std::cout << "[Replay Test] test_anti_replay_basic PASSED" << std::endl;
}

void test_anti_replay_ttl() {
    AntiReplayLedger::get_instance().clear();

    uint64_t hash = 987654321ULL;
    Timestamp initial_time = 1000000000ULL;

    bool success1 = AntiReplayLedger::get_instance().insert_and_check(hash, initial_time);
    assert(success1 == true);

    bool success2 = AntiReplayLedger::get_instance().insert_and_check(hash, initial_time + 240000000ULL);
    assert(success2 == false);

    bool success3 = AntiReplayLedger::get_instance().insert_and_check(hash, initial_time + 360000000ULL);
    assert(success3 == true);

    std::cout << "[Replay Test] test_anti_replay_ttl PASSED" << std::endl;
}

void test_event_ledger() {
    std::string test_log = "test_security_audit.log";
    std::remove(test_log.c_str());

    EventLedger::get_instance().configure(test_log);
    EventLedger::get_instance().log_event("tx_123", "SIGNATURE_VERIFIED", "SUCCESS", "Valid HMAC");
    EventLedger::get_instance().log_event("tx_456", "REPLAY_DETECTED", "FAILED", "Duplicate detected");

    std::ifstream file(test_log);
    assert(file.is_open() == true);

    std::string line;
    int line_count = 0;
    while (std::getline(file, line)) {
        line_count++;
        std::cout << "[Event Log] " << line << std::endl;
        if (line_count == 1) {
            assert(line.find("SIGNATURE_VERIFIED") != std::string::npos);
            assert(line.find("tx_123") != std::string::npos);
            assert(line.find("SUCCESS") != std::string::npos);
        } else if (line_count == 2) {
            assert(line.find("REPLAY_DETECTED") != std::string::npos);
            assert(line.find("tx_456") != std::string::npos);
            assert(line.find("FAILED") != std::string::npos);
        }
    }
    assert(line_count == 2);

    file.close();
    std::remove(test_log.c_str());

    std::cout << "[Replay Test] test_event_ledger PASSED" << std::endl;
}

int main() {
    try {
        test_sequence_hasher();
        test_anti_replay_basic();
        test_anti_replay_ttl();
        test_event_ledger();
        std::cout << "[Replay Test] All Replay & Logging Tests Passed Successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[Replay Test] Failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
