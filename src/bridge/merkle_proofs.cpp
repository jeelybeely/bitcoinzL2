// File: src/bridge/merkle_proofs.cpp
#include "merkle_proofs.h"
#include "consensus/merkle.h"
#include "utilstrencodings.h"
#include <vector>
#include <string>
#include <iostream>
#include <openssl/sha.h>

// Compute a secure cryptographic hash
std::string ComputeHash(const std::string& left, const std::string& right) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    std::string combined = left + right;
    SHA256(reinterpret_cast<const unsigned char*>(combined.c_str()), combined.size(), hash);
    return HexStr(hash, hash + SHA256_DIGEST_LENGTH);
}

// Verify Merkle proof given a transaction hash and proof
bool verifyMerkleProof(const std::string& txHash, const std::vector<std::string>& proof, const std::string& root) {
    std::string computedHash = txHash;
    for (const auto& siblingHash : proof) {
        computedHash = ComputeHash(computedHash, siblingHash);
    }
    return computedHash == root;
}

// Generate a Merkle proof for a given transaction hash
std::vector<std::string> generateMerkleProof(const std::vector<std::string>& transactions, const std::string& txHash) {
    std::vector<std::string> proof;
    if (transactions.empty()) {
        std::cerr << "Error: Empty transaction list provided." << std::endl;
        return proof;
    }

    std::vector<std::string> currentLevel = transactions;
    while (currentLevel.size() > 1) {
        std::vector<std::string> nextLevel;
        for (size_t i = 0; i < currentLevel.size(); i += 2) {
            std::string left = currentLevel[i];
            std::string right = (i + 1 < currentLevel.size()) ? currentLevel[i + 1] : left;
            nextLevel.push_back(ComputeHash(left, right));

            if (txHash == left || txHash == right) {
                proof.push_back(txHash == left ? right : left);
            }
        }
        currentLevel = nextLevel;
    }

    if (proof.empty()) {
        std::cerr << "Error: Transaction hash not found in the provided list." << std::endl;
    }
    return proof;
}

// Compute the Merkle root for a list of transactions
std::string computeMerkleRoot(const std::vector<std::string>& transactions) {
    if (transactions.empty()) {
        std::cerr << "Error: Cannot compute Merkle root for an empty transaction list." << std::endl;
        return "";
    }

    std::vector<std::string> currentLevel = transactions;
    while (currentLevel.size() > 1) {
        std::vector<std::string> nextLevel;
        for (size_t i = 0; i < currentLevel.size(); i += 2) {
            std::string left = currentLevel[i];
            std::string right = (i + 1 < currentLevel.size()) ? currentLevel[i + 1] : left;
            nextLevel.push_back(ComputeHash(left, right));
        }
        currentLevel = nextLevel;
    }
    return currentLevel[0];
}

// Add functionality for state anchoring
bool processStateAnchor(const std::string& merkleRoot, const std::string& stateRoot, const std::vector<std::string>& transactions) {
    // Compute the Merkle root locally
    std::string computedMerkleRoot = computeMerkleRoot(transactions);
    if (computedMerkleRoot != merkleRoot) {
        std::cerr << "Error: Provided Merkle root does not match computed Merkle root." << std::endl;
        return false;
    }

    // Log successful validation
    std::cout << "State anchor processed successfully. Merkle root: " << merkleRoot
              << ", State root: " << stateRoot << std::endl;
    return true;
}
