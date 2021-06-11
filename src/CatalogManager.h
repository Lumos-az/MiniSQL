//
// Created by 何昂展 on 2021/6/4.
//

#ifndef MINISQL_CATALOGMANAGER_H
#define MINISQL_CATALOGMANAGER_H

#include "Attribute.h"
#include "BufferManager.h"
#include <string>
#include<vector>

using namespace std;
#define TABLE_INFO_PATH "../data/TableInfo.txt"


extern BufferManager bm;

class CatalogManager {
public:

    // Table
    string createTableInfo(const string& tableName, vector<Attribute> *attributes);

    int findTable(const string& tableName);

    int dropTable(const string& tableName);

    int createTable(const string& tableName, vector<Attribute> *attributes);


    // Attribute
    vector<Attribute> getAttribute(const string& tableName);

    int findAttribute(const string& tableName, const string& attributeName);

    // Index
    int createIndex(const string& tableName, const string& attributeName, const string& indexName);

    int dropIndex(const string& tableName, const string& attributeName, const string& indexName);

    string findIndex(const string& tableName, const string& indexName);
};

#endif //MINISQL_CATALOGMANAGER_H
