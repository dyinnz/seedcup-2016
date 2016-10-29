
Clike Interpreter
===============================

by DY, HL


## 编译环境
g++ 6.2.1
编译选项： `-std=c++11 -Wall`

**为获得最佳用户体验，请使用Arch Linux (kernel: 4.8.4-1), g++6.2.1 编译并测试**

## 编译步骤
1. 在工程目录下，运行 cmake . 命令
2. 在工程目录下，运行 make 命令

## 运行方法
直接运行可执行文件。


## 目录结构
    .
    ├── Bin
    ├── Doc
    └── Src
        ├── data_manager.cc
        ├── data_manager.h
        ├── file_io.cc
        ├── file_io.h
        ├── ll_io.cc
        ├── ll_io.h
        ├── main.cc
        ├── makefile
        ├── message.cc
        ├── message_consumer.cc
        ├── message_consumer.h
        ├── message.h
        ├── message_queue.cc
        ├── message_queue.h
        ├── message_sender.cc
        ├── student.cc
        ├── student.h
        ├── task.cc
        ├── task.h
        └── utility.h

---------------------------------------------------------------

## 实现功能

**基本功能**
1. 顺序结构的5种语句类型识别和运行
2. 分支结构的识别和执行
3. 循环结构的识别和执行
4. 单行多语句的处理
5. 变量作用域的处理

**拓展功能**
1. 支持单独出现的花括号作用域
2. 支持任意多的变量数量

## 实现简介

### 基本特点
本程序在gcc 6.2.1, 64bit Linux下，编译选项:`-std=c++11 -Wall`.

1. 采用C++11编写，编码风格基本符合Google Style Guider。
2. 结构清晰，代码耦合度低，可扩展性强。

### 程序结构
`clike_grammar`命名空间内用正则表达式定义了所用词法,然后使用`TokenizerBuilder`类构建词法分析器。
构建的`Tokenizer`类为所需要的词法分析器，进行输入文件的词法分析，产生一串顺序的Tokens。
生成的Tokens交由`ClikeParser`进行语法分析，构建抽象语法树(AST)。
`ClikeParser`类生成的AST交由`Interpreter`类进行解释执行，完成所有计算和操作。
`SymbolTable`类为符号表，实现了作用域的分级与解释期变量的保存。

### 程序逻辑
// TODO

---------------------------------------------------------------

### 程序接口简介

#### TokenizerBuilder

#### Tokenizer

#### ClikeParser

#### Interpreter

