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
    /* Table */
    /* tableInfo: TABLE_NAME:tableName|ATTRIBUTE_NUM:attributeNum｜PRIMARY_KEY:primaryKey|INDEX_NUM:indexNum|ATTRIBUTE_INFO_0:attributeName,dataType,charSize,primaryKey,unique,index|# */
    // Create the tableInfo of the table
    string createTableInfo(const string& tableName, vector<Attribute> *attributes);

    // Determine whether the table exists. If the table exists, return 1, else return 0
    int findTable(const string& tableName);

    // Drop the table. If success, return 1, else return 0
    int dropTable(const string& tableName);

    // Create the table. If success, return 1, else return 0
    int createTable(const string& tableName, vector<Attribute> *attributes);

    // Show the information of the table
    void showTable(const string& tableName);


    /* Attribute */
    // Return attributes of the table
    vector<Attribute> getAttribute(const string& tableName);

    // Determine whether the attribute exists in the table. If the attribute exists, return 1, else return 0
    int findAttribute(const string& tableName, const string& attributeName);


    /* Index */
    // Create the index. If success, return 1, else return 0
    int createIndex(const string& tableName, const string& attributeName, const string& indexName);

    // Drop the index. If success, return 1, else return 0
    int dropIndex(const string& tableName, const string& indexName);

    // Determine whether the index exists in the table. If the index exists, return 1, else return 0
    string findIndex(const string& tableName, const string& indexName);

    // Show all indexes in the database
    void showAllIndex();

    CatalogManager() = default;;

    ~CatalogManager() = default;;
};

#endif //MINISQL_CATALOGMANAGER_H
