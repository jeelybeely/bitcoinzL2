// File: src/bridge/merkle_proofs.h

#ifndef MERKLE_PROOFS_H
#define MERKLE_PROOFS_H

#include <vector>
#include <string>

struct MerkleNode;

std::string sha256(const std::string& input);
MerkleNode* createMerkleTree(const std::vector<std::string>& transactions);
std::vector<std::string> getMerkleProof(const std::string& tx);

#endif // MERKLE_PROOFS_H
