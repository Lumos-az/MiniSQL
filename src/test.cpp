//
// Created by 何昂展 on 2021/6/4.
//
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include "Interpreter.h"
#include "CatalogManager.h"
using namespace std;

int main() {
//    string s = "000001.2";
//    float a = stof(s);
//    cout << a << endl;

    Interpreter i;
    i.MainPage();

//    bool flag = true;
//    cout << to_string(flag) << endl;
//
//    string s = "abbb23df";
//    char * c = new char[100];
//    strcpy(c, s.c_str());
//    cout << c << endl;
//    cout << s.find("123") << endl;
//    int start = s.find("b");
//    int end = s.find("b", start + 1);
//    cout << start << " " << end << endl;
//    cout << s.substr(start, s.size() - start) << endl;
//
//    bool a = 0;
//    cout << a << endl;

//    char *c = new char[100];
//    strcpy(c, s.c_str());
//    cout << c << endl;
//    strcpy(c, "11111111111111");
//    cout << c << endl;
//    string cs(c);
//    cout << cs << endl;

//    CatalogManager cm;
//    cout << cm.findTable("abc") << endl;
//    cout << cm.findTable("test") << endl;

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
