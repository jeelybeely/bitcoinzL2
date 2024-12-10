// File: src/bridge/merkle_proofs.cpp

#include "merkle_proofs.h"
#include <openssl/sha.h>
#include <vector>
#include <string>
#include <map>
#include <sstream>

struct MerkleNode {
    std::string hash;
    MerkleNode* left;
    MerkleNode* right;
};

static std::map<std::string, MerkleNode*> transactionNodes;

std::string sha256(const std::string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << static_cast<int>(hash[i]);
    }
    return ss.str();
}

MerkleNode* createMerkleTree(const std::vector<std::string>& transactions) {
    std::vector<MerkleNode*> nodes;

    for (const auto& tx : transactions) {
        MerkleNode* node = new MerkleNode{sha256(tx), nullptr, nullptr};
        transactionNodes[tx] = node;
        nodes.push_back(node);
    }

    while (nodes.size() > 1) {
        std::vector<MerkleNode*> nextLevel;

        for (size_t i = 0; i < nodes.size(); i += 2) {
            MerkleNode* left = nodes[i];
            MerkleNode* right = (i + 1 < nodes.size()) ? nodes[i + 1] : nullptr;

            std::string combinedHash = left->hash + (right ? right->hash : "");
            MerkleNode* parent = new MerkleNode{sha256(combinedHash), left, right};
            nextLevel.push_back(parent);
        }
        nodes = nextLevel;
    }

    return nodes.front();
}

std::vector<std::string> getMerkleProof(const std::string& tx) {
    std::vector<std::string> proof;

    if (transactionNodes.find(tx) == transactionNodes.end()) {
        return proof; // Transaction not found
    }

    MerkleNode* current = transactionNodes[tx];

    while (current->left || current->right) {
        if (current->left && current->right) {
            proof.push_back(current->right->hash);
        }
        current = current->left ? current->left : current->right;
    }

    return proof;
}
