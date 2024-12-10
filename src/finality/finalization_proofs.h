// File: src/finality/finalization_proofs.h

#ifndef FINALIZATION_PROOFS_H
#define FINALIZATION_PROOFS_H

#include <map>
#include <string>

bool addFinalizationProof(uint64_t blockHeight, const std::string& merkleRoot);
std::string getFinalizationProof(uint64_t blockHeight);
bool verifyFinalizationProof(uint64_t blockHeight, const std::string& merkleRoot);
std::map<uint64_t, std::string> getAllFinalizationProofs();

#endif // FINALIZATION_PROOFS_H
