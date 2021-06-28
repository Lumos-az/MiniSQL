//
// Created by 何昂展 on 2021/6/11.
//

#ifndef MINISQL_API_H
#define MINISQL_API_H

#include "RecordManager.h"
#include "CatalogManager.h"
#include "IndexManager.h"
class RecordManager;

class API {
public:
    void createTable(const string& tableName, vector<Attribute> *attributes);

    void dropTable(const string &tableName);

    void createIndex(const string &tableName, const string &attributeName, const string &indexName);

    void dropIndex(const string &tableName, const string &indexName);

    void insertValue(const string &tableName, vector<string> *tuple);
    
    void select(const string& tableName, vector<string>* attributeName, vector<Condition>* conditions);

    void deleteTable(const string &tableName);

    void deleteRecord(const string &tableName, vector<Condition> *conditions);

    void insertIndex(const string& tableName, Attribute _attribute, const string& _value, struct RecordPosition recordposition); //在插入数据之后插入index
    
    vector<RecordPosition> indexforcondition(const string& tableName, Attribute attribute, int operate, const string& _value);

    void deleteIndex(const string& tableName, Attribute _attribute, const string& _value);

    vector<Attribute> getTableAttribute(const string& tableName);

    void selectwithconditions(string tablename, vector<Attribute>* Attributes, vector<Condition>* conditions);

    void deletewithconditions(string tablename, vector<Attribute>* Attributes, vector<Condition>* conditions);

    void createNewIndex(const string& tableName, const string& attributeName, const string& indexName);

private:
    CatalogManager cm;
    RecordManager* rm;
    IndexManager im;
};


#endif //MINISQL_API_H
