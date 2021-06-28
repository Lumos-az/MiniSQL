#include "RecordManager.h"


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdio.h> 

using namespace std;


bool RecordManager::Createtable(string tablename) {
    string tablefilename = "../data/" + tablename + ".txt";

    if (bufferManager.LoadFile(tablefilename) > 0) {
        bufferManager.SaveFile(tablefilename);
        return true;
    }
    else
        return false;

}

bool RecordManager::DropTable(string tablename) {
    string tablefilename = "../data/" + tablename + ".txt";
    if (!bufferManager.DeleteFile(tablefilename))
        return true;
    else
        return false;
}

struct RecordPosition RecordManager::InsertRecord(string tablename, string* record, int recordsize) {

    string tablefilename = "../data/" + tablename + ".txt";

    struct RecordPosition recordposition;
    recordposition.blockNum = 0;
    recordposition.recordNum = 0;//初始化
    if (!bufferManager.IsFileRegistered(tablefilename)) {
        bufferManager.LoadFile(tablefilename);
    }

    int i = bufferManager.GetFileSize(tablefilename);
    class Block* btmp = bufferManager.GetFileBlock(tablefilename, i - 1);
    int recordNum = (btmp->GetSize() - 4) / recordsize;

    if (btmp == NULL) {
        cout << "insert " << tablename << " record fails" << endl;
        return recordposition;
    }

    if (recordsize + 1 <= BLOCK_SIZE - btmp->GetSize()) {

        char* addr = btmp->data + btmp->GetSize();
        //addr += record;
        //string* add;
        //*add = (*addr)[0];
        //add += recordsize;
        //*add = *record;
        memcpy(addr, record->c_str(), recordsize);
        btmp->SetSize(btmp->GetSize() + recordsize);
        btmp->SetDirty(true);

        recordposition.blockNum = i;
        recordposition.recordNum = recordNum + 1;
        return recordposition;
    }
    else {
        bufferManager.SaveFile(tablefilename);
        bufferManager.AppendFile(tablefilename);
        return InsertRecord(tablename, record, recordsize);
    }


}



int RecordManager::getrecordsize(vector<Attribute>* Attributes)
{
    int length = 0;
    for (int i = 0;i < Attributes->size();i++)
        length += (*Attributes)[i].charSize;

    return length + 1;
}

void RecordManager::putout(string str, vector<Attribute>* Attributes)
{

    int p = 1;
    cout << "|";
    for (auto& Attribute : *Attributes) { //遍历所有属性

        int typesize = Attribute.charSize;
        string tem = str.substr(p, typesize);

        if (Attribute.dataType == 0) {

            int n = stoi(tem);
            cout.width(10);
            cout << n;
            cout << "|";
        }
        else if (Attribute.dataType == 1) {

            float f = stof(tem);
            cout.width(10);
            cout << f;
            cout << "|";

        }
        else {

            int k;
            for (k = 0;k < typesize;k++) {
                if (tem.at(k) != '!') break;
            }
            tem = tem.substr(k);
            cout.width(10);
            cout << tem;
            cout << "|";
        }
        p += typesize;

    }
    cout << endl;

}


bool RecordManager::isSatisfied(vector<Attribute>* Attributes, string* record, vector<Condition>* conditions)
{

    //	char str[] = *value;
    //	char *posbegin = str+1;
    //	int step=0;
    //	vector<char>* position;
    if(conditions->size() == 0)
        return true;

    int s = conditions->size();
    int n = Attributes->size();
    int attr[100];
    int position[100];
    int step = 0;


    for (int i = 0; i < s; i++)
    {
        position[i] = 0;

        for (int j = 0; j < n; j++)
        {
            step += (*Attributes)[j].charSize;
            //    cout << "step:" << step << endl;

            if (((*Attributes)[j].name) == ((*conditions)[i].name))
            {

                attr[i] = j;

                position[i] += step;
                //  cout << "position[i]" << position[i] <<endl;
                break;
            }

        }
        step = 0;
    }

    for (int k = 0;k < s;k++)
    {

        if ((*conditions)[k].value != "") {

            if ((*Attributes)[attr[k]].dataType == 0) {

                int step = (*Attributes)[attr[k]].charSize;
                string com = (*conditions)[k].value;
                string tem = (*record).substr(position[k] - step + 1, step);
                int temp = stoi(tem);
                int con = stoi(com);


                switch ((*conditions)[k].operate) {
                case 0: if (!(temp == con)) return false; break;
                case 1: if (!(temp != con)) return false; break;
                case 2: if (!(temp < con)) return false; break;
                case 3: if (!(temp > con)) return false; break;
                case 4: if (!(temp <= con)) return false; break;
                case 5: if (!(temp >= con)) return false; break;
                default: break;

                }
            }
            else if ((*Attributes)[attr[k]].dataType == 1) {

                int step = (*Attributes)[attr[k]].charSize;
                string com = (*conditions)[k].value;
                string tem = (*record).substr(position[k] - step + 1, step);
                float temp = stof(tem);
                float con = stof(com);


                switch ((*conditions)[k].operate) {
                case 0: if (!(abs(temp - con) < 1e-3)) return false; break;
                case 1: if (!(temp != con)) return false; break;
                case 2: if (!(temp < con)) return false; break;
                case 3: if (!(temp > con)) return false; break;
                case 4: if (!(temp <= con)) return false; break;
                case 5: if (!(temp >= con)) return false; break;
                default: break;

                }
            }
            else {

                int step = (*Attributes)[attr[k]].charSize;
                string con = (*conditions)[k].value;
                string temp = (*record).substr(position[k] - step + 1, step);

                int h;
                for (h = 0;h < step;h++) {
                    if (temp.at(h) != '!') break;
                }
                temp = temp.substr(h);

//                cout << "temp:" << temp << endl;
//                cout << (temp == con) << endl;
//                cout << (*conditions)[k].operate << endl;

                switch ((*conditions)[k].operate) {
                case 0: if (!(temp == con)) return false; break;
                case 1: if (temp == con)   return false; break;
                case 2: if (!(temp < con)) return false; break;
                case 3: if (!(temp > con)) return false; break;
                case 4: if (!(temp < con || temp == con)) return false; break;
                case 5: if (!(temp > con || temp == con)) return false; break;
                default: break;
                }

            }
        }




    }
    return true;

}
/**

bool RecordManager::DeleteRecordsWithPosition(string tablename, vector<Attribute>* Attributes, vector<RecordPosition>* repo)
{
    if ((*repo)[0].blockNum = -1) {
        cout << "There are no records can be seleted!" << endl;
        return false;
    }


    string tablefilename = "../data/" + tablename + ".txt";

    if (!bufferManager.IsFileRegistered(tablefilename)) {
        bufferManager.LoadFile(tablefilename);
    }

    int recordsize = getrecordsize(Attributes);
    string str;


    int n = repo->size();
    for (int i = 0;i < n;i++)
    {
        Block* btmp = bufferManager.GetFileBlock(tablefilename, (*repo)[i].blockNum - 1);
        str = (*(btmp->GetRecords((*repo)[i].recordNum - 1, (*repo)[i].recordNum, recordsize)))[0];
        str.at(0) = '#';//标记删除
        btmp->SetDirty(true);
        bufferManager.SaveFile(tablefilename);

    }
    return true;
}
**/
bool RecordManager::DeleteAllRecords(string tablename, vector<Attribute>* Attributes)
{
    string tablefilename = "../data/" + tablename + ".txt";
    if (!bufferManager.IsFileRegistered(tablefilename)) {
        bufferManager.LoadFile(tablefilename);
    }
    bufferManager.ResetFile(tablefilename);
    /* int n = bufferManager.GetFileSize(tablefilename);


    if (!bufferManager.IsFileRegistered(tablefilename)) {
        bufferManager.LoadFile(tablefilename);
    }

    for (int i = 1;i < n;i++)
    {
        bufferManager.DeleteFileLastBlock(tablefilename);
    }
    Block* btmp = bufferManager.GetFileBlock(tablefilename, 0);
    btmp->SetSize(4);
    bufferManager.SaveFile(tablefilename);
 */
    return true;

}

/**
bool RecordManager::DeleteRecordsWithConditions(string tablename, vector<Attribute>* Attributes, vector<Condition>* conditions)
{
    vector<RecordPosition> result;
//    result = *selectwithconditions(tablename, Attributes, conditions);
    if (DeleteRecordsWithPosition(tablename, Attributes, &result))
        return true;
    else
        return false;

}

**/
void RecordManager::PrintAllRecord(string tablename, vector<Attribute>* Attributes)
{
    Block* btmp;
    int recordsize = getrecordsize(Attributes);
    string tablefilename = "../data/" + tablename + ".txt";


    if (!bufferManager.IsFileRegistered(tablefilename)) {
        bufferManager.LoadFile(tablefilename);
    }

    int blocknum = bufferManager.GetFileSize(tablefilename);
    string str;
    string tem;

    for (int i = 0;i < blocknum;i++) {
        btmp = bufferManager.GetFileBlock(tablefilename, i);
        int recordnum = (btmp->GetSize() - 4) / recordsize;

        if (i == 0 && recordnum == 0) {
            cout << "The table is empty!" << endl;
            break;
        }

        for (int j = 0;j < recordnum;j++) {
            if (i == 0 & j == 0) {
                printouttablehead(Attributes);
            }
            int p = 1;
            str = (*(btmp->GetRecords(j, j + 1, recordsize)))[0];
            if (str.at(0) == '%') {

                cout << "|";
                for (auto& Attribute : *Attributes) { //遍历所有属性

                    int typesize = Attribute.charSize;
                    tem = str.substr(p, typesize);


                    if (Attribute.dataType == 0) {

                        int n = stoi(tem);
                        cout.width(10);
                        cout << n;
                        cout << "|";
                    }
                    else if (Attribute.dataType == 1) {
                        float f = stof(tem);
                        cout.width(10);
                        cout << f;
                        cout << "|";

                    }
                    else {

                        int k;
                        for (k = 0;k < typesize;k++) {
                            if (tem.at(k) != '!') break;
                        }
                        tem = tem.substr(k);
                        cout.width(10);
                        cout << tem;
                        cout << "|";
                    }

                    p = p + typesize;

                }
                cout << endl;
            }

        }
    }
}

void RecordManager::PrintRecord(string tablename, vector<RecordPosition>* result, vector<Attribute>* Attributes)
{

    int recordsize = getrecordsize(Attributes);
    string tablefilename = "../data/" + tablename + ".txt";
    int n;
    n = result->size();//n表示有几条记录
    string tem;
    string str;
    Block* btmp;
    int p = 1;//记录需输出的数据在record中的位置

    if (!bufferManager.IsFileRegistered(tablefilename)) {
        bufferManager.LoadFile(tablefilename);
    }

    if ((*result)[0].blockNum = -1) {
        cout << "There are no records satisfied!" << endl;
        return;
    }

    for (int i = 0;i < n;i++)
    {
        btmp = bufferManager.GetFileBlock(tablefilename, (*result)[i].blockNum - 1);
        str = (*(btmp->GetRecords((*result)[i].recordNum - 1, (*result)[i].recordNum, recordsize)))[0];

        cout << "|";
        for (auto& Attribute : *Attributes) { //遍历所有属性

            int typesize = Attribute.charSize;
            tem = str.substr(p, typesize);
            cout.setf(ios::left);

            if (Attribute.dataType == 0) {

                int n = stoi(tem);
                cout.width(10);
                cout << n;
                cout << "|";
            }
            else if (Attribute.dataType == 1) {
                float f = stof(tem);
                cout.width(10);
                cout << f;
                cout << "|";

            }
            else {

                int k;
                for (k = 0;k < typesize;k++) {
                    if (tem.at(k) != '!') break;
                }
                tem = tem.substr(k);
                cout.width(10);
                cout << tem;
                cout << "|";
            }
            p += typesize;

        }
        cout << endl;
    }

}

int RecordManager::selectbyorders(string tablename, vector<Attribute>* Attributes, vector<Condition>* conditions)
{
    string tablefilename = "../data/" + tablename + ".txt";

    if (!bufferManager.IsFileRegistered(tablefilename)) {
        bufferManager.LoadFile(tablefilename);
    }

    int i = bufferManager.GetFileSize(tablefilename.c_str());  //block的数量
    Block* btmp;

    int flag = 0;

    int recordNum;
    int p = 0;

    int recordsize;
    recordsize = getrecordsize(Attributes);

    string putouts;

    for (int j = 0;j < i;j++)
    {
        btmp = bufferManager.GetFileBlock(tablefilename.c_str(), j);
        if (btmp == NULL) {
            cout << "The records don't exsit!" << endl;

        }

        recordNum = (btmp->GetSize() - 4) / recordsize;

        for (int k = 0;k < recordNum;k++) //第k+1条记录
        {
            putouts = (*(btmp->GetRecords(k, k + 1, recordsize)))[0];

            if (putouts.at(0) == '%') {

                if (isSatisfied(Attributes, &putouts, conditions)) {
                    if (flag == 0) printouttablehead(Attributes);
//                    putouts[0] = '#';
//                    cout << putouts << endl;
                        putout(putouts, Attributes);
                        flag = 1;
                }

            }
        }
    }

    return flag;
}

int RecordManager::selectwithoutindex(string tablename, vector<Attribute>* Attributes, vector<RecordPosition>* rep, vector<Condition>* conditions) {
    string tablefilename = "../data/" + tablename + ".txt";

    int recordsize = getrecordsize(Attributes);

    if (!bufferManager.IsFileRegistered(tablefilename)) {
        bufferManager.LoadFile(tablefilename);
    }

    int flag = 0;

    Block* btmp;
    string str;
    for (int i = 0; i < rep->size(); i++) {
        btmp = bufferManager.GetFileBlock(tablefilename, (*rep)[i].blockNum - 1);
        str = (*(btmp->GetRecords((*rep)[i].recordNum - 1, (*rep)[i].recordNum, recordsize)))[0];

        if (str.at(0) == '%') {

            if (isSatisfied(Attributes, &str, conditions)) {
                if (flag == 0) printouttablehead(Attributes);
                putout(str, Attributes);
                flag = 1;
            }
        }
    }
    return flag;
}

int RecordManager::deletebyorders(string tablename, vector<Attribute>* Attributes, vector<Condition>* conditions)
{
    string tablefilename = "../data/" + tablename + ".txt";

    if (!bufferManager.IsFileRegistered(tablefilename)) {
        bufferManager.LoadFile(tablefilename);
    }

    int i = bufferManager.GetFileSize(tablefilename.c_str());  //block的数量
    Block* btmp;

    int flag = 0;//0表示没有符合记录的，1表示有

    int recordNum;
    int p = 0;

    int recordsize;
    recordsize = getrecordsize(Attributes);

    string str;

    for (int j = 0;j < i;j++)
    {
        btmp = bufferManager.GetFileBlock(tablefilename.c_str(), j);
        if (btmp == NULL) {
            cout << "The records don't exsit!" << endl;

        }

        recordNum = (btmp->GetSize() - 4) / recordsize;

        for (int k = 0;k < recordNum;k++) //第k+1条记录
        {
            str = (*(btmp->GetRecords(k, k + 1, recordsize)))[0];

            if (str.at(0) == '%') {

                if (isSatisfied(Attributes, &str, conditions)) {
                    char c = '#';
//                    memcpy(btmp + HEAD_SIZE + k * recordsize, &c, 1);
//                    cout << btmp->data + HEAD_SIZE + k * recordsize << endl;
                    btmp->data[HEAD_SIZE + k * recordsize] = '#';
//                    cout << btmp->data + HEAD_SIZE + k * recordsize << endl;
//                    str = (*(btmp->GetRecords(k, k + 1, recordsize)))[0];
//                    cout << str << endl;

                    btmp->SetDirty(true);
                    flag = 1;

                }
            }
        }
    }

    return flag;

}

int RecordManager::deletewithoutindex(string tablename, vector<Attribute>* Attributes, vector<RecordPosition>* rep, vector<Condition>* conditions)
{
    string tablefilename = "../data/" + tablename + ".txt";

    int flag = 0;//标记是否有记录被删除

    int recordsize = getrecordsize(Attributes);

    if (bufferManager.IsFileRegistered(tablefilename)) {
        bufferManager.LoadFile(tablefilename);
    }

    Block* btmp;
    string str;
    for (int i = 0; i < rep->size(); i++) {
        btmp = bufferManager.GetFileBlock(tablefilename.c_str(), (*rep)[i].blockNum - 1);
        str = (*(btmp->GetRecords((*rep)[i].recordNum - 1, (*rep)[i].recordNum, recordsize)))[0];
        if (str.at(0) == '%') {

            if (isSatisfied(Attributes, &str, conditions)) {
                char c = '#';
//                memcpy(btmp + HEAD_SIZE + ((*rep)[i].recordNum - 1) * recordsize, &c, 1);
                btmp->data[HEAD_SIZE + HEAD_SIZE + ((*rep)[i].recordNum - 1) * recordsize] = '#';
                btmp->SetDirty(true);
                flag = 1;
            }
        }
    }
    return flag;
}

void RecordManager::printouttablehead(vector<Attribute>* Attributes)
{

    cout << "|";
    for (int i = 0;i < Attributes->size();i++)
    {
        cout.setf(ios::left);
        cout.width(10);
        cout << (*Attributes)[i].name;
        cout << "|";

    }
    cout << endl;
}