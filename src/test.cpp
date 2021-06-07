//
// Created by 何昂展 on 2021/6/4.
//
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "Interpreter.h"
using namespace std;

int main() {
    Interpreter i;
    i.MainPage();

//    string line;
//    string text;
//    string blank = " ";
//    text = "";
//    do {
//        getline(cin, line);
//        text += blank;
//        text += line;
//    } while (text[text.size() - 1] != ';');
//    cout << text << endl;
//    int position = 0;
//    int* shift = &position;
//    cout << i.extractWord(text, shift) << endl;
//    cout << (i.extractWord(text, shift) == "") << endl;


//    string c = "123";
//    c = s + c;
//    for (int i = 0; i <= c.size(); ++i) {
//        cout << i << c[i] << endl;
//    }
//    string p = s.substr(1, 0);
//    cout << (p == "\n") << endl;
//    cout << (p == "") << endl;
//    cout << s.substr(2, 1) << endl;
//    cout << p << endl;

//    ifstream ifs("../data/test.txt");
//    // Determine whether the file can be opened
//    if (!ifs) {
//        cout << "executable file not found" << endl;
//        return 2;
//    }
//    string line;
//    string fileText;
//    string blank = " ";
//    while (!ifs.eof()) {
//        fileText = "";
//        do {
//            getline(ifs, line);
//            if (ifs.eof())
//                break;
//            fileText += blank;
//            fileText += line;
//        } while (fileText[fileText.size() - 1] != ';');
//        cout << fileText << endl;
//    }
}
