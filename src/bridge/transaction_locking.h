// File: src/bridge/transaction_locking.h

#ifndef TRANSACTION_LOCKING_H
#define TRANSACTION_LOCKING_H

#include <map>
#include <string>

bool lockTransaction(const std::string& txId, uint64_t amount);
bool unlockTransaction(const std::string& txId);
uint64_t getLockedAmount(const std::string& txId);
std::map<std::string, uint64_t> getAllLockedTransactions();

#endif // TRANSACTION_LOCKING_H
