# NombaCrypt Shell: Security & Collaboration Development Plan
> **Role**: Security Engineer
> **Hackathon Timeline**: July 3 – July 7, 2026
> **Current Date**: July 4, 2026 (Sprint 2)

Because this is a collaborative, parallel-built project, we must ensure our Security layer implementation does not cause compilation blocks or merge conflicts with other teams (Speed and Frontend). 

This plan details **exactly how to build, test, and integrate your security code** safely while other teams are working on their parts of the system.

---

## 📅 Chronological Collaboration Roadmap

### 🔵 Phase 1: Standalone Security Library (Today - July 4)
* **Goal**: Build all cryptographic, replay detection, and logging logic in isolation.
* **Dependencies**: **None**. All classes here are completely self-contained.
* **Tasks**:
  1. Implement [sequence_hasher](file:///c:/Users/HP/Desktop/numbaHackaton/backend-engine/include/crypto/sequence_hasher.hpp) & [sequence_hasher.cpp](file:///c:/Users/HP/Desktop/numbaHackaton/backend-engine/src/crypto/sequence_hasher.cpp) (FNV-1a hash algorithm).
  2. Implement [anti_replay_ledger](file:///c:/Users/HP/Desktop/numbaHackaton/backend-engine/include/crypto/anti_replay_ledger.hpp) & [anti_replay_ledger.cpp](file:///c:/Users/HP/Desktop/numbaHackaton/backend-engine/src/crypto/anti_replay_ledger.cpp) (Thread-safe memory store for hashes with TTL eviction).
  3. Implement [hmac_verifier](file:///c:/Users/HP/Desktop/numbaHackaton/backend-engine/include/crypto/hmac_verifier.hpp) & [hmac_verifier.cpp](file:///c:/Users/HP/Desktop/numbaHackaton/backend-engine/src/crypto/hmac_verifier.cpp) (HMAC-SHA256 signature verification).
  4. Implement [event_ledger](file:///c:/Users/HP/Desktop/numbaHackaton/backend-engine/include/crypto/event_ledger.hpp) & [event_ledger.cpp](file:///c:/Users/HP/Desktop/numbaHackaton/backend-engine/src/crypto/event_ledger.cpp) (Thread-safe logging of security events to file).
* **Integration Rule**: We do **not** modify `main.cpp` or the `pipeline` files yet. We write tests in [tests/](file:///c:/Users/HP/Desktop/numbaHackaton/backend-engine/tests/) to verify our code.

---

### 🟢 Phase 2: Pipeline Integration (July 5)
* **Goal**: Connect the Security components to the main transaction pipeline.
* **Dependencies**: **Waiting on Speed Team** to finish the pipeline design (`pipeline.hpp`/`pipeline.cpp` and `crypto_worker.hpp`/`crypto_worker.cpp`).
* **Collaboration Strategy**:
  * While the Speed team writes the pipeline orchestrator, they will use `SlotState::WRITTEN` as the initial state of a slot enqueued by `IngestWorker`.
  * The Speed team will implement a thread pool of `CryptoWorkers` that polls the buffer for slots in the `WRITTEN` state.
  * In the `CryptoWorker` loop, our verification function is called:
    ```cpp
    // Inside CryptoWorker thread loop
    TransactionSlot* slot = ring_buffer->dequeue_for_crypto_verification();
    if (slot) {
        bool verified = nombacrypt::HmacVerifier::verify_signature(
            slot->payload,
            get_signature_header(slot),
            config.private_key
        );
        
        uint64_t hash = nombacrypt::SequenceHasher::fnv1a_hash(slot->payload, slot->payload_len);
        bool unique = nombacrypt::AntiReplayLedger::get_instance().insert_and_check(hash);
        
        if (verified && unique) {
            slot->state.store(SlotState::PROCESSING, std::memory_order_release);
            nombacrypt::EventLedger::get_instance().log_event(
                slot->slot_id, "TRANSACTION_VERIFIED", "SUCCESS", ""
            );
        } else {
            slot->state.store(SlotState::EMPTY, std::memory_order_release); // drop it
            nombacrypt::EventLedger::get_instance().log_event(
                slot->slot_id, "TRANSACTION_REJECTED", "FAILED", 
                !verified ? "Invalid Signature" : "Replay Attack Detected"
            );
        }
    }
    ```

---

### 🟡 Phase 3: Telemetry & UI Integration (July 6)
* **Goal**: Broadcast security alerts (like replay attacks blocked) to the React terminal dashboard.
* **Dependencies**: **Waiting on Python/Frontend Teams** for the IPC bridge (`ipc.cpp`/`ipc.hpp` and Python control API routes).
* **Collaboration Strategy**:
  * We will add an `AppendOnlyEventLog` event emitter or thread-safe callback that the WebSocket server inside `ipc.cpp` can read.
  * When a signature check fails, we broadcast a JSON event:
    ```json
    {
      "type": "SECURITY_ALERT",
      "event": "REPLAY_ATTACK_BLOCKED",
      "tx_id": "tx_xxxx_xxxx",
      "timestamp": 1719999999
    }
    ```
  * The frontend team will read this WebSocket event stream and fire the **"Intrusion Blocked"** pop-up notification.

---

### 🔴 Phase 4: Stress Testing & Hardening (July 7)
* **Goal**: Code freeze, run load tests, and optimize security overhead.
* **Dependencies**: None.
* **Tasks**:
  * Verify that signature validation does not slow down throughput below our 1,000,000 requests/sec target.
  * Verify memory consumption of the `AntiReplayLedger` and ensure TTL eviction runs smoothly in a background thread.
  * Freeze the codebase at noon, compile production build, and document implementation in `README.md`.

---

## 🛠️ Testing Our Security Code in Isolation

To test our code before integrating it into the full server build, we will:
1. Create a lightweight test harness inside the `tests` directory.
2. Update the `backend-engine/CMakeLists.txt` temporarily (or write a separate test Makefile) to compile our test programs.
3. This guarantees we have **100% correct, verified security code** ready to copy-paste into the main pipeline the moment the Speed team is ready.
