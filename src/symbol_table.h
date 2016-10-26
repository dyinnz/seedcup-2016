//
// Created by henrylee on 16-10-24.
//

#ifndef PARSING_TECHNIQUES_SYMBOL_TABLE_H
#define PARSING_TECHNIQUES_SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <map>

namespace symbol_table {

  class SymbolTable {
  public:
    SymbolTable();

    ~SymbolTable();

    int GetInt(const std::string key);
    void SetInt(const std::string key, const int val);

    void PushLevel();
    void PopLevel();

    void EnterLevel();
    void LeaveLevel();

    void PopToNowLevel();

    std::size_t GetDepth();
    void Print();

  public:
    typedef std::unordered_map<std::string, int> table_t;

  private:
    std::vector<table_t*> tables;
    std::size_t  now_depth;
  };

}

#endif //PARSING_TECHNIQUES_SYMBOL_TABLE_H
