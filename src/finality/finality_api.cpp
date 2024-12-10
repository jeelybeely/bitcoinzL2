// File: src/finality/finality_api.cpp

#include "finalization_proofs.h"
#include <json/json.h>
#include <iostream>
#include <string>

std::string handleAddFinalizationProof(uint64_t blockHeight, const std::string& merkleRoot) {
    if (addFinalizationProof(blockHeight, merkleRoot)) {
        Json::Value response;
        response["status"] = "success";
        response["message"] = "Finalization proof added successfully.";
        response["blockHeight"] = (Json::UInt64)blockHeight;
        response["merkleRoot"] = merkleRoot;
        Json::StreamWriterBuilder writer;
        return Json::writeString(writer, response);
    } else {
        Json::Value response;
        response["status"] = "error";
        response["message"] = "Proof already exists for this block height.";
        Json::StreamWriterBuilder writer;
        return Json::writeString(writer, response);
    }
}

std::string handleGetFinalizationProof(uint64_t blockHeight) {
    std::string merkleRoot = getFinalizationProof(blockHeight);
    Json::Value response;
    if (merkleRoot.empty()) {
        response["status"] = "error";
        response["message"] = "No proof found for the specified block height.";
    } else {
        response["status"] = "success";
        response["blockHeight"] = (Json::UInt64)blockHeight;
        response["merkleRoot"] = merkleRoot;
    }
    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, response);
}

std::string handleVerifyFinalizationProof(uint64_t blockHeight, const std::string& merkleRoot) {
    bool valid = verifyFinalizationProof(blockHeight, merkleRoot);
    Json::Value response;
    response["status"] = valid ? "success" : "error";
    response["message"] = valid ? "Proof verified successfully." : "Proof verification failed.";
    response["blockHeight"] = (Json::UInt64)blockHeight;
    response["merkleRoot"] = merkleRoot;
    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, response);
}

std::string handleGetAllFinalizationProofs() {
    auto proofs = getAllFinalizationProofs();
    Json::Value response;
    response["status"] = "success";
    for (const auto& [blockHeight, merkleRoot] : proofs) {
        Json::Value proof;
        proof["blockHeight"] = (Json::UInt64)blockHeight;
        proof["merkleRoot"] = merkleRoot;
        response["proofs"].append(proof);
    }
    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, response);
}
