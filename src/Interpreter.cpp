//
// Created by 何昂展 on 2021/6/4.
//

#include "Interpreter.h"
#include "Attribute.h"
#include "Condition.h"
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
using namespace std;

void Interpreter::MainPage() {
    cout << "Welcome to Our MiniSQL!" << endl;
    string line;
    string text;
    string blank = " ";
    int fact = 1;
    while (fact != 587) {
        cout << "Please Enter Your Commands!" << endl;
        text = "";
        do {
            getline(cin, line);
            text += blank;
            text += line;
        } while (text[text.size() - 1] != ';');
        cout << text << endl;
        fact = Interpret(text);
        cout << fact << endl;
    }

}

/* Interpret user's commands */
int Interpreter::Interpret(const string& text) {
    int position = 0;
    int* shift = &position;
    string word = extractWord(text, shift);
    if (word == "create") {
        return execCreate(text, shift);
    }
    else if (word == "drop") {
        return execDrop(text, shift);
    }
    else if (word == "select") {
        return execSelect(text, shift);
    }
    return 0;
}

/* Process the word "create" */
int Interpreter::execCreate(const string& text, int* shift) {
    string word = extractWord(text, shift);
    if (word == "table") {
        return execCreateTable(text, shift);
    }
    else if (word == "index") {
        return execCreateIndex(text, shift);
    }
    else {
        cout << "Syntax error for create" << endl;
        return 0;
    }
}

/* Create table */
int Interpreter::execCreateTable(const string& text, int *shift) {
    string tableName;
    string primaryKey;
    string word = extractWord(text, shift);
    // Determine whether the table name exists
    if (word.empty()) {
        cout << "Syntax Error for no table name" << endl;
        return 0;
    }
    tableName = word;

    // Judge whether "(" exist
    word = extractWord(text, shift);
    if (word != "(") {
        cout << "Syntax Error for no (" << endl;
        return 0;
    }

    // Deal with attributes
    word = extractWord(text, shift);
    vector<Attribute> attributes;
    while (word != "primary" && word != ")" && !word.empty()) {
        string attributeName = word;
        int dataType;
        bool unique = false;
        int charSize;

        // Deal with the data type
        word = extractWord(text, shift);
        if (word == "int")
            dataType = 0;
        else if (word == "float")
            dataType = 1;
        else if (word == "char") {
            // Deal with (number)
            dataType = 2;

            // Determine whether "(" exists
            word = extractWord(text, shift);
            if (word != "(") {
                cout << "Syntax Error for unknown or missing data type" << endl;
                return 0;
            }

            // Judge whether it is numeral
            word = extractWord(text, shift);
            try {
                charSize = stoi(word);
                string check = to_string(charSize);
                // Determine whether the string contains content other than numbers
                if (check != word) {
                    cout << "Syntax error for illegal number in char type" << endl;
                    return 0;
                }
            }
            catch (...) {
                cout << "Syntax error for illegal number in char type" << endl;
                return 0;
            }

            // Determine whether the ")" exists
            word = extractWord(text, shift);
            if (word != ")") {
                cout << "Syntax error for no )" << endl;
                return 0;
            }
        }
        else {
            cout << word << endl;
            cout << "Syntax error for unknown or missing data type" << endl;
            return 0;
        }

        // Judge whether it is unique
        word = extractWord(text, shift);
        if (word == "unique") {
            unique = true;
            word = extractWord(text, shift);
        }

        // Determine whether "," exists
        if (word != ",") {
            cout << "Syntax Error for no ," << endl;
            return 0;
        }
        word = extractWord(text, shift);

        // Create and store attribute
        if (dataType == 2) {
            Attribute attribute(attributeName, dataType, unique, charSize);
            attributes.push_back(attribute);
        }
        else {
            Attribute attribute(attributeName, dataType, unique);
            attributes.push_back(attribute);
        }
    }

    // Deal with primary key
    if (word == "primary") {
        //Determine whether "key" exists
        word = extractWord(text, shift);
        if (word != "key") {
            cout << "Syntax Error for primary key: no \"key\"" << endl;
            return 0;
        }

        // Determine whether "(" exists
        word = extractWord(text, shift);
        if (word != "(") {
            cout << "Syntax Error for primary key: no \"(\"" << endl;
            return 0;
        }

        // Add primary key to attribute
        word = extractWord(text, shift);
        primaryKey = word;
        int primaryKeyIndex;
        for (primaryKeyIndex = 0; primaryKeyIndex < attributes.size(); ++primaryKeyIndex) {
            if (primaryKey == attributes[primaryKeyIndex].name) {
                attributes[primaryKeyIndex].unique = true;
                attributes[primaryKeyIndex].primaryKey = true;
                break;
            }
        }

        // Don't find primary key in attributes
        if (primaryKeyIndex == attributes.size()) {
            cout << "Syntax Error for primary key" << endl;
            return 0;
        }

        // Determine whether ")" exits
        word = extractWord(text, shift);
        if (word != ")") {
            cout << "Syntax Error for primary key: no \")\"" << endl;
            return 0;
        }
    }

    // Determine whether ")" exists
    word = extractWord(text, shift);
    if (word != ")") {
        cout << "Syntax Error for no \")\" in the end" << endl;
        return 0;
    }

    return 1;
}

/* Create index */
int Interpreter::execCreateIndex(const string& text, int *shift) {
    string indexName;
    string tableName;
    string attributeName;
    string word = extractWord(text, shift);
    indexName = word;

    // Determine whether "on" exists
    word = extractWord(text, shift);
    if (word != "on") {
        cout << "Syntax error for no \"on\"" << endl;
        return 0;
    }

    word = extractWord(text, shift);
    tableName = word;

    // Determine whether "(" exists
    word = extractWord(text, shift);
    if (word != "(") {
        cout << "Syntax error for no \"(\"" << endl;
        return 0;
    }

    word = extractWord(text, shift);
    attributeName = word;

    // Determine whether ")" exists
    word = extractWord(text, shift);
    if (word != ")") {
        if (word != "(") {
            cout << "Syntax error for no \")\"" << endl;
            return 0;
        }
    }

    return 1;
}

/* Process the word "drop" */
int Interpreter::execDrop(const string& text, int* shift) {
    string word = extractWord(text, shift);
    if (word == "table") {
        return execDropTable(text, shift);
    }
    else if (word == "index") {
        return execDropIndex(text, shift);
    }
    else {
        cout << "Syntax error for drop" << endl;
        return 0;
    }
}

/* Drop table */
int Interpreter::execDropTable(const string& text, int* shift) {
    string tableName;
    string word = extractWord(text, shift);
    tableName = word;

    // Determine whether table name exists
    word = extractWord(text, shift);
    if (word != ";") {
        cout << "Syntax error for missing table name" << endl;
        return 0;
    }

    return 1;
}

/* Drop index */
int Interpreter::execDropIndex(const string& text, int* shift) {
    string indexName;
    string word = extractWord(text, shift);
    indexName = word;

    // Determine whether index name exists
    word = extractWord(text, shift);
    if (word != ";") {
        cout << "Syntax error for missing index name" << endl;
        return 0;
    }

    return 1;
}

/* Process the word "select" */
int Interpreter::execSelect(const string &text, int *shift) {
    // In case we need to select specific attributes later, use vector
    vector<string> attributes;
    string tableName;

    // Determine whether "*" exists
    string word = extractWord(text, shift);
    if (word != "*") {
        cout << "Syntax error for missing \"*\"" << endl;
        return 0;
    }
    attributes.push_back(word);

    // Determine whether "from" exists
    word = extractWord(text, shift);
    if (word != "from") {
        cout << "Syntax error for missing \"from\"" << endl;
        return 0;
    }

    word = extractWord(text, shift);
    tableName = word;

    // Determine whether the sentence is over
    word = extractWord(text, shift);
    // Sentence is over
    if (word == ";") {
        return 1;
    }
    // Conditional statement exists
    else if (word == "where") {
        vector<Condition> conditions;
        string attributeName;
        int operate;
        string value;

        // Extract all conditions
        do {
            word = extractWord(text, shift);
            attributeName = word;
            word = extractWord(text, shift);
            if (word ==  "=")
                operate = Condition::OPERATOR_EQUAL;
            else if (word == "<")
                operate = Condition::OPERATOR_LESS;
            else if (word == ">")
                operate = Condition::OPERATOR_MORE;
            else if (word == "<=")
                operate = Condition::OPERATOR_LESS_EQUAL;
            else if (word == ">=")
                operate = Condition::OPERATOR_MORE_EQUAL;
            else if (word == "<>")
                operate = Condition::OPERATOR_NOT_EQUAL;
            else {
                cout << "Syntax error for condition" << endl;
                return 0;
            }
            word = extractWord(text, shift);
            value = word;

            // Create and store the condition
            Condition condition(attributeName, operate, value);
            conditions.push_back(condition);

            word = extractWord(text, shift);
        } while (word == "and");

        // Determine whether the ";" exists
        if (word != ";") {
            cout << "Syntax error for no \";\"" << endl;
            return 0;
        }
        return 1;
    }
    else {
        cout << "Syntax error" << endl;
        return 0;
    }
}
/* Pick up single word or key symbol */
string Interpreter::extractWord(const string& text, int *shift) {
    string word;
    int start, end;
    // Skip the blanks, tab
    while (text[*shift] == ' ' || text[*shift] == '\t') {
        (*shift)++;
    }
    start = *shift;
    // Extract the key symbol
    if (text[*shift] == '(' || text[*shift] == ',' || text[*shift] == ')' || text[*shift] == ';') {
        (*shift)++;
        end = *shift;
        word = text.substr(start, end - start);
        return word;
    }
    // Encounter quotation mark
    else if (text[*shift] == '\'') {
        (*shift)++;
        start = *shift;
        while (text[*shift] != '\'' && text[*shift] != ';') {
            if (*shift > 100) {
                break;
            }
            (*shift)++;
        }
        // Can't find the corresponding quotation mark
        if (text[*shift] != '\'') {
            return word;
        }
        else {
            end = *shift;
            word = text.substr(start, end - start);
            (*shift)++;
            return word;
        }
    }
    // Extract the word
    else {
        while (text[*shift] != ' ' && text[*shift] != ',' && text[*shift] != '\t' && text[*shift] != ';'&&
                text[*shift] != '(' && text[*shift] != ')') {
            (*shift)++;
        }
        end = *shift;
        word = text.substr(start, end - start);
        return word;
    }

}




