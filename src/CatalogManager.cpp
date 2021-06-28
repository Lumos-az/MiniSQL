//
// Created by 何昂展 on 2021/6/4.
//

#include "CatalogManager.h"
#include <iostream>

using namespace std;

/* Find the table according the table name */
int CatalogManager::findTable(const string& tableName) {
    int fileSize = bufferManager.GetFileSize(TABLE_INFO_PATH);
    for (int i = 0; i < fileSize; ++i) {
        Block *block = bufferManager.GetFileBlock(TABLE_INFO_PATH, i);
        string content(block->data);
        int find = (int) content.find("TABLE_NAME:" + tableName);
        if (find != std::string::npos)
            return 1;
    }

    return 0;
}

/* tableInfo: TABLE_NAME:tableName|ATTRIBUTE_NUM:attributeNum｜PRIMARY_KEY:primaryKey|INDEX_NUM:indexNum|ATTRIBUTE_INFO_0:attributeName,dataType,charSize,primaryKey,unique,index|# */
string CatalogManager::createTableInfo(const string &tableName, vector<Attribute> *attributes) {
    string tableInfo;

    // Store the name of the table
    tableInfo.append("TABLE_NAME:" + tableName + "|");

    // Store the number of attributes
    tableInfo.append("ATTRIBUTE_NUM:" + to_string(((*attributes).size())) + "|");

    // Store the primary key
    bool hasKey = false;
    for (const auto& attribute : (*attributes))
        if (attribute.primaryKey) {
            hasKey = true;
            tableInfo.append("PRIMARY_KEY:" + attribute.name + "|");
            break;
        }
    if (!hasKey)
        tableInfo.append("PRIMARY_KEY:NONE|");


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

    int tableInfoLength = (int) tableInfo.length();

    // Traverse blocks to find whether there is space to store the tableInfo
    int fileSize = bufferManager.GetFileSize(TABLE_INFO_PATH);
    for (int i = 0; i < fileSize; ++i) {
        Block *block = bufferManager.GetFileBlock(TABLE_INFO_PATH, i);
        string content(block->data);
        int contentLength = (int) content.length();
        // If there is enough space for table info
        if (contentLength + tableInfoLength <= BLOCK_SIZE) {
            content.append(tableInfo);
//            cout << content << endl;
            strcpy(block->data, content.c_str());
            block->SetDirty(true);

            return 1;
        }
    }

    // No space, so need to append a block
    bufferManager.AppendFile(TABLE_INFO_PATH);
    Block *block = bufferManager.GetFileBlock(TABLE_INFO_PATH, fileSize++);
    block->SetPin(true);
    strcpy(block->data, tableInfo.c_str());


    return 1;
}

/* Show the information of the table */
void CatalogManager::showTable(const string &tableName) {
    if (!findTable(tableName))
        cout << "Table named " << tableName << " not exist!" << endl;
    else {
        vector<Attribute> attributes = getAttribute(tableName);
        cout << "TABLE NAME: " << tableName << "|ATTRIBUTE NUM: " << attributes.size() << "|";

        bool hasKey = false;
        int indexNum = 0;
        for (auto & attribute : attributes) {
            if (attribute.primaryKey)
                cout << "PRIMARY KEY: " << attribute.name << "|";
            if (attribute.index != "none")
                indexNum++;
        }
        cout << "INDEX NUM: " << indexNum << "|" << endl;

        cout << "ATTRIBUTES:" << endl;
        for (auto & attribute : attributes) {
            cout << attribute.name << " ";
            if (attribute.dataType == 0)
                cout << "int ";
            else if (attribute.dataType == 1)
                cout << "float ";
            else
                cout << "char(" << attribute.charSize << ") ";
            if (attribute.primaryKey)
                cout << "primaryKey ";
            if (attribute.unique)
                cout << "unique ";
            if (attribute.index != "none")
                cout << "index:" << attribute.index << " ";
            cout << endl;
        }
    }
}

/* Drop the table */
int CatalogManager::dropTable(const string& tableName) {
    // If the table not exist
    if (!findTable(tableName)) {
        cout << "Table not exists!" << endl;
        return 0;
    }

    int fileSize = bufferManager.GetFileSize(TABLE_INFO_PATH);


    // Traverse blocks to find the tableInfo according the tableName
    for (int i = 0; i < fileSize; ++i) {
        Block *block = bufferManager.GetFileBlock(TABLE_INFO_PATH, i);
        string content(block->data);
        int location = (int) content.find("TABLE_NAME:" + tableName);
        // If find the table name, delete it in the block
        if (location != std::string::npos) {
            string front = content.substr(0, location);
            int end = (int) content.find("#", location);
            string latter = content.substr(end + 1, content.size() - end - 1);
            string new_content;
            new_content.append(front);
            new_content.append(latter);
            content = new_content;
//            cout << content << endl;
            strcpy(block->data, content.c_str());
            block->SetDirty(true);
            break;
        }
    }

    return 1;
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
        int fileSize = bufferManager.GetFileSize(TABLE_INFO_PATH);
        int startIndex, endIndex;

        // Traverse blocks to find the tableInfo according the tableName
        for (int i = 0; i < fileSize; ++i) {
            Block *block = bufferManager.GetFileBlock(TABLE_INFO_PATH, i);
            string content(block->data);
            startIndex = (int) content.find("TABLE_NAME:" + tableName);
            // If find the table name, extract the tableInfo
            if (startIndex != std::string::npos) {
                startIndex = (int) content.find("|", startIndex);
                endIndex = (int) content.find("#", startIndex);
                tableInfo = content.substr(startIndex + 1, endIndex - startIndex - 1);
            }
        }
//        cout << tableInfo << endl;

        // Extract the number of attributes
        startIndex = (int)tableInfo.find("ATTRIBUTE_NUM");
        startIndex = (int)tableInfo.find(":", startIndex);
        endIndex = (int)tableInfo.find("|", startIndex);
        int attributeNum = stoi(tableInfo.substr(startIndex + 1, endIndex - startIndex - 1));
//        cout << "attributeNum" << attributeNum << endl;

        // Extract attributes
        for (int i = 0; i < attributeNum; ++i) {
            startIndex = (int)tableInfo.find("ATTRIBUTE_INFO_" + to_string(i));
            startIndex = (int)tableInfo.find(":", startIndex);
            endIndex  = (int)tableInfo.find("|", startIndex);
            string attributeInfo = tableInfo.substr(startIndex + 1, endIndex - startIndex - 1);

            startIndex = 0;
            endIndex = (int)attributeInfo.find(",");
            string name = attributeInfo.substr(startIndex, endIndex - startIndex);

            startIndex = endIndex + 1;
            endIndex = (int)attributeInfo.find(",", startIndex);
            int dataType = stoi(attributeInfo.substr(startIndex, endIndex - startIndex));

            startIndex = endIndex + 1;
            endIndex = (int)attributeInfo.find(",", startIndex);
            int charSize = stoi(attributeInfo.substr(startIndex, endIndex - startIndex));

            startIndex = endIndex + 1;
            endIndex = (int)attributeInfo.find(",", startIndex);
            bool primaryKey = stoi(attributeInfo.substr(startIndex, endIndex - startIndex));

            startIndex = endIndex + 1;
            endIndex = (int)attributeInfo.find(",", startIndex);
            bool unique = stoi(attributeInfo.substr(startIndex, endIndex - startIndex));

            startIndex = endIndex + 1;
            endIndex = (int)attributeInfo.size();
            string index = attributeInfo.substr(startIndex, endIndex - startIndex);

            Attribute attribute(name, dataType, unique, charSize, primaryKey, index);
            attributes.push_back(attribute);

//            cout << "name:" << attribute.name << " datatype:" << attribute.dataType << " charSize:" << attribute.charSize << " primaryKey:" << attribute.primaryKey << " Unique:" << attribute.unique << " Index:" << attribute.index << endl;
        }

        return attributes;
    }
}

/* Determine whether the attribute exists in the table */
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

/* Create the index */
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
    for (int i = 0; i < attributes.size(); i++) {
        if (attributes[i].name == attributeName) {
            attributes[i].index = indexName;
        }
    }

//    for (const auto& attribute : attributes) {
//        cout << "name:" << attribute.name << " datatype:" << attribute.dataType << " charSize:" << attribute.charSize << " primaryKey:" << attribute.primaryKey << " Unique:" << attribute.unique << " Index:" << attribute.index << endl;
//    }

    // Update the tableInfo
    dropTable(tableName);
    createTable(tableName, &attributes);

    cout << "Success to creat index " << indexName << " on " << tableName << endl;
    return 1;
}

/* Drop the index */
int CatalogManager::dropIndex(const string &tableName, const string &indexName) {
    // If the table not exist
    if (!findTable(tableName)) {
        cout << "Table named " << tableName << " not exists!" << endl;
        return 0;
    }

    // Drop the index
    vector<Attribute> attributes = getAttribute(tableName);
    for (int i = 0; i < attributes.size(); i++) {
        if (attributes[i].index == indexName) {
            attributes[i].index = "none";
            // Update the tableInfo
            dropTable(tableName);
            createTable(tableName, &attributes);
            cout << "Success to drop index " << indexName << " on " << tableName << endl;
            return 1;
        }
    }

    cout << "There is no index named " << indexName << " in table " << tableName << endl;
    return 0;
}

/* Determine whether the index is in the table */
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

/* Show the information of all indexes in the database */
void CatalogManager::showAllIndex() {
    int fileSize = bufferManager.GetFileSize(TABLE_INFO_PATH);

    // Traverse all blocks to print index info
    for (int i = 0; i < fileSize; ++i) {
        Block *block = bufferManager.GetFileBlock(TABLE_INFO_PATH, i);
        string content(block->data);
        int startIndex = (int) content.find("TABLE_NAME");
        int endIndex;

        // Extract all tableInfos in a block
        while (startIndex != std::string::npos) {
            // Extract the table name
            startIndex = (int) content.find(":", startIndex);
            endIndex = (int) content.find("|", startIndex);
            string tableName = content.substr(startIndex + 1, endIndex - startIndex - 1);

            // Extract the indexNum
            startIndex = (int) content.find("INDEX_NUM");
            startIndex = (int) content.find(":", startIndex);
            endIndex = (int) content.find("|", startIndex);
            int indexNum = stoi(content.substr(startIndex + 1, endIndex - startIndex - 1));

            if (indexNum != 0) { // If the table has index
                cout << tableName << ": ";
                vector<Attribute> attributes = getAttribute(tableName);
                for (auto & attribute : attributes)
                    if (attribute.index != "none")
                        cout << attribute.index << " on " << attribute.name << " ";
                cout << endl;
            } else {
                cout << "no index" << endl;
            }
            startIndex = (int) content.find("TABLE_NAME", startIndex);
        }
    }
}





