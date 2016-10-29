//
// Created by henrylee on 16-10-24.
//
#include <iostream>

#include "symbol_table.h"

using namespace symbol_table;
using namespace std;

SymbolTable::SymbolTable() : now_depth(0) {
  tables.push_back(new table_t);
}

SymbolTable::~SymbolTable() {

  while (!tables.empty()) {
    delete tables.back();
    tables.pop_back();
  }

}

/**
 * @param key var's name
 * @return  var's value
 * @brief  Get a var from symbol table. Search top level firstly, if not found, search secound level.
 */
int SymbolTable::GetInt(const std::string key) {
  int result = 0;

  size_t pos = tables.size() - (now_depth + 1);

  for (auto iter = tables.rbegin() + pos; iter != tables.rend(); iter++) {
    table_t &table = **iter;
    if (table.find(key) != table.end()) {
      result = table[key];
      break;
    }
  }

  return result;
}

/**
 * @param key var's name
 * @param val var's value
 * @brief  Set a var from symbol table. Search now level firstly, if not found, search lower level.
 *         If can't found it in whole table, then set a new var at the now level
 */
void SymbolTable::SetInt(const std::string key, const int val) {
  size_t pos = tables.size() - (now_depth + 1);

  for (auto iter = tables.rbegin() + pos; iter != tables.rend(); iter++) {
    table_t &table = **iter;
    if (table.find(key) != table.end()) {
      table[key] = val;
      return;
    }
  }

  table_t &table = *tables[now_depth];
  table[key] = val;
}

/**
 * @brief  Set a new level
 */
void SymbolTable::PushLevel() {
  tables.push_back(new table_t);
  if (now_depth == tables.size() - 2) {
    now_depth = tables.size() - 1;
  }
}

/**
 * @brief  Destroy the top level
 */
void SymbolTable::PopLevel() {
  if (tables.size() == 1) {
    return;
  }

  delete tables.back();
  tables.pop_back();
  if (now_depth == tables.size()) {
    now_depth = tables.size() - 1;
  }
}

/**
 * @brief  Point to a higher level. If now level is the highest, then set a new one and point to it.
 */
void SymbolTable::EnterLevel() {
  if (now_depth + 1 == tables.size()) {
    tables.push_back(new table_t);
  }

  now_depth++;
}

/**
 * @brief  Point to a lower level.
 */
void SymbolTable::LeaveLevel() {
  if (!now_depth) {
    return;
  }

  now_depth--;
}

/**
 * @brief  Destroy all level which higher than now level.
 */
void SymbolTable::PopToNowLevel() {
  while (now_depth < tables.size() - 1) {
    delete tables.back();
    tables.pop_back();
  }
}
