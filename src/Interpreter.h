//
// Created by 何昂展 on 2021/6/4.
//

#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

#include <cstring>
#include <vector>

using namespace std;

class Interpreter {
public:
    // User Interface
    void MainPage();

    // Extract a single word or the key symbol
    static string extractWord(const string &text, int *shift);

    // Interpret the commands
    int Interpret(const string &text);

    // Process word "create"
    int execCreate(const string &text, int *shift);

    // Create table
    int execCreateTable(const string &text, int *shift);

    // Create index
    int execCreateIndex(const string &text, int *shift);

    // Process word "drop"
    int execDrop(const string &text, int *shift);

    // Drop table
    int execDropTable(const string &text, int *shift);

    // Drop index
    int execDropIndex(const string &text, int *shift);

    // Process word "select"
    int execSelect(const string &text, int *shift);

    // Insert record
    int execInsert(const string &text, int *shift);

    // Delete table or records
    int execDelete(const string &text, int *shift);

    // Quit the MiniSQL
    int execQuit(const string &text, int *shift);

    // Exec SQL file
    int execFile(const string &text, int *shift);

    Interpreter() = default;;

    ~Interpreter() = default;;

};

#endif //MINISQL_INTERPRETER_H
