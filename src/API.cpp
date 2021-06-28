//
// Created by 何昂展 on 2021/6/11.
//

#include "API.h"

void API::createTable(const string& tableName, vector<Attribute> *attributes) {
    // Create table file by RecordManager
    rm->Createtable(tableName);
    // Create TableInfo
    if (cm.createTable(tableName, attributes) != -1)
        cout << "Success to create the table: " << tableName << "!" << endl;
}


void API::dropTable(const string &tableName) {
    if(bufferManager.IsFileExist(tableName)) {
        vector<Attribute> attributes = getTableAttribute(tableName);
        // Drop all indexes by IndexManager
        for (int i = 0; i < attributes.size(); i++) {
            if (attributes[i].index != "none") im.dropIndex(tableName, attributes[i]);
        }
        // Drop all records by RecordManager
        rm->DropTable(tableName);
        // Drop the tableInfo
        cout << "Success to drop table: " << tableName << "!" << endl;
    }
    cm.dropTable(tableName);

}

void API::createIndex(const string &tableName, const string &attributeName, const string &indexName) {
    // Create index by IndexManager
    vector<Attribute> attributes = getTableAttribute(tableName);
    for (auto attribute : attributes) {
        if (attribute.name == attributeName) {
            im.createIndex(tableName, attribute);
            createNewIndex(tableName, attributeName, indexName);
        }
    }

    // Update the tableInfo
    cm.createIndex(tableName, attributeName, indexName);
}

void API::dropIndex(const string &tableName, const string &indexName) {
    // Drop index by IndexManager
    vector<Attribute> attributes = getTableAttribute(tableName);
    for (auto attribute : attributes) {
        if (attribute.index == indexName) {
            im.dropIndex(tableName, attribute);
        }
    }

    // Update the tableInfo
    cm.dropIndex(tableName, indexName);
}

void API::insertValue(const string& tableName, vector<string>* tuples)
{


    int step;
    int attrindex[10];
    int p = 0;


    vector<Attribute> Attributes;
    Attributes = getTableAttribute(tableName);

    for (int j = 0; j < Attributes.size(); j++)
    {
        step = Attributes[j].charSize;
        if (Attributes[j].index != "none") {
            attrindex[p] = j;
            p++;
        }
    }


    vector<string> str;
    for (int i = 0; i < (*tuples).size(); i++) {
        step = Attributes[i].charSize;
        string temp((*tuples)[i]);
        int interval = step - temp.length();
        if (Attributes[i].dataType == Attribute::TYPE_CHAR) {
            for (int j = 0; j < interval; ++j) {
                temp = "!" + temp;
            }
        }
        else {
            for (int j = 0; j < interval; ++j) {
                temp = "0" + temp;
            }
        }
        str.push_back(temp);
    }

//    str = (*tuples);
    string record = "%";


    for (int j = 0;j < str.size();j++)
        record.append(str[j]);

    int recordsize = record.size();

    struct RecordPosition recordposition;

    recordposition = rm->InsertRecord(tableName, &record, recordsize);
    for (int k = 0;k < p;k++) {
        insertIndex(tableName, Attributes[attrindex[k]], (*tuples)[attrindex[k]], recordposition);
    }

    cout << "Success to insert value!" << endl;
}

void API::select(const string& tableName, vector<string>* attributeName, vector<Condition>* conditions) {

    vector<Attribute> attributes = getTableAttribute(tableName);

    if (conditions == nullptr) {
        rm->PrintAllRecord(tableName, &attributes);
    }
    else {
        selectwithconditions(tableName, &attributes, conditions);
    }


}

void API::selectwithconditions(string tablename, vector<Attribute>* Attributes, vector<Condition>* conditions)
{
    int n = conditions->size();
    int s = Attributes->size();
    int recordsize = rm->getrecordsize(Attributes);

    vector<RecordPosition> repo;

    int flag = 0;//标记是否找有索引
    int flag1 = 0;//标记是否找到相应记录

    vector<Condition> condwithouindex; //将没有index的conditions放在一起

    for (int i = 0;i < n;i++)
    {
        for (int j = 0;j < s;j++) {
            if ((*conditions)[i].name == (*Attributes)[j].name)
            {
                if ((*Attributes)[j].index != "none" && flag == 0) {

                    repo = indexforcondition(tablename, (*Attributes)[j], (*conditions)[i].operate, (*conditions)[i].value); //在API里调用index搜索，返回有Index的查找条件对应数据所在位置
//                    for (auto re : repo) {
//                       cout << re.recordNum << endl;
//                    }

                    flag = 1;
                }
                else {

                    condwithouindex.push_back((*conditions)[i]);
                }

            }
        }
    }
    if (flag == 1) {

        flag1 = rm->selectwithoutindex(tablename, Attributes, &repo, &condwithouindex); //获得没有条件不在索引上的被筛选出数据的位置
    }

    else {
        flag1 = rm->selectbyorders(tablename, Attributes, conditions);

    }
    if (flag1 == 0)
    {
        cout << "No records!" << endl;
    }
}


void API::deleteTable(const string &tableName) {

    vector<Attribute> Attributes;
    Attributes = getTableAttribute(tableName);
    rm->DeleteAllRecords(tableName, &Attributes);
    cout << "Success to delete the table!" << endl;
}

void API::deleteRecord(const string &tableName, vector<Condition> *conditions) {

    vector<Attribute> Attributes;
    Attributes = getTableAttribute(tableName);
    deletewithconditions(tableName, &Attributes, conditions);
}


void API::insertIndex(const string& tableName, Attribute _attribute, const string& _value, struct RecordPosition recordposition)
{
    char* value;
    int tempInt = atoi(_value.c_str());
    float tempFloat = atof(_value.c_str());

    switch (_attribute.dataType) {
        //int
        case 0: value = (char*)&tempInt; break;
            //float
        case 1: value = (char*)&tempFloat; break;
            //string
        default: value = (char*)&_value; break;
    }
    im.insertIndex(tableName, _attribute, value, recordposition);
}

void API::deleteIndex(const string& tableName, Attribute _attribute, const string& _value)
{
    im.deleteIndex(tableName, _attribute, (char*)&_value);
}

vector<RecordPosition> API::indexforcondition(const string& tableName, Attribute attribute, int operate, const string& _value)
{
    char* value;
    int tempInt = atoi(_value.c_str());
    float tempFloat = atof(_value.c_str());

    switch (attribute.dataType) {
        //int
        case 0: value = (char*)&tempInt; break;
            //float
        case 1: value = (char*)&tempFloat; break;
            //string
        default: value = (char*)&_value; break;
    }

    return im.findIndexBlockNumber(tableName, attribute, value, operate);
}

vector<Attribute> API::getTableAttribute(const string& tableName) {
    return cm.getAttribute(tableName);
}


void API::deletewithconditions(string tablename, vector<Attribute>* Attributes, vector<Condition>* conditions)
{
    int n = conditions->size();
    int s = Attributes->size();
    int recordsize = rm->getrecordsize(Attributes);

    vector<RecordPosition> repo;

    int flag = 0;//标记是否找有索引
    int flag1 = 0;//标记是否有数据被删除


    vector<Condition> condwithouindex; //将没有index的conditions放在一起

    for (int i = 0;i < n;i++)
    {
        for (int j = 0;j < s;j++) {
            if ((*conditions)[i].name == (*Attributes)[j].name)
            {
                if ((*Attributes)[j].index != "none" && flag == 0) {

                    repo = indexforcondition(tablename, (*Attributes)[j], (*conditions)[i].operate, (*conditions)[i].value); //在API里调用index搜索，返回有Index的查找条件对应数据所在位置
                    flag = 1;
                }
                else {
                    condwithouindex.push_back((*conditions)[i]);
                }

            }
        }
    }
    if (flag == 1) {
        flag1 = rm->selectwithoutindex(tablename, Attributes, &repo, &condwithouindex); //获得没有条件不在索引上的被筛选出数据的位置
    }
    else {
        flag1 = rm->deletebyorders(tablename, Attributes, conditions);
    }

    bufferManager.SaveFile(tablename);

    if (flag1 == 0) {
        cout << "There are no records can be deleted!" << endl;
    }
    else {
        cout << "Sucessfully deleted!" << endl;
    }
}

void API::createNewIndex(const string& tableName, const string& attributeName, const string& indexName) {

    vector<Attribute> Attributes;
    Attributes = getTableAttribute(tableName);

    struct RecordPosition recordposition;

    int p = 1;//属性起始位置
    int attributeaddress;
    for (int h = 0; h < Attributes.size(); h++) {
        if (Attributes[h].name == attributeName) {
            attributeaddress = h;
            break;

        }
        p += Attributes[h].charSize;
    }

    int recordsize = rm->getrecordsize(&Attributes);

    string value;
    string tablefilename = "../data/" + tableName + ".txt";

    if (!bufferManager.IsFileRegistered(tablefilename)) {
        bufferManager.LoadFile(tablefilename);
    }

    int buffernum = bufferManager.GetFileSize(tablefilename);

    for (int i = 0; i < buffernum; i++) {

        Block *btmp = bufferManager.GetFileBlock(tablefilename, i);
        int recordNum = (btmp->GetSize() - 4) / recordsize;

        for (int j = 0; j < recordNum; j++) {
            value = (*(btmp->GetRecords(j, j + 1, recordsize)))[0];

            if (value.at(0) == '%') {
                value = value.substr(p, Attributes[attributeaddress].charSize);
                recordposition.blockNum = i + 1;
                recordposition.recordNum = j + 1;


                if (Attributes[attributeaddress].dataType == 0) {

                    int n = stoi(value);
                    string value1 = to_string(n);
                    insertIndex(tableName, Attributes[attributeaddress], value1, recordposition);

                } else if (Attributes[attributeaddress].dataType == 1) {

                    float f = stof(value);
                    string value1 = to_string(f);
                    insertIndex(tableName, Attributes[attributeaddress], value1, recordposition);
                } else {

                    int k;
                    for (k = 0; k < Attributes[attributeaddress].charSize; k++) {
                        if (value.at(k) != '!') break;
                    }
                    value = value.substr(k);
                    insertIndex(tableName, Attributes[attributeaddress], value, recordposition);

                }
            }
        }
    }
}
