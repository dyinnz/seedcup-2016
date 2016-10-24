//
// Created by henrylee on 16-10-24.
//
#include <iostream>

#include "symbol_table.h"

using namespace symbol_table;
using namespace std;

SymbolTable::SymbolTable() {
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

  for (auto iter = tables.rbegin(); iter != tables.rend(); iter++) {
    table_t &table = **iter;
    if (table.find(key) != table.end()) {
      result = table[key];
      break;
    }
  }

  return result;
}

void SymbolTable::SetInt(const std::string key, const int val) {
  table_t &table = *tables.back();
  table[key] = val;
}

void SymbolTable::PushLevel() {
  tables.push_back(new table_t);
}

void SymbolTable::PopLevel() {
  if (tables.size() == 1) {
    return;
  }

  delete tables.back();
  tables.pop_back();
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
