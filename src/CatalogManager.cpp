//
// Created by 何昂展 on 2021/6/4.
//

#include "CatalogManager.h"
#include <fstream>
#include <iostream>
#include <cstdio>

using namespace std;

/* Find the table according the table name */
int CatalogManager::findTable(const string& tableName) {
    string path = "../data/" + tableName + ".txt";
    ifstream ifs;
    ifs.open(path, ios::in);

    // If find the table, return 1, else return 0
    if (ifs.fail())
        return 0;
    else {
        ifs.close();
        return 1;
    }
}

/* tableInfo: TABLE_NAME:tableName|ATTRIBUTE_NUM:attributeNum｜PRIMARY_KEY:primaryKey|INDEX_NUM:indexNum|ATTRIBUTE_INFO_0:attributeName,dataType,charSize,primaryKey,unique,index|# */
string CatalogManager::createTableInfo(const string &tableName, vector<Attribute> *attributes) {
    string tableInfo;

    // Store the name of the table
    tableInfo.append("TABLE_NAME:" + tableName + "|");

    // Store the number of attributes
    tableInfo.append("ATTRIBUTE_NUM:" + to_string(((*attributes).size())) + "|");

    // Store the primary key
    for (const auto& attribute : (*attributes))
        if (attribute.primaryKey) {
            tableInfo.append("PRIMARY_KEY:" + attribute.name + "|");
            break;
        }

    // Store the number of indexes
    int indexNum = 0;
    for (const auto& attribute : (*attributes)) {
        if (attribute.index != "none")
            indexNum++;
    }
    tableInfo.append("INDEX_NUM:" + to_string(indexNum) + "|");

    // Store the information of attributes
    int attributeCount = 0;
    for (const auto& attribute : (*attributes)) {
        tableInfo.append("ATTRIBUTE_INFO_" + to_string(attributeCount) + ":");
        tableInfo.append(attribute.name + "," + to_string(attribute.dataType) + "," +
                         to_string(attribute.charSize) + "," + to_string(attribute.primaryKey) + "," +
                         to_string(attribute.unique) + "," + attribute.index + "|");
        attributeCount++;
    }

    // End with "#"
    tableInfo.append("#");

    return tableInfo;
}

/* Create table */
int CatalogManager::createTable(const string& tableName, vector<Attribute> *attributes) {
    // If the table exists
    if (findTable(tableName)) {
        cout << "Table already exists!" << endl;
        return -1;
    }

    // Get table info
    string tableInfo = createTableInfo(tableName, attributes);
    cout << tableInfo << endl;

    // Write tableInfo to buffer
    bm.CreateFile(tableName);
    int size = tableInfo.size();
    int blockNum = size / BLOCK_SIZE + 1;
    Block *block = bm.GetFileBlock(tableName, 0);
    if (blockNum == 1) {
        strcpy(block->data, tableInfo.c_str());
    } else { // TableInfo should be stored in more than one block
        strcpy(block->data, tableInfo.substr(0, BLOCK_SIZE).c_str());
        int count = 1;
        while (count != blockNum) {
            bm.AppendFile(tableName);
            block = bm.GetFileBlock(tableName, count);
            strcpy(block->data, tableInfo.substr(BLOCK_SIZE * count, BLOCK_SIZE).c_str());
            count++;
        }
    }

    // Save table info to disk
    bm.SaveFile(tableName);

    cout << "Success to create the table!" << endl;
    return 1;
}

/* Drop the table */
int CatalogManager::dropTable(string tableName) {
    // If the table not exist
    if (!findTable(tableName)) {
        cout << "Table not exists!" << endl;
        return 0;
    }

    bm.DeleteFile(tableName);
}

/* Get attributes in a table according the table name */
vector<Attribute> CatalogManager::getAttribute(const string &tableName) {
    // If the table not exist
    if (!findTable(tableName)) {
        cout << "Table not exists!" << endl;
    } else {
        vector<Attribute> attributes;

        // Extract table info
        string tableInfo;
        int blockNum = bm.GetFileSize(tableName);
        for (int i = 0; i < blockNum; ++i) {
            Block *block = bm.GetFileBlock(tableName, i);
            string temp(block->data);
            int endIndex = temp.find("#");
            // Find "#" in the block
            if (endIndex != temp.npos) {
                tableInfo.append(temp.substr(0, endIndex));
                break;
            } else  // Don't find "#"
                tableInfo.append(temp);
        }

        int startIndex, endIndex;

        // Extract the number of attributes
        startIndex = (int)tableInfo.find("ATTRIBUTE_NUM");
        startIndex = (int)tableInfo.find(":", startIndex);
        endIndex = (int)tableInfo.find("|", startIndex);
        int attributeNum = stoi(tableInfo.substr(startIndex + 1, endIndex - startIndex - 1));

        for (int i = 0; i < attributeNum; ++i) {
            startIndex = (int)tableInfo.find("ATTRIBUTE_INFO_" + to_string(i));
            startIndex = (int)tableInfo.find(":", startIndex);
            endIndex  = (int)tableInfo.find("|", startIndex);
            string attributeInfo = tableInfo.substr(startIndex + 1, endIndex - startIndex - 1);

            startIndex = 0;
            endIndex = (int)attributeInfo.find(",");
            string name = attributeInfo.substr(startIndex, endIndex - startIndex);

            startIndex = endIndex + 1;
            endIndex = (int)attributeInfo.find(",");
            int dataType = stoi(attributeInfo.substr(startIndex, endIndex - startIndex));

            startIndex = endIndex + 1;
            endIndex = (int)attributeInfo.find(",");
            int charSize = stoi(attributeInfo.substr(startIndex, endIndex - startIndex));

            startIndex = endIndex + 1;
            endIndex = (int)attributeInfo.find(",");
            bool primaryKey = stoi(attributeInfo.substr(startIndex, endIndex - startIndex));

            startIndex = endIndex + 1;
            endIndex = (int)attributeInfo.find(",");
            bool unique = stoi(attributeInfo.substr(startIndex, endIndex - startIndex));

            startIndex = endIndex + 1;
            endIndex = (int)attributeInfo.size();
            string index = attributeInfo.substr(startIndex, endIndex - startIndex);

            Attribute attribute(name, dataType, unique, charSize, primaryKey, index);
            attributes.push_back(attribute);
        }
        return attributes;
    }
}

int CatalogManager::findAttribute(const string &tableName, const string &attributeName) {
    // If the table not exist
    if (!findTable(tableName)) {
        cout << "Table not exists!" << endl;
        return 0;
    }

    // Traverse the attributes to find if attributeName is exists
    vector<Attribute> attributes = getAttribute(tableName);
    for (auto & attribute : attributes) {
        if (attribute.name == attributeName)
            return 1;
    }

    return 0;
}

int CatalogManager::createIndex(const string &tableName, const string &attributeName, const string &indexName) {
    // If the table not exist
    if (!findTable(tableName)) {
        cout << "Table not exists!" << endl;
        return 0;
    }

    // If the attribute not exist
    if(!findAttribute(tableName, attributeName)) {
        cout << "Attribute not exists!" << endl;
        return 0;
    }

    // Create the index
    vector<Attribute> attributes = getAttribute(tableName);
    for (auto attribute : attributes) {
        if (attribute.name == attributeName) {
            attribute.index = indexName;
            break;
        }
    }

    // Assume table info could be stored in one block
    Block *block = bm.GetFileBlock(tableName, 0);
    string tableInfo = createTableInfo(tableName, &attributes);
    strcpy(block->data, tableInfo.c_str());
    bm.SaveFile(tableInfo);

    return 1;
}


int CatalogManager::dropIndex(const string &tableName, const string &attributeName, const string &indexName) {
    // If the table not exist
    if (!findTable(tableName)) {
        cout << "Table not exists!" << endl;
        return 0;
    }

    // If the attribute not exist
    if(!findAttribute(tableName, attributeName)) {
        cout << "Attribute not exists!" << endl;
        return 0;
    }

    // Drop the index
    vector<Attribute> attributes = getAttribute(tableName);
    for (auto attribute : attributes) {
        if (attribute.name == attributeName) {
            attribute.index = "none";
            break;
        }
    }

    // Assume table info could be stored in one block
    Block *block = bm.GetFileBlock(tableName, 0);
    string tableInfo = createTableInfo(tableName, &attributes);
    strcpy(block->data, tableInfo.c_str());
    bm.SaveFile(tableInfo);

    return 1;
}

string CatalogManager::findIndex(const string &tableName, const string &indexName) {
    // If the table not exist
    if (!findTable(tableName)) {
        cout << "Table not exists!" << endl;
        return 0;
    }

    vector<Attribute> attributes = getAttribute(tableName);
    for (const auto& attribute : attributes) {
        if (attribute.index == indexName)
            return attribute.name;
    }
    cout << indexName << "not in" << tableName << endl;
}
