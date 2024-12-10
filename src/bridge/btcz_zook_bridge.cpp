// File: src/bridge/btcz_zook_bridge.cpp

#include "finalization_proofs.h"
#include <json/json.h>
#include <iostream>
#include <curl/curl.h>
#include <string>
#include <map>
#include <mutex>
#include <set>

static std::string zookBurnSyncUrl = "http://localhost:3030/bridge/burn-sync";
static std::map<std::string, uint64_t> unlockedTransactions;
static std::set<std::string> processedBurnTxIds;
static std::mutex unlockMutex;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool synchronizeBurnedRecords(const std::string& burnRecordsJson) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, zookBurnSyncUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, burnRecordsJson.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(nullptr, "Content-Type: application/json"));

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
            curl_easy_cleanup(curl);
            return false;
        }
        curl_easy_cleanup(curl);

        Json::CharReaderBuilder reader;
        Json::Value response;
        std::string errs;
        std::istringstream s(readBuffer);
        if (Json::parseFromStream(reader, s, &response, &errs)) {
            if (response["status"].asString() == "success") {
                return true;
            } else {
                std::cerr << "Zook Network error: " << response["message"].asString() << std::endl;
            }
        } else {
            std::cerr << "Failed to parse Zook Network response." << std::endl;
        }
    }
    return false;
}

bool unlockBTCZ(const std::string& txId, uint64_t amount) {
    std::lock_guard<std::mutex> guard(unlockMutex);
    if (unlockedTransactions.find(txId) != unlockedTransactions.end()) {
        std::cerr << "Transaction already unlocked: " << txId << std::endl;
        return false;
    }
    if (processedBurnTxIds.find(txId) != processedBurnTxIds.end()) {
        std::cerr << "Burn transaction already processed: " << txId << std::endl;
        return false;
    }
    unlockedTransactions[txId] = amount;
    processedBurnTxIds.insert(txId);
    std::cout << "Unlocked " << amount << " BTCZ for transaction " << txId << std::endl;
    return true;
}

std::string handleBurnSync(const std::string& burnRecordsJson) {
    Json::CharReaderBuilder reader;
    Json::Value burnRecords;
    std::string errs;
    std::istringstream s(burnRecordsJson);

    if (!Json::parseFromStream(reader, s, &burnRecords, &errs)) {
        return "{"status": "error", "message": "Failed to parse burn records."}";
    }

    for (const auto& record : burnRecords) {
        const std::string txId = record["tx-id"].asString();
        const uint64_t amount = record["amount"].asUInt64();
        if (!unlockBTCZ(txId, amount)) {
            return "{"status": "error", "message": "Failed to unlock BTCZ."}";
        }
    }
    return "{"status": "success", "message": "Burn records synchronized and BTCZ unlocked."}";
}

bool verifyBurnConsistency(const std::string& txId) {
    std::lock_guard<std::mutex> guard(unlockMutex);
    return processedBurnTxIds.find(txId) != processedBurnTxIds.end();
}
