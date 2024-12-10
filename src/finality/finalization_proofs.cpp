// File: src/finality/finalization_proofs.cpp

#include "finalization_proofs.h"
#include <map>
#include <string>
#include <mutex>

static std::map<uint64_t, std::string> finalizationProofs;
static std::mutex finalityMutex;

bool addFinalizationProof(uint64_t blockHeight, const std::string& merkleRoot) {
    std::lock_guard<std::mutex> guard(finalityMutex);
    if (finalizationProofs.find(blockHeight) != finalizationProofs.end()) {
        return false; // Proof already exists for this block
    }
    finalizationProofs[blockHeight] = merkleRoot;
    return true;
}

std::string getFinalizationProof(uint64_t blockHeight) {
    std::lock_guard<std::mutex> guard(finalityMutex);
    if (finalizationProofs.find(blockHeight) == finalizationProofs.end()) {
        return ""; // Proof not found
    }
    return finalizationProofs[blockHeight];
}

bool verifyFinalizationProof(uint64_t blockHeight, const std::string& merkleRoot) {
    std::lock_guard<std::mutex> guard(finalityMutex);
    if (finalizationProofs.find(blockHeight) == finalizationProofs.end()) {
        return false; // Proof not found
    }
    return finalizationProofs[blockHeight] == merkleRoot;
}

std::map<uint64_t, std::string> getAllFinalizationProofs() {
    std::lock_guard<std::mutex> guard(finalityMutex);
    return finalizationProofs;
}
