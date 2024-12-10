// File: src/bridge/bridge_api.cpp

#include "transaction_locking.h"
#include "merkle_proofs.h"
#include <json/json.h>
#include <iostream>
#include <string>
#include <vector>

std::string handleLockTransaction(const std::string& txId, uint64_t amount) {
    if (lockTransaction(txId, amount)) {
        Json::Value response;
        response["status"] = "success";
        response["message"] = "Transaction locked successfully.";
        response["txId"] = txId;
        response["amount"] = (Json::UInt64)amount;
        Json::StreamWriterBuilder writer;
        return Json::writeString(writer, response);
    } else {
        Json::Value response;
        response["status"] = "error";
        response["message"] = "Transaction already locked.";
        Json::StreamWriterBuilder writer;
        return Json::writeString(writer, response);
    }
}

std::string handleUnlockTransaction(const std::string& txId) {
    if (unlockTransaction(txId)) {
        Json::Value response;
        response["status"] = "success";
        response["message"] = "Transaction unlocked successfully.";
        response["txId"] = txId;
        Json::StreamWriterBuilder writer;
        return Json::writeString(writer, response);
    } else {
        Json::Value response;
        response["status"] = "error";
        response["message"] = "Transaction not found.";
        Json::StreamWriterBuilder writer;
        return Json::writeString(writer, response);
    }
}

std::string handleGetLockedTransactions() {
    auto transactions = getAllLockedTransactions();
    Json::Value response;
    response["status"] = "success";
    for (const auto& [txId, amount] : transactions) {
        Json::Value tx;
        tx["txId"] = txId;
        tx["amount"] = (Json::UInt64)amount;
        response["transactions"].append(tx);
    }
    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, response);
}

std::string handleGenerateMerkleProof(const std::string& txId, const std::vector<std::string>& transactions) {
    createMerkleTree(transactions); // Ensure tree is built
    auto proof = getMerkleProof(txId);

    Json::Value response;
    if (proof.empty()) {
        response["status"] = "error";
        response["message"] = "Transaction not found in Merkle tree.";
    } else {
        response["status"] = "success";
        response["message"] = "Proof generated successfully.";
        for (const auto& hash : proof) {
            response["proof"].append(hash);
        }
    }
    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, response);
}