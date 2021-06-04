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
    int Interpret(string text);
    string ExtractWord(string text, int *shift);
    Interpreter();
    ~Interpreter();

};

#endif //MINISQL_INTERPRETER_H
