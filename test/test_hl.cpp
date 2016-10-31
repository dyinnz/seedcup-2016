//
// Created by henrylee on 16-10-24.
//

#include <iostream>
#include "variable_table.h"

using namespace std;

int main() {
  cout << "hello world" << endl;

  VariableTable table;

  table.SetInt("i", 1);
  table.PushLevel();
  table.SetInt("i", 2);
  cout << "i = " << table.GetInt("i") << endl;
  table.Print();
  table.SetInt("i", 3);
  cout << "i = " << table.GetInt("i") << endl;
  table.PopLevel();
  cout << "i = " << table.GetInt("i") << endl;
  return 0;
}
