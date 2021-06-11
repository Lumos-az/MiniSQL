//
// Created by 何昂展 on 2021/6/11.
//

#ifndef MINISQL_API_H
#define MINISQL_API_H

#include "RecordManager.h"
#include "CatalogManager.h"

class API {
public:
    void dropTable(const string &tableName);

    void createIndex(const string &tableName, const string &attributeName, const string &indexName);

    void dropIndex(const string &tableName, const string &indexName);

private:
    CatalogManager cm;
};


#endif //MINISQL_API_H
