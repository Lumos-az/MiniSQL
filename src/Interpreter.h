//
// Created by 何昂展 on 2021/6/4.
//

#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

#include <string.h>
#include <vector>
using namespace std;

class Interpreter {
public:
    void MainPage();
    int Interpret(const string& text);
    // Extract single word or key symbol
    string extractWord(const string& text, int* shift);
    // Process Word "create"
    int execCreate(const string& text, int* shift);
    // Create Table
    int execCreateTable(const string& text, int* shift);
    // Create Index
    int execCreateIndex(const string& text, int* shift);
    Interpreter();
    ~Interpreter();

};

#endif //MINISQL_INTERPRETER_H
