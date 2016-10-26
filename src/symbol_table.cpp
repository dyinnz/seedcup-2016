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

void SymbolTable::PushLevel() {
  tables.push_back(new table_t);
  if (now_depth == tables.size() - 2) {
    now_depth = tables.size() - 1;
  }
}

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

void SymbolTable::EnterLevel() {
  if (now_depth + 1 == tables.size()) {
    tables.push_back(new table_t);
  }

  now_depth++;
}

void SymbolTable::LeaveLevel() {
  if (!now_depth) {
    return;
  }

  now_depth--;
}

void SymbolTable::PopToNowLevel() {
  while (now_depth < tables.size() - 1) {
    delete tables.back();
    tables.pop_back();
  }
}

size_t SymbolTable::GetDepth() {
  return tables.size();
}

void SymbolTable::Print() {
  cout << __func__ << "(): " << endl;
  int depth = 0;
  for (auto table_iter = tables.begin(); table_iter != tables.end(); table_iter++) {
    table_t &table = **table_iter;

    for (auto iter = table.begin(); iter != table.end(); iter++) {
      for (int i = 0; i < depth; i++) {
        cout << "--";
      }
      cout << iter->first << ": " << iter->second << endl;
    }

    depth++;
  }
}
