#!/bin/python

import sys
import re

patterns = {
        re.compile(r'^\s*do\s*\{?\s$'),
        re.compile(r'^\s*;\s*$'),
        re.compile(r'^\s*\{\s$'),
        re.compile(r'^\s*\}\s$'),
        re.compile(r'\s*\*/\s*$'),
        re.compile(r'^\s*$'),
        }

def main(argv):

    if len(argv) != 2:
        print('./test_tool filename')
        return

    print('#include <stdio.h>')
    print('int main() {')
    is_find = False;
    for line in open(argv[1]):
        for pattern in patterns:
            result = pattern.search(line)
            if (result):
                is_find = True;
                break

        if is_find:
            print(line.strip())
        else:
            print(line.strip() + 'fprintf(stderr, "%d ", __LINE__ - 2);')
        is_find = False
    print('}')

main(sys.argv)
