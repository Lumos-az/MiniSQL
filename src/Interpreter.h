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
    // Process word "create"
    int execCreate(const string& text, int* shift);
    // Create table
    int execCreateTable(const string& text, int* shift);
    // Create index
    int execCreateIndex(const string& text, int* shift);
    // Process word "drop"
    int execDrop(const string& text, int* shift);
    // Drop table
    int execDropTable(const string& text, int* shift);
    // Drop index
    int execDropIndex(const string& text, int* shift);
    // Process word "select"
    int execSelect(const string& text, int* shift);

    Interpreter() {};
    ~Interpreter() {};

};

#endif //MINISQL_INTERPRETER_H
