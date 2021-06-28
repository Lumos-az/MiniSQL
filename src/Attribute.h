//
// Created by 何昂展 on 2021/6/5.
//

#ifndef MINISQL_ATTRIBUTE_H
#define MINISQL_ATTRIBUTE_H

#include <string>

using namespace std;

class Attribute {
public:
    int static const TYPE_INT = 0;
    int static const TYPE_FLOAT = 1;
    int static const TYPE_CHAR = 2;

    string name;
    // Data type: 0->int, 1->float, 2->char
    int dataType;
    int charSize;
    bool unique;
    string index;
    bool primaryKey;

    Attribute(string n, int t, bool u = false, int c = 0, bool p = false, string i = "none") {
        name = n;
        dataType = t;
        unique = u;
        charSize = c;
        primaryKey = p;
        index = i;
    }
};

#endif //MINISQL_ATTRIBUTE_H
