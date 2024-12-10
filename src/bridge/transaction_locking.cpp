// File: src/bridge/transaction_locking.cpp

#include "transaction_locking.h"
#include <map>
#include <mutex>
#include <string>

static std::map<std::string, uint64_t> lockedTransactions;
static std::mutex lockMutex;

bool lockTransaction(const std::string& txId, uint64_t amount) {
    std::lock_guard<std::mutex> guard(lockMutex);
    if (lockedTransactions.find(txId) != lockedTransactions.end()) {
        return false; // Transaction already locked
    }
    lockedTransactions[txId] = amount;
    return true;
}

bool unlockTransaction(const std::string& txId) {
    std::lock_guard<std::mutex> guard(lockMutex);
    if (lockedTransactions.find(txId) == lockedTransactions.end()) {
        return false; // Transaction not found
    }
    lockedTransactions.erase(txId);
    return true;
}

uint64_t getLockedAmount(const std::string& txId) {
    std::lock_guard<std::mutex> guard(lockMutex);
    if (lockedTransactions.find(txId) == lockedTransactions.end()) {
        return 0; // Transaction not found
    }
    return lockedTransactions[txId];
}

std::map<std::string, uint64_t> getAllLockedTransactions() {
    std::lock_guard<std::mutex> guard(lockMutex);
    return lockedTransactions;
}
