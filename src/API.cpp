//
// Created by 何昂展 on 2021/6/11.
//

#include "API.h"

void API::dropTable(const string &tableName) {
    // Drop all indexes by IndexManager

    // Drop all records by RecordManager

    // Drop the tableInfo
    cm.dropTable(tableName);
}

void API::createIndex(const string &tableName, const string &attributeName, const string &indexName) {
    // Create index by IndexManager


    // Update the tableInfo
    cm.createIndex(tableName, attributeName, indexName);
}

void API::dropIndex(const string &tableName, const string &indexName) {
    // Drop index by IndexManager


    // Update the tableInfo
    cm.dropIndex(tableName, indexName);
}


