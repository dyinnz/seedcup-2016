//
// Created by Dyinnz on 16-10-25.
//

#pragma once

#include <cstring>
#include <fstream>
#include <string>

#include "utility.h"

inline std::streampos GetFileLength(std::ifstream &fin) {
  auto backup = fin.tellg();
  fin.seekg(0, fin.end);
  auto length = fin.tellg();
  fin.seekg(backup, fin.beg);
  return length;
}

inline char *ReadFileData(const std::string &path, size_t &size) {
  std::ifstream fin(path);
  if (!fin) {
    return nullptr;
  }

  size = static_cast<size_t>(GetFileLength(fin));
  auto data = new char[size + 1];
  memset(data, 0, sizeof(char) * (size + 1));

  if (fin.read(data, size)) {
    return data;
  } else {
    delete data;
    return nullptr;
  }
}

#define GET_FILE_DATA_SAFELY(name, size, relative_path) \
size_t size = 0; \
const char *name = ReadFileData((relative_path), (size)); \
ScopeGuard [&] { delete[] name; };

