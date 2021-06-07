//
// Created by 何昂展 on 2021/6/6.
//

#ifndef MINISQL_CONDITION_H
#define MINISQL_CONDITION_H
#include <string>
using namespace std;

class Condition {
public:
    const static int OPERATOR_EQUAL = 0; // "="
    const static int OPERATOR_NOT_EQUAL = 1; // "<>"
    const static int OPERATOR_LESS = 2; // "<"
    const static int OPERATOR_MORE = 3; // ">"
    const static int OPERATOR_LESS_EQUAL = 4; // "<="
    const static int OPERATOR_MORE_EQUAL = 5; // ">="

    string name;
    // Operation type
    int operate;
    string value;

    Condition(string n, int o, string v);


};

#endif //MINISQL_CONDITION_H
