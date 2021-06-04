//
// Created by 何昂展 on 2021/6/4.
//

#include "Interpreter.h"
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
        fact = Interpret(text);
    }

}

/* Interpret user's commands */
int Interpreter::Interpret(string text) {
    int position = 0;
    int *shift = &position;
    string word = ExtractWord(text, shift);


}

/* Pick up single word or key symbol */
string Interpreter::ExtractWord(string text, int *shift) {
    string word;
    int start, end;
    // Skip the blanks, tab and end-of-line
    while (text[*shift] == ' ' || text[*shift] == '\t') {
        (*shift)++;
    }
    start = *shift;
    if (text[*shift] == ';') {
        (*shift)++;
        word = "";
        return word;
    }
    // Extract the key symbol
    else if (text[*shift] == '(' || text[*shift] == ',' || text[*shift] == ')') {
        (*shift)++;
        end = *shift;
        word = text.substr(start, end - start);
        return word;
    }
    // Encounter quotation mark
    else if (text[*shift] == '\'' || text[*shift] == '\"') {
        (*shift)++;
        start = *shift;
        while (text[*shift] != '\'' || text[*shift] != '\"') {
            (*shift)++;
        }
        // Can't find the corresponding quotation mark
        if (text[*shift] != '\'' && text[*shift] != '\"') {
            word = "";
            return word;
        }
        else {
            end = *shift;
            word = text.substr(start, end - start);
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

Interpreter::Interpreter() {

}

Interpreter::~Interpreter() {

}


