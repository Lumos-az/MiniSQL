#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#include "Attribute.h"
#include "Condition.h"
#include "RecordPosition.h"
#include "API.h"

#include <string>
#include <vector>

using namespace std;
class API;

class RecordManager
{
public:
    RecordManager() {};
    ~RecordManager() {};

    bool Createtable(string tablename);

    bool DropTable(string tablename);

    struct RecordPosition InsertRecord(string tablename, string* record, int recordsize);//返回插入位置

  //  void selectwithconditions(string tablename, vector<Attribute>* Attributes, vector<Condition>* conditions);//获得满足条件数据位置
    
  //  void deletewithconditions(string tablename, vector<Attribute>*Attributes, vector<Condition>*conditions);

    bool DeleteAllRecords(string tablename, vector<Attribute>* Attributes);//删除所有记录

    void PrintAllRecord(string tablename, vector<Attribute>* Attributes);
    
    int getrecordsize(vector<Attribute>* Attributes);

    int selectwithoutindex(string tablename, vector<Attribute>* Attributes, vector<RecordPosition>* rep,vector<Condition>* conditions);

    int selectbyorders(string tablename, vector<Attribute>* Attributes, vector<Condition>* conditions);

    int deletebyorders(string tablename, vector<Attribute>* Attributes, vector<Condition>* conditions);

    int deletewithoutindex(string tablename, vector<Attribute>*Attributes, vector<RecordPosition>*rep, vector<Condition>*conditions);


private:
    API* api;

    void printouttablehead(vector<Attribute>* Attributes);

    bool isSatisfied(vector<Attribute>* Attributes, string* record, vector<Condition>* conditions);
   
    void PrintRecord(string tablename, vector<RecordPosition>* result, vector<Attribute>* Attributes);

    void putout(string str, vector<Attribute>* Attributes);

};




#endif