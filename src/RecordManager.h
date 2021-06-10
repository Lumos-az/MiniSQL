//
// created by 林佳 on 2021/6/8
//

#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#include "Attribute.h"
#include "Condition.h"
#include "BufferManager.h"
#include "CatalogManager.h"
#include "IndexManager.h"
//是否还需要再写一个头文件

#include <string>
#include <vector>

using namespace std;

class RecordManager
{

		BufferManager buffer_m;
		CatalogManager catalog;
		IndexManager index_m;
	
	public:
		RecordManager(){};
		~RecordManager(){
			
			//这里需要buffermanager把已有的数据写入disk
			//删除buffer
			//删除catalog 
		};
		
		int create_table(string tname); //建立一个表 
		int drop_table(string tname); //删除一个表
		 
		int creat_index(string iname);
		int drop_index(string iname);
		int inserted_indexrecord(string tname, string iname);
		
		int insert_record(string tname, char* record) ;//插入一条记录 
		int display_record(string tname, vector<string>*attributenamevector, vector<Condition>*conditionvector) ;//展示部分记录
		int display_record(string tname) ;//展示所有记录 
		int delete_record(string tname, vector<Condition>*conditionvector); //删除所有记录 
		
		Table Select(Table& tableIn, vector<int>attrSelect, vector<int>mask, vector<Condition>& w); //带条件的查找 
	    Table Select(Table& tableIn, vector<int>attrSelect); //不带条件的查找 
		
	private:
	    int display_record_index(string tname, vector<string>*attributenamevector, Block* block, int offset);
        int display_record_block(string tname, vector<string>*attributenamevector, Block* block, vector<Condition>*conditionvector);
        int display_record_block(string tname, Block *block);

        int delete_record_block(string tname, vector<Condition>*conditionvector, Block* block);
        bool record_conditionfit(string recordBegin, int recordSize, vector<Attribute>*attributevector, vector<Condition>*conditionvector);
        bool content_conditionfit(string content, int type, Condition *condition);

        void print_record(string recordBegin, int recordSize,  vector<Attribute>*attributevector, vector<string>*attributenamevector);
        void print_record(string recordBegin, int recordSize, vector<Attribute>*attributevector);

        void print_content(char *content, int type);
	
	
	
}




#endif